/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include "stm32f10x.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no SPI device is defined */
#if SPI_0_EN

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
    uint16_t br_div;
    uint8_t bus_div;

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            bus_div = SPI_0_BUS_DIV;
            SPI_0_CLKEN();
            break;
#endif
        default:
            return -1;
    }

    /* configure SCK, MISO and MOSI pin */
    spi_conf_pins(dev);

    /* configure SPI bus speed */
    switch(speed) {
        case SPI_SPEED_10MHZ:
            br_div = 0x01 + bus_div;      /* actual speed: 9MHz   */
            break;
        case SPI_SPEED_5MHZ:
            br_div = 0x02 + bus_div;     /* actual speed: 4.5MHz */
            break;
        case SPI_SPEED_1MHZ:
            br_div = 0x04 + bus_div;     /* actual speed: 1.1MHz */
            break;
        case SPI_SPEED_400KHZ:
            br_div = 0x05 + bus_div;    /* actual speed: 560kHz */
            break;
        case SPI_SPEED_100KHZ:
            br_div = 0x07;              /* actual speed: 280kHz on APB2, 140KHz on APB1 */
            break;
        default:
            return -2;
    }

    /* set up SPI */
    spi->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | (conf & 0x3) | (br_div << 3);
    spi->I2SCFGR &= 0xF7FF;     /* select SPI mode */
    spi->CRCPR = 0x7;           /* reset CRC polynomial */
    /* enable the SPI device */
    spi->CR1 |= SPI_CR1_SPE;

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    /* TODO */
    return -1;
}

int spi_conf_pins(spi_t dev)
{
    GPIO_TypeDef *port[3];
    int pin[3];

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            port[0] = SPI_0_CLK_PORT;
            pin[0] = SPI_0_CLK_PIN;
            port[1] = SPI_0_MOSI_PORT;
            pin[1] = SPI_0_MOSI_PIN;
            port[2] = SPI_0_MISO_PORT;
            pin[2] = SPI_0_MISO_PIN;
            break;
#endif
        default:
            return -1;
    }

    /* configure pins for alternate function input (MISO) or output (MOSI, CLK) */
    for (int i = 0; i < 3; i++) {
        int crbitval = (i < 2) ? 0xb : 0x4;
        if (pin[i] < 8) {
            port[i]->CR[0] &= ~(0xf << (pin[i] * 4));
            port[i]->CR[0] |= (crbitval << (pin[i] * 4));
        }
        else {
            port[i]->CR[1] &= ~(0xf << ((pin[i] - 8) * 4));
            port[i]->CR[1] |= (crbitval << ((pin[i] - 8) * 4));
        }
    }

    return 0;
}

int spi_acquire(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[dev]);
    return 0;
}

int spi_release(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[dev]);
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SPI_TypeDef *spi;
    int transferred = 0;

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            break;
#endif
        default:
            return -1;
    }

    while ((spi->SR & SPI_SR_TXE) == RESET);
    spi->DR = out;
    transferred++;

    while ((spi->SR & SPI_SR_RXNE) == RESET);
    if (in != NULL) {
        *in = spi->DR;
        transferred++;
    }
    else {
        spi->DR;
    }

    /* SPI busy */
    while ((spi->SR & 0x80));
#if ENABLE_DEBUG
    if (in != NULL) {
        DEBUG("\nout: %x in: %x transferred: %x\n", out, *in, transferred);
    }
    else {
        DEBUG("\nout: %x in: was nullPointer transferred: %x\n", out, transferred);
    }
#endif /*ENABLE_DEBUG */

    return transferred;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    int transferred = 0;

    if (out != NULL) {
        DEBUG("out*: %p out: %x length: %x\n", out, *out, length);
        while (length--) {
            int ret = spi_transfer_byte(dev, *(out)++, 0);
            if (ret <  0) {
                return ret;
            }
            transferred += ret;
        }
    }
    if (in != NULL) {
        while (length--) {
            int ret = spi_transfer_byte(dev, 0, in++);
            if (ret <  0) {
                return ret;
            }
            transferred += ret;
        }
        DEBUG("in*: %p in: %x transferred: %x\n", in, *(in-transferred), transferred);
    }

    DEBUG("sent %x byte(s)\n", transferred);
    return transferred;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    spi_transfer_byte(dev, reg, NULL);
    return spi_transfer_byte(dev, out, in);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    spi_transfer_byte(dev, reg, NULL);
    return spi_transfer_bytes(dev, out, in, length);
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* slave mode not implemented, yet */
}

void spi_poweron(spi_t dev)
{
    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPI_0_CLKEN();
            SPI_0_DEV->CR1 |= SPI_CR1_SPE;   /* turn SPI peripheral on */
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPI_0_DEV->CR1 &= ~(SPI_CR1_SPE);   /* turn SPI peripheral off */
            SPI_0_CLKDIS();
            break;
#endif
    }
}

#endif /* SPI_0_EN */
