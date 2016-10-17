/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

#if SPI_0_EN  || SPI_1_EN

/**
 * @brief Internal helper function to do the actual work for spi_poweroff
 */
static void _spi_poweroff(SercomSpi* spi_dev);

/**
 * @brief Internal helper function to do the actual work for spi_poweron
 */
static void _spi_poweron(SercomSpi* spi_dev);

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
    SercomSpi* spi_dev = 0;
    uint8_t sercom_gclk_id = 0;
    gpio_t pin_sclk = 0;
    gpio_t pin_miso = 0;
    gpio_t pin_mosi = 0;
    gpio_mux_t mux_sclk = 0;
    gpio_mux_t mux_miso = 0;
    gpio_mux_t mux_mosi = 0;
    spi_mosipad_t mosi_pad = 0;
    spi_misopad_t miso_pad = 0;
    uint32_t   cpha = 0;
    uint32_t   cpol = 0;
    uint32_t   f_baud = 0;
    switch (speed)
    {
    case SPI_SPEED_100KHZ:
        f_baud = 100000;
        break;
    case SPI_SPEED_400KHZ:
        f_baud = 400000;
        break;
    case SPI_SPEED_1MHZ:
        f_baud = 1000000;
        break;
    case SPI_SPEED_5MHZ:
#if CLOCK_CORECLOCK >= 5000000
        f_baud = 5000000;
        break;
#else
        return -1;
#endif
    case SPI_SPEED_10MHZ:
#if CLOCK_CORECLOCK >= 10000000
        f_baud = 10000000;
        break;
#else
        return -1;
#endif
    }
    switch (conf)
    {
    case SPI_CONF_FIRST_RISING:         /**< first data bit is transacted on the first rising SCK edge */
        cpha = 0;
        cpol = 0;
        break;
    case SPI_CONF_SECOND_RISING:        /**< first data bit is transacted on the second rising SCK edge */
        cpha = SERCOM_SPI_CTRLA_CPHA;
        cpol = 0;
        break;
    case SPI_CONF_FIRST_FALLING:        /**< first data bit is transacted on the first falling SCK edge */
        cpha = 0;
        cpol = SERCOM_SPI_CTRLA_CPOL;
        break;
    case SPI_CONF_SECOND_FALLING:       /**< first data bit is transacted on the second falling SCK edge */
        cpha = SERCOM_SPI_CTRLA_CPHA;
        cpol = SERCOM_SPI_CTRLA_CPOL;
        break;
    }
    switch (dev)
    {
#if SPI_0_EN
    case SPI_0:
        spi_dev = &SPI_0_DEV;
        sercom_gclk_id = SPI_0_GCLK_ID;
        pin_sclk = SPI_0_SCLK;
        mux_sclk = SPI_0_SCLK_MUX;
        pin_miso = SPI_0_MISO;
        mux_miso = SPI_0_MISO_MUX;
        pin_mosi = SPI_0_MOSI;
        mux_mosi = SPI_0_MOSI_MUX;
        mosi_pad = SPI_0_MOSI_PAD;
        miso_pad = SPI_0_MISO_PAD;
        break;
#endif
#if SPI_1_EN
    case SPI_1:
        spi_dev = &SPI_1_DEV;
        sercom_gclk_id = SPI_1_GCLK_ID;
        pin_sclk = SPI_1_SCLK;
        mux_sclk = SPI_1_SCLK_MUX;
        pin_miso = SPI_1_MISO;
        mux_miso = SPI_1_MISO_MUX;
        pin_mosi = SPI_1_MOSI;
        mux_mosi = SPI_1_MOSI_MUX;
        mosi_pad = SPI_1_MOSI_PAD;
        miso_pad = SPI_1_MISO_PAD;
        break;
#endif
    default:
        return -1;
    }

    /* Use the same sequence as ArduinoCore
     *  - setup pins
     *  - disable SPI
     *  - init SPI (reset, init clock NVIC, CTRLA, CTRLB)
     *  - init cpha/cpol, BAUD.reg
     *  - enable SPI
     */
    gpio_init(pin_miso, GPIO_IN_PD);
    gpio_init_mux(pin_sclk, mux_sclk);
    gpio_init_mux(pin_miso, mux_miso);
    gpio_init_mux(pin_mosi, mux_mosi);

    /* Disable spi to write confs */
    _spi_poweroff(spi_dev);

    /* reset */
    // Setting the Software Reset bit to 1
    spi_dev->CTRLA.bit.SWRST = 1;

    // Wait both bits Software Reset from CTRLA and SYNCBUSY are equal to 0
    while (spi_dev->CTRLA.bit.SWRST || spi_dev->SYNCBUSY.bit.SWRST) {}

    /* Turn on power manager for sercom */
    PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << (sercom_gclk_id - GCLK_CLKCTRL_ID_SERCOM0_CORE_Val));

    /* Setup clock */
    /* SPI using CLK GEN 0 */
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
                         GCLK_CLKCTRL_GEN_GCLK0 |
                         GCLK_CLKCTRL_ID(sercom_gclk_id));
    while (GCLK->STATUS.bit.SYNCBUSY) {}

    /* ???? init NVIC. Maybe not needed in master mode. */

    /* Master mode */
    spi_dev->CTRLA.reg |= SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
    while (spi_dev->SYNCBUSY.reg) {}// ???? not needed

    spi_dev->BAUD.bit.BAUD = (uint8_t) (((uint32_t)CLOCK_CORECLOCK) / (2 * f_baud) - 1); /* Synchronous mode*/

    spi_dev->CTRLA.reg |= SERCOM_SPI_CTRLA_DOPO(mosi_pad)
                       |  SERCOM_SPI_CTRLA_DIPO(miso_pad)
                       |  cpha
                       |  cpol;
    while (spi_dev->SYNCBUSY.reg) {}	// ???? not needed

    /* datasize 0 => 8 bits */
    spi_dev->CTRLB.reg = (SERCOM_SPI_CTRLB_CHSIZE(0) | SERCOM_SPI_CTRLB_RXEN);
    while (spi_dev->SYNCBUSY.reg) {}	// ???? Only wait for clear of spi_dev->SYNCBUSY.bit.CTRLB

    /* enable */
    _spi_poweron(spi_dev);
    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    /* TODO */
    return -1;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* TODO*/
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
    SercomSpi* spi_dev = 0;
    char tmp;

    switch(dev)
    {
#if SPI_0_EN
    case SPI_0:
        spi_dev = &(SPI_0_DEV);
        break;
#endif
#if SPI_1_EN
    case SPI_1:
        spi_dev = &(SPI_1_DEV);
        break;
#endif
    }

    while (!spi_dev->INTFLAG.bit.DRE) {} /* while data register is not empty*/
    spi_dev->DATA.bit.DATA = out;

    while (!spi_dev->INTFLAG.bit.DRE || !spi_dev->INTFLAG.bit.RXC) {} /* while receive is not complete*/
    tmp = (char)spi_dev->DATA.bit.DATA;

    if (in != NULL)
    {
        in[0] = tmp;
    }
    return 1;
}

static void _spi_poweron(SercomSpi* spi_dev)
{
    if (spi_dev == NULL) {
        return;
    }
    spi_dev->CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
    while (spi_dev->SYNCBUSY.bit.ENABLE) {}
}

void spi_poweron(spi_t dev)
{
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        _spi_poweron(&SPI_0_DEV);
        break;
#endif
#if SPI_1_EN
    case SPI_1:
        _spi_poweron(&SPI_1_DEV);
        break;
#endif
    }
}

static void _spi_poweroff(SercomSpi* spi_dev)
{
    if (spi_dev == NULL) {
        return;
    }
    spi_dev->CTRLA.bit.ENABLE = 0; /* Disable spi */
    while (spi_dev->SYNCBUSY.bit.ENABLE) {}
}

void spi_poweroff(spi_t dev)
{
    switch(dev) {
#if SPI_0_EN
    case SPI_0:
        _spi_poweroff(&SPI_0_DEV);
        break;
#endif
#if SPI_1_EN
    case SPI_1:
        _spi_poweroff(&SPI_1_DEV);
        break;
#endif
    }
}

#endif /* SPI_0_EN || SPI_1_EN */
