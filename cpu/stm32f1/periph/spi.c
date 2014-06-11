/*
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "stm32f10x.h"
#include "spi.h"
#include "periph_conf.h"

int spi_init_master(spi_t dev, spi_conf_t conf, uint32_t speed)
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

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
            SPI_dev = SPI1;
#endif
        case SPI_UNDEFINED:
        default:
            return -1;
    }

    if (out != 0) {
        /* wait for empty tx buffer */
        while ((SPI_dev->SR & 0x2) == 0);
        /* write out data to buffer */
        SPI_dev->DR = out;
        /* increase transfered bytes counter */
        transfered++;
    }

    if (in != 0) {
        /* wait for not empty rx buffer */
        while ((SPI_dev->SR & 0x1) == 0);
        /* read out data to in buffer */
        *in = SPI_dev->DR;
        /* increase transfered bytes counter */
        transfered++;
    }

    while ((SPI_dev->SR & 0x80) == SET);

    return transfered;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, int length)
{
    int transfered = 0;
    int ret = 0;

    if (out != 0) {
        while (length--) {
            ret += spi_transfer_byte(dev, *(out)++, 0);
            if (ret <  0) {
                return ret;
            }
            transfered += ret;
        }
    }
    if (in != 0) {
        while (length--) {
            ret += spi_transfer_byte(dev, 0, in++);
            if (ret <  0) {
                return ret;
            }
            transfered += ret;
        }
    }

    return transfered;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char *out, char *in)
{
    return -1;
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, int length)
{
    return -1;
}

int spi_poweron(spi_t dev)
{
    return -1;
}

int spi_poweroff(spi_t dev)
{
    return -1;
}
