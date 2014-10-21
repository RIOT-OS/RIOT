/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"

/* guard file in case no SPI device is defined */
#if SPI_NUMOF

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SPI_TypeDef *spi;

    /* power on the SPI device */
    spi_poweron(dev);

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            SPI_0_PORT_CLKEN();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = SPI_1_DEV;
            SPI_1_PORT_CLKEN();
            break;
#endif
        default:
            return -1;
    }

    /* configure SCK, MISO and MOSI pin */
    spi_conf_pins(dev);

    /* reset SPI configuration registers */
    spi->CR1 = 0;
    spi->CR2 = 0;
    spi->I2SCFGR = 0;       /* this makes sure SPI mode is selected */

    /* configure bus clock speed */
    switch (speed) {
        case SPI_SPEED_100KHZ:
            spi->CR1 |= (7 << 3);       /* actual clock: 187.5KHz (lowest possible) */
            break;
        case SPI_SPEED_400KHZ:
            spi->CR1 |= (6 << 3);       /* actual clock: 375KHz */
            break;
        case SPI_SPEED_1MHZ:
            spi->CR1 |= (4 << 3);       /* actual clock: 1.5MHz */
            break;
        case SPI_SPEED_5MHZ:
            spi->CR1 |= (2 << 3);       /* actual clock: 6MHz */
            break;
        case SPI_SPEED_10MHZ:
            spi->CR1 |= (1 << 3);       /* actual clock 12MHz */
    }

    /* select clock polarity and clock phase */
    spi->CR1 |= conf;
    /* select master mode */
    spi->CR1 |= SPI_CR1_MSTR;
    /* the NSS (chip select) is managed purely by software */
    spi->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    /* set data-size to 8-bit */
    spi->CR2 |= (0x7 << 8);
    /* set FIFO threshold to set RXNE when 8 bit are received */
    spi->CR2 |= SPI_CR2_FRXTH;
    /* enable the SPI device */
    spi->CR1 |= SPI_CR1_SPE;
    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    /* due to issues with the send buffer, the master mode is not (yet) supported */
    return -1;
}

int spi_conf_pins(spi_t dev)
{
    GPIO_TypeDef *port;
    int pin[3];        /* 3 pins: sck, miso, mosi */
    int af = 0;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            port = SPI_0_PORT;
            pin[0] = SPI_0_PIN_SCK;
            pin[1] = SPI_0_PIN_MISO;
            pin[2] = SPI_0_PIN_MOSI;
            af = SPI_0_PIN_AF;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            port = SPI_1_PORT;
            pin[0] = SPI_1_PIN_SCK;
            pin[1] = SPI_1_PIN_MISO;
            pin[2] = SPI_1_PIN_MOSI;
            af = SPI_1_PIN_AF;
            break;
#endif
        default:
            return -1;
    }

    /* configure pins for their correct alternate function */
    for (int i = 0; i < 3; i++) {
        port->MODER &= ~(3 << (pin[i] * 2));
        port->MODER |= (2 << (pin[i] * 2));
        int hl = (pin[i] < 8) ? 0 : 1;
        port->AFR[hl] &= ~(0xf << ((pin[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pin[i] - (hl * 8)) * 4));
    }

    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    char tmp;
    SPI_TypeDef *spi = 0;

    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            spi = SPI_1_DEV;
            break;
#endif
        default:
            return 0;
    }

    /* wait for an eventually previous byte to be readily transferred */
    while(!(spi->SR & SPI_SR_TXE));
    /* put next byte into the output register */
    *((volatile uint8_t *)(&spi->DR)) = (uint8_t)out;
    /* wait until the current byte was successfully transferred */
    while(!(spi->SR & SPI_SR_RXNE) );
    /* read response byte to reset flags */
    tmp = *((volatile uint8_t *)(&spi->DR));
    /* 'return' response byte if wished for */
    if (in) {
        *in = tmp;
    }

    return 1;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    char res;
    int count = 0;

    for (int i = 0; i < length; i++) {
        if (out) {
            count += spi_transfer_byte(dev, out[i], &res);
        }
        else {
            count += spi_transfer_byte(dev, 0, &res);
        }
        if (in) {
            in[i] = res;
        }
    }

    return count;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    spi_transfer_byte(dev, reg, 0);
    return spi_transfer_byte(dev, out, in);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    spi_transfer_byte(dev, reg, 0);
    return spi_transfer_bytes(dev, out, in, length);
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* slave mode is not (yet) supported */
}

void spi_poweron(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            SPI_0_CLKEN();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            SPI_1_CLKEN();
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch (dev) {
#if SPI_0_EN
        case SPI_0:
            while (SPI_0_DEV->SR & SPI_SR_BSY);
            SPI_0_CLKDIS();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            while (SPI_1_DEV->SR & SPI_SR_BSY);
            SPI_1_CLKDIS();
            break;
#endif
    }
}

#endif /* SPI_NUMOF */
