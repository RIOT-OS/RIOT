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

#include "cpu.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"
#include "port_util.h"
#define ENABLE_DEBUG (0)
#include "debug.h"

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    GCLK_CLKCTRL_Type clkctrl_core;
    SercomSpi* spi_dev;
    uint8_t   data_out_pad;
    uint8_t   data_in_pad;
    uint8_t   cpha;
    uint8_t   cpol;
    uint32_t   f_baud;
    /* Speed < ½ f_ref, current f_ref = 8`000`000 
    *  baud = f_ref/(2 f_baud) - 1 
    */
    switch(speed)
    {
        case SPI_SPEED_100KHZ:       /**< drive the SPI bus with 100KHz */
            f_baud = 100000;
            break;
        case SPI_SPEED_400KHZ:           /**< drive the SPI bus with 400KHz */
            f_baud = 400000;
            break;
        case SPI_SPEED_1MHZ:             /**< drive the SPI bus with 1MHz */
            f_baud = 1000000;
            break;
        case SPI_SPEED_5MHZ:             /**< drive the SPI bus with 5MHz */
            return -1;
        case SPI_SPEED_10MHZ:             /**< drive the SPI bus with 10MHz */
            return -1;
    }
    switch(conf)
    {
        case SPI_CONF_FIRST_RISING: /**< first data bit is transacted on the first rising SCK edge */
            cpha = 0;
            cpol = 0;
            break;
        case SPI_CONF_SECOND_RISING:/**< first data bit is transacted on the second rising SCK edge */
            cpha = 0;
            cpol = 1;
            break;
        case SPI_CONF_FIRST_FALLING:/**< first data bit is transacted on the first falling SCK edge */
            cpha = 1;
            cpol = 0;
            break;
        case SPI_CONF_SECOND_FALLING:/**< first data bit is transacted on the second falling SCK edge */
            
            break;
    }
    switch(dev)
    {
#ifdef SPI_0_EN
        case SPI_0:
            /* Enable sercom4 in power manager */
            PM->APBCMASK.reg |= PM_APBCMASK_SERCOM4;
            /* Setup clock */
            clkctrl_core.bit.ID = SERCOM4_GCLK_ID_CORE;
            clkctrl_core.bit.GEN = 0; //Generator 0, which at the moment is at 8Mhz
            clkctrl_core.bit.CLKEN = 1;
            clkctrl_core.bit.WRTLOCK = 0;
            GCLK->CLKCTRL = clkctrl_core;
            while(GCLK->STATUS.bit.SYNCBUSY);
            /* TODO: Setup pins */
            pmux_set(SPI_0_SCLK_PIN, F);
            pmux_set(SPI_0_MISO_PIN, F);
            pmux_set(SPI_0_MOSI_PIN, F);
            pmux_set(SPI_0_CS_PIN,   F);

            spi_dev = &SPI_0_DEV;
            data_out_pad = SPI_0_MOSI_PAD;
            data_in_pad  = SPI_0_MISO_PAD;
            break;
#endif
    }
    spi_dev->CTRLA.bit.ENABLE = 0; //Disable spi to write confs
    while(spi_dev->SYNCBUSY.bit.ENABLE);
    SERCOM_SPI_CTRLA_Type ctrla =
    {
        .bit.SWRST  = 0,
        .bit.ENABLE = 0, //Don't enable yet
        .bit.MODE   = SERCOM_SPI_CTRLA_MODE_SPI_MASTER_Val,
        .bit.RUNSTDBY = 0, //Don't run in standby
        .bit.IBON   = 0,
        .bit.DOPO   = data_out_pad,
        .bit.DIPO   = data_in_pad,
        .bit.FORM   = 0, //SPI frame without address
        .bit.CPHA   = cpha,
        .bit.CPOL   = cpol,
        .bit.DORD   = 0 // Transfer MSB first
    };
    spi_dev->CTRLA = ctrla;

    SERCOM_SPI_CTRLB_Type ctrlb = 
    {
        .bit.CHSIZE  = 0, //8 bit character size
        .bit.PLOADEN = 0, //Slave data preload disabled
        .bit.SSDE    = 0, //Not applicable in master mode
        .bit.MSSEN   = 1, //SS enabled
        .bit.AMODE   = 0, //Not applicable in master mode
        .bit.RXEN    = 1  //Enable receiver
    };
    spi_dev->CTRLB    = ctrlb;
    spi_dev->BAUD.bit.BAUD     = (uint8_t) SPI_0_F_REF / (2* f_baud) - 1; //Synchronous mode
    spi_poweron(dev);

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    /* TODO */
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SercomSpi* spi_dev;
    switch(dev)
    {
#ifdef SPI_0_EN
        case SPI_0:
            spi_dev = &(SPI_0_DEV);

#endif
    }
    while(!spi_dev->INTFLAG.bit.DRE); //while data register is not empty
    spi_dev->DATA.bit.DATA = out;
    while(!spi_dev->INTFLAG.bit.RXC); //while receive is not complete
    *in = spi_dev->DATA.bit.DATA;
    return 1; // Always transfer 1 byte, as the name of the function suggests
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

void spi_poweron(spi_t dev)
{
    
     switch(dev) {
 #ifdef SPI_0_EN
         case SPI_0:
            SPI_0_DEV.CTRLA.bit.ENABLE = true; //Enable spi
             break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPI_0_DEV.CTRLA.bit.ENABLE = false; //Disnable spi
            break;
#endif
   }
}
