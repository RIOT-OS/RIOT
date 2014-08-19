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
 * @file        spi.c
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "stm32f10x.h"
#include "periph/gpio.h"
#include "spi.h"
#include "periph_conf.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* TODO: parse and use conf and speed parameter */
int spi_init_master(spi_t dev, spi_conf_t conf, uint32_t speed)
{
    SPI_TypeDef *SPIx;

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPIx = SPI_0_DEV;
            SPI_0_CLKEN();
            break;
#endif
        case SPI_UNDEFINED:
        default:
            return -1;
    }

    /* set up SPI */
    uint16_t tmp = SPIx->CR1;
    tmp &= 0x3040;  /* reset value */

    tmp |= SPI_2_LINES_FULL_DUPLEX;
    tmp |= SPI_MASTER_MODE;
    tmp |= SPI_DATA_SIZE_8B;
    tmp |= SPI_CPOL_LOW;
    tmp |= SPI_CPHA_1_EDGE;
    tmp |= SPI_NSS_SOFT;
    tmp |= SPI_BR_PRESCALER_16;
    tmp |= SPI_1ST_BIT_MSB;

    SPIx->CR1 = tmp;
    SPIx->I2SCFGR &= 0xF7FF;     /* select SPI mode */

    SPIx->CRCPR = 0x7;           /* reset CRC polynomial */

    SPIx->CR2 |= (uint16_t)(1<<7);

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    /* TODO */
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SPI_TypeDef *SPI_dev;
    int transfered = 0;

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPI_dev = SPI_0_DEV;
            break;
#endif
        case SPI_UNDEFINED:
        default:
            return -1;
    }

    while ((SPI_dev->SR & SPI_SR_TXE) == RESET);
    SPI_dev->DR = out;
    transfered += (out) ? 1 : 0;

    while ((SPI_dev->SR & SPI_SR_RXNE) == RESET);
    if (in != NULL) {
        *in = SPI_dev->DR;
        transfered += (*in) ? 1 : 0;
    }
    else {
        SPI_dev->DR;
    }

    while ((SPI_dev->SR & 0x80) == 1);

    DEBUG("\nout: %x in: %x transfered: %x\n", out, *in, transfered);

    return transfered;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, int length)
{
    int transfered = 0;
    int ret = 0;

    if (out != NULL) {
        DEBUG("out*: %p out: %x length: %x\n", out, *out, length);
        while (length--) {
            ret += spi_transfer_byte(dev, *(out)++, 0);
            if (ret <  0) {
                return ret;
            }
            transfered += ret;
        }
    }
    if (in != NULL) {
        while (length--) {
            ret += spi_transfer_byte(dev, 0, in++);
            if (ret <  0) {
                return ret;
            }
            transfered += ret;
        }
        DEBUG("in*: %p in: %x transfered: %x\n", in, *(in-transfered), transfered);
    }

    DEBUG("sent %x byte(s)\n", transfered);
    return transfered;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char *out, char *in)
{
    spi_transfer_byte(dev, reg, NULL);
    return spi_transfer_byte(dev, *out, in);
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, int length)
{
    spi_transfer_byte(dev, reg, NULL);
    return spi_transfer_bytes(dev, out, in, length);
}

int spi_poweron(spi_t dev)
{
    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPI_0_CLKEN();
            SPI_0_DEV->CR1 |= 0x0040;   /* turn SPI peripheral on */
            break;
#endif
        case SPI_UNDEFINED:
        default:
            return -1;
    }
    return 0;
}

int spi_poweroff(spi_t dev)
{
    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPI_0_CLKDIS();
            SPI_0_DEV->CR1 &= ~(0x0040);   /* turn SPI peripheral off */
            break;
#endif
        case SPI_UNDEFINED:
        default:
            return -1;
    }
    return 0;
}
