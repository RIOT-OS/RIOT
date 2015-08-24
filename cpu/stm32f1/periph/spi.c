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

void spi_conf_pins(spi_t dev)
{
    gpio_t mosi, miso, clk;

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            clk = SPI_0_CLK_PIN;
            mosi = SPI_0_MOSI_PIN;
            miso = SPI_0_MISO_PIN;
            break;
#endif
        default:
            return;
    }

    /* configure pins for alternate function input (MISO) or output (MOSI, CLK) */
    gpio_init_af(clk, GPIO_AF_OUT_PP);
    gpio_init_af(mosi, GPIO_AF_OUT_PP);
    gpio_init(miso, GPIO_DIR_IN, GPIO_NOPULL);
    return 0;
}

void spi_acquire(spi_t dev)
{
    mutex_lock(&locks[dev]);
}

void spi_release(spi_t dev)
{
    mutex_unlock(&locks[dev]);
}

char spi_transfer_byte(spi_t dev, char out)
{
    SPI_TypeDef *spi;
    char tmp;

    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            spi = SPI_0_DEV;
            break;
#endif
        default:
            return 0;
    }

    while ((spi->SR & SPI_SR_TXE) == RESET);
    spi->DR = out;
    while ((spi->SR & SPI_SR_RXNE) == RESET);
    tmp = spi->DR;
    return tmp;
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
