/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_samd21
 * @{
 *
 * @file        spi.c
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
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

#include "saml21_periph.h"

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

typedef struct spi_saml21_pin {
    uint32_t pin;
    uint32_t pmux;
} spi_saml21_pin_t;

typedef struct spi_saml21 {
    SercomSpi* dev;
    uint32_t mclk;
    uint32_t gclk_id;

    spi_saml21_pin_t sclk;
    spi_saml21_pin_t miso;
    spi_saml21_pin_t mosi;

    int dipo;
    int dopo;
} spi_saml21_t;

static const spi_saml21_t spi[] = {
#if SPI_0_EN
    /* SPI device */   /* MCLK flag */        /* GLCK id */         /* SCLK */  /* MISO */  /* MOSI */ /* dipo+dopo */
    { &(SERCOM0->SPI), MCLK_APBCMASK_SERCOM0, SERCOM0_GCLK_ID_CORE, { GPIO(PA,7), 3 }, { GPIO(PA,4), 3 }, { GPIO(PA,6), 3 }, 0, 1 }
#endif
};

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    SercomSpi* spi_dev = spi[dev].dev;

    uint8_t   dopo = 0;
    uint8_t   dipo = 0;
    uint8_t   cpha = 0;
    uint8_t   cpol = 0;
    uint32_t  f_baud = 0;

    switch(speed)
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
            return -1;
        case SPI_SPEED_10MHZ:
            return -1;
    }

    switch(conf)
    {
        case SPI_CONF_FIRST_RISING: /**< first data bit is transacted on the first rising SCK edge */
            cpha = 0;
            cpol = 0;
            break;
        case SPI_CONF_SECOND_RISING:/**< first data bit is transacted on the second rising SCK edge */
            cpha = 1;
            cpol = 0;
            break;
        case SPI_CONF_FIRST_FALLING:/**< first data bit is transacted on the first falling SCK edge */
            cpha = 0;
            cpol = 1;
            break;
        case SPI_CONF_SECOND_FALLING:/**< first data bit is transacted on the second falling SCK edge */
            cpha = 1;
            cpol = 1;
            break;
    }

    /* Enable sercom4 in power manager */
    MCLK->APBCMASK.reg |= spi[dev].mclk;

    /* Setup clock */
    GCLK->PCHCTRL[ spi[dev].gclk_id ].reg =
        GCLK_PCHCTRL_CHEN |
        GCLK_PCHCTRL_GEN_GCLK0;

    while (!(GCLK->PCHCTRL[spi[dev].gclk_id].reg & GCLK_PCHCTRL_CHEN));

    /* SCLK+MOSI = output */
    gpio_init(spi[dev].sclk.pin, GPIO_DIR_OUT, GPIO_NOPULL);
    gpio_init(spi[dev].mosi.pin, GPIO_DIR_OUT, GPIO_NOPULL);
    /* MISO = input */
    gpio_init(spi[dev].miso.pin, GPIO_DIR_IN, GPIO_PULLUP);

    /*
     * Set alternate funcion (PMUX) for our ports.
     */
    gpio_init_mux(spi[dev].sclk.pin, spi[dev].sclk.pmux);
    gpio_init_mux(spi[dev].miso.pin, spi[dev].miso.pmux);
    gpio_init_mux(spi[dev].mosi.pin, spi[dev].mosi.pmux);

    /* pin pad mapping */
    dipo = spi[dev].dipo;
    dopo = spi[dev].dopo;

    /* Disable spi to write config */
    spi_dev->CTRLA.bit.ENABLE = 0;
    while (spi_dev->SYNCBUSY.reg);

    /* setup baud */
    spi_dev->BAUD.bit.BAUD = (uint8_t) (((uint32_t) GCLK_REF) / (2 * f_baud) - 1); /* Syncronous mode*/

    spi_dev->CTRLA.reg |= SERCOM_SPI_CTRLA_MODE(0x3) /* 0x2 = slave 0x3 = master */
                          |  (SERCOM_SPI_CTRLA_DOPO(dopo))
                          |  (SERCOM_SPI_CTRLA_DIPO(dipo))
                          |  (cpha << SERCOM_SPI_CTRLA_CPHA_Pos)
                          |  (cpol << SERCOM_SPI_CTRLA_CPOL_Pos);

    while (spi_dev->SYNCBUSY.reg);
    spi_dev->CTRLB.reg = (SERCOM_SPI_CTRLB_CHSIZE(0) | SERCOM_SPI_CTRLB_RXEN);
    while(spi_dev->SYNCBUSY.reg);
    spi_poweron(dev);
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
    SercomSpi* spi_dev = spi[dev].dev;

    while (!spi_dev->INTFLAG.bit.DRE); /* while data register is not empty*/
    spi_dev->DATA.bit.DATA = out;

    if (in)
    {
        while (!spi_dev->INTFLAG.bit.RXC); /* while receive is not complete*/
        *in = spi_dev->DATA.bit.DATA;
    }
    else
    {
        /* clear input byte even if we're not interested */
        spi_dev->DATA.bit.DATA;
    }
    return 1;
}

void spi_poweron(spi_t dev)
{
    SercomSpi* spi_dev = spi[dev].dev;
    spi_dev->CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
    while(spi_dev->SYNCBUSY.bit.ENABLE);
}

void spi_poweroff(spi_t dev)
{
    SercomSpi* spi_dev = spi[dev].dev;
    spi_dev->CTRLA.bit.ENABLE = 0;
    while(spi_dev->SYNCBUSY.bit.ENABLE);
}

#endif /* SPI_0_EN || SPI_1_EN */
