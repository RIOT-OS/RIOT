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
 * @file        spi.c
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *              Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
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
    SercomSpi* spi_dev = 0;
    uint8_t   dopo = 0;
    uint8_t   dipo = 0;
    uint8_t   cpha = 0;
    uint8_t   cpol = 0;
    uint32_t   f_baud = 0;
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
    switch(dev)
    {
#ifdef SPI_0_EN
        case SPI_0:
            spi_dev = &SPI_0_DEV;  
            
            /* Enable sercom4 in power manager */
            PM->APBCMASK.reg |= PM_APBCMASK_SERCOM4;
            GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (SERCOM4_GCLK_ID_CORE << GCLK_CLKCTRL_ID_Pos)));

            /* Setup clock */            
            while (GCLK->STATUS.bit.SYNCBUSY);
            pmux_set(SPI_0_SCLK_PIN, F);
            pmux_set(SPI_0_MISO_PIN, F);
            pmux_set(SPI_0_MOSI_PIN, F);

            // uint32_t pinmask = (((1UL << (SPI_0_SCLK_PIN % 32)) | (1UL << (SPI_0_MISO_PIN % 32)))  >> 16);
            // /* PMUX */
            // SPI_0_PORT0.WRCONFIG.reg |= 
            //                  PORT_WRCONFIG_HWSEL
            //                 | PORT_WRCONFIG_WRPMUX
            //                 | (0x5 << PORT_WRCONFIG_PMUX_Pos)
            //                 | PORT_WRCONFIG_PMUXEN
            //                 | (pinmask << PORT_WRCONFIG_PINMASK_Pos);

            // pinmask = (1UL << (SPI_0_MOSI_PIN % 32)) >> 16;
            // SPI_0_PORT1.WRCONFIG.reg |= 
            //                  PORT_WRCONFIG_HWSEL
            //                 | PORT_WRCONFIG_WRPMUX
            //                 | (0x5 << PORT_WRCONFIG_PMUX_Pos)
            //                 | PORT_WRCONFIG_PMUXEN
            //                 | (pinmask << PORT_WRCONFIG_PINMASK_Pos);
            //SCLK+MOSI
            SPI_0_SCLK_DEV.DIRSET.reg = (1 << (SPI_0_SCLK_PIN % 32));
            SPI_0_MOSI_DEV.DIRSET.reg = (1 << (SPI_0_MOSI_PIN % 32));

            //MISO = input
            /* configure as input */
            SPI_0_MISO_DEV.DIRCLR.reg = (1<<(SPI_0_MISO_PIN % 32));
            SPI_0_MISO_DEV.PINCFG[SPI_0_MISO_PIN % 32].bit.INEN = true;

            SPI_0_MISO_DEV.OUTCLR.reg = (1 << (SPI_0_MISO_PIN % 32));
            SPI_0_MISO_DEV.PINCFG[SPI_0_MISO_PIN % 32].bit.PULLEN = true;

            dopo = SPI_0_DOPO;
            dipo  = SPI_0_DIPO;
            break;
#endif
#ifdef SPI_1_EN
        case SPI_1:
            spi_dev = &SPI_1_DEV;

            /* Enable sercom5 in power manager */
            PM->APBCMASK.reg |= PM_APBCMASK_SERCOM5;
            /* Setup clock */            /* configure GCLK0 to feed sercom5 */;
            GCLK->CLKCTRL.reg = (uint32_t)((GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | (SERCOM5_GCLK_ID_CORE << GCLK_CLKCTRL_ID_Pos)));
            while (GCLK->STATUS.reg);

            pmux_set(SPI_1_SCLK_PIN, D);
            pmux_set(SPI_1_MISO_PIN, D);
            pmux_set(SPI_1_MOSI_PIN, D);
            //SCLK+MOSI
            SPI_1_SCLK_DEV.DIRSET.reg = (1 << (SPI_1_SCLK_PIN % 32));
            SPI_1_MOSI_DEV.DIRSET.reg = (1 << (SPI_1_MOSI_PIN % 32));

            //MISO = input
            /* configure as input */
            SPI_1_MISO_DEV.DIRCLR.reg = (1<<(SPI_1_MISO_PIN % 32));
            SPI_1_MISO_DEV.PINCFG[SPI_1_MISO_PIN % 32].bit.INEN = true;

            SPI_1_MISO_DEV.OUTCLR.reg = (1 << (SPI_1_MISO_PIN % 32));
            SPI_1_MISO_DEV.PINCFG[SPI_1_MISO_PIN % 32].bit.PULLEN = true;

            dopo = SPI_1_DOPO;
            dipo  = SPI_1_DIPO;
            break;
#endif
    }
    spi_dev->CTRLA.bit.ENABLE = 0;  // Disable spi to write confs
    while (spi_dev->SYNCBUSY.reg);
    spi_dev->CTRLA.reg |= SERCOM_SPI_CTRLA_MODE_SPI_MASTER;
    while (spi_dev->SYNCBUSY.reg);
    
    spi_dev->BAUD.bit.BAUD = (uint8_t) (((uint32_t) SPI_0_F_REF) / (2 * f_baud) - 1); // Synchronous mode
    spi_dev->CTRLA.reg |= (SERCOM_SPI_CTRLA_DOPO(dopo))
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
    return 0;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    SercomSpi* spi_dev = 0;
    int transfered = 0;

    switch(dev)
    {
#ifdef SPI_0_EN
        case SPI_0:
            spi_dev = &(SPI_0_DEV);
            break;
#endif
#ifdef SPI_1_EN
        case SPI_1:
            spi_dev = &(SPI_1_DEV);
            break;
#endif
    }

    while (!spi_dev->INTFLAG.bit.DRE); // while data register is not empty
    spi_dev->DATA.bit.DATA = out;
    transfered++;
    if (in != NULL)
    {
        while (!spi_dev->INTFLAG.bit.RXC); // while receive is not complete
        *in = spi_dev->DATA.bit.DATA;
        transfered++;
    } 
    else
    {
        int volatile temp = spi_dev->DATA.reg;
    }
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

void spi_poweron(spi_t dev)
{
     switch(dev) {
 #ifdef SPI_0_EN
         case SPI_0:
            SPI_0_DEV.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
            while(SPI_0_DEV.SYNCBUSY.bit.ENABLE);
            break;
#endif
 #ifdef SPI_1_EN
         case SPI_1:
            SPI_1_DEV.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
            while(SPI_1_DEV.SYNCBUSY.bit.ENABLE);
            break;
#endif
    }
}

void spi_poweroff(spi_t dev)
{
    switch(dev) {
#ifdef SPI_0_EN
        case SPI_0:
            SPI_0_DEV.CTRLA.bit.ENABLE = 0; //Disable spi
            while(SPI_0_DEV.SYNCBUSY.bit.ENABLE);
            break;
#endif
#ifdef SPI_1_EN
        case SPI_1:
            SPI_1_DEV.CTRLA.bit.ENABLE = 0; //Disable spi
            while(SPI_1_DEV.SYNCBUSY.bit.ENABLE);
            break;
#endif
   }
}