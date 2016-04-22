/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"

/* guard file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1] = MUTEX_INIT,
#endif
#if SPI_2_EN
    [SPI_2] = MUTEX_INIT
#endif
};

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
            spi->CR1 |= (7 << 3);       /* actual clock: 125KHz (lowest possible) */
            break;
        case SPI_SPEED_400KHZ:
            spi->CR1 |= (5 << 3);       /* actual clock: 500KHz */
            break;
        case SPI_SPEED_1MHZ:
            spi->CR1 |= (4 << 3);       /* actual clock: 1MHz */
            break;
        case SPI_SPEED_5MHZ:
            spi->CR1 |= (2 << 3);       /* actual clock: 4MHz */
            break;
        case SPI_SPEED_10MHZ:
            spi->CR1 |= (1 << 3);       /* actual clock 8MHz */
    }

    /* select clock polarity and clock phase */
    spi->CR1 |= conf;
    /* select master mode */
    spi->CR1 |= SPI_CR1_MSTR;
    /* the NSS (chip select) is managed purely by software */
    spi->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    /* enable the SPI device */
    spi->CR1 |= SPI_CR1_SPE;
    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char data))
{
    /* due to issues with the send buffer, the slave mode is not (yet) supported */
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
        port->OSPEEDR |= (3 << (pin[i] * 2));
        int hl = (pin[i] < 8) ? 0 : 1;
        port->AFR[hl] &= ~(0xf << ((pin[i] - (hl * 8)) * 4));
        port->AFR[hl] |= (af << ((pin[i] - (hl * 8)) * 4));
    }

    return 0;
}

int spi_acquire(spi_t dev)
{
    if ((unsigned int)dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if ((unsigned int)dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
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
    while(!(spi->SR & SPI_SR_TXE)) {}
    /* put next byte into the output register */
    spi->DR = out;
    /* wait until the current byte was successfully transferred */
    while(!(spi->SR & SPI_SR_RXNE)) {}
    /* read response byte to reset flags */
    tmp = spi->DR;
    /* 'return' response byte if wished for */
    if (in) {
        *in = tmp;
    }

#if ENABLE_DEBUG
    if (in != NULL) {
        DEBUG("\nout: %x in: %x \n", out, *in, transferred);
    }
    else {
        DEBUG("\nout: %x in: was nullPointer\n", out, transferred);
    }
#endif /*ENABLE_DEBUG */

    return 1;
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
            while (SPI_0_DEV->SR & SPI_SR_BSY) {}
            SPI_0_CLKDIS();
            break;
#endif
#if SPI_1_EN
        case SPI_1:
            while (SPI_1_DEV->SR & SPI_SR_BSY) {}
            SPI_1_CLKDIS();
            break;
#endif
    }
}

#endif /* SPI_NUMOF */
