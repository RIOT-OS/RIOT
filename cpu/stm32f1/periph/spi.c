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
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SPI_TypeDef *spi;
    GPIO_TypeDef *clk_port, *mosi_port, *miso_port;
    int clk_pin, mosi_pin, miso_pin;
    uint16_t br_div;
    uint8_t bus_div;

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            clk_port = SPI_0_CLK_PORT;
            clk_pin = SPI_0_CLK_PIN;
            mosi_port = SPI_0_MOSI_PORT;
            mosi_pin = SPI_0_MOSI_PIN;
            miso_port = SPI_0_MISO_PORT;
            miso_pin = SPI_0_MISO_PIN;
            bus_div = SPI_0_BUS_DIV;
            SPI_0_CLKEN();
            break;
#endif
        default:
            return -1;
    }

    /* configure CLK pin */
    if (clk_pin < 8) {
        clk_port->CRL &= ~(0xf << (clk_pin * 4));
        clk_port->CRL |= (0xb << (clk_pin * 4));
    }
    else {
        clk_port->CRH &= ~(0xf << ((clk_pin - 8) * 4));
        clk_port->CRH &= (0xb << ((clk_pin - 8) * 4));
    }
    /* configure the MOSI pin */
    if (mosi_pin < 8) {
        mosi_port->CRL &= ~(0xf << (mosi_pin * 4));
        mosi_port->CRL |= (0xb << (mosi_pin * 4));
    }
    else {
        mosi_port->CRH &= ~(0xf << ((mosi_pin - 8) * 4));
        mosi_port->CRH &= (0xb << ((mosi_pin - 8) * 4));
    }
    /* configure MISO pin */
    if (miso_pin < 8) {
        miso_port->CRL &= ~(0xf << (miso_pin * 4));
        miso_port->CRL |= (0x4 << (miso_pin * 4));
    }
    else {
        miso_port->CRH &= ~(0xf << ((miso_pin - 8) * 4));
        miso_port->CRH &= (0x4 << ((miso_pin - 8) * 4));
    }

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

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SPI_TypeDef *spi;
    int transfered = 0;

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
    transfered++;

    while ((spi->SR & SPI_SR_RXNE) == RESET);
    if (in != NULL) {
        *in = spi->DR;
        transfered++;
    }
    else {
        spi->DR;
    }

    /* SPI busy */
    while ((spi->SR & 0x80));

    DEBUG("\nout: %x in: %x transfered: %x\n", out, *in, transfered);

    return transfered;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    int transfered = 0;

    if (out != NULL) {
        DEBUG("out*: %p out: %x length: %x\n", out, *out, length);
        while (length--) {
            int ret = spi_transfer_byte(dev, *(out)++, 0);
            if (ret <  0) {
                return ret;
            }
            transfered += ret;
        }
    }
    if (in != NULL) {
        while (length--) {
            int ret = spi_transfer_byte(dev, 0, in++);
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
