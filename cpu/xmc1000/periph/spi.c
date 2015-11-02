/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_xmc1000
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stdint.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph/usic_ssc.h"
#include "periph/usic.h"
#include "periph/gating.h"
#include "periph_conf.h"
#include "board.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* guard file in case no SPI device was specified */
#if SPI_NUMOF

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
    [SPI_0] = MUTEX_INIT,
};

int spi_conf_pins(spi_t spi)
{
    gpio_init(spi_instance[spi].mosi_pin & 0xff,
              GPIO_DIR_OUT | (spi_instance[spi].mosi_pin >> 8), GPIO_NOPULL);

    gpio_init(spi_instance[spi].sclk_pin & 0xff,
              GPIO_DIR_OUT | (spi_instance[spi].sclk_pin >> 8), GPIO_NOPULL);

    return 0;
}

int spi_init_master(spi_t spi, spi_conf_t conf, spi_speed_t speed)
{
    usic_fdr_t fdr = { 0 };
    usic_brg_t brg = { 0 };

    /* STEP value: (640/1024) * 32Mhz = 20Mhz base clock */
    fdr.field.step = 640;
    /* DM: fractional divider mode */
    fdr.field.dm   = 2;

    /* (DCTQ+1) x (PCTQ+1) / f[SCLK]: one full phase of clock signal
     * for leading/trailing word delay */
    brg.field.ctqsel  = USIC_CTQIN_SCLK;
    brg.field.dctq    = 0;
    brg.field.pctq    = 0;

    /* polarity & phase setting */
    switch (conf) {
        case SPI_CONF_FIRST_RISING:   /* CPOL=0, CPHA=0 */
            brg.field.sclkcfg = 2;
            break;
        case SPI_CONF_SECOND_RISING:  /* CPOL=0, CPHA=1 */
            brg.field.sclkcfg = 0;
            break;
        case SPI_CONF_FIRST_FALLING:  /* CPOL=1, CPHA=0 */
            brg.field.sclkcfg = 3;
            break;
        case SPI_CONF_SECOND_FALLING: /* CPOL=1, CPHA=1 */
            brg.field.sclkcfg = 1;
            break;
        default:
            return -2;
    }

    /* 32Mhz * (640/1024) = 20Mhz / brg_pdiv / 2 = SPI speed */
    switch (speed) {
        case SPI_SPEED_100KHZ:
            brg.field.pdiv = 100;
            break;
        case SPI_SPEED_400KHZ:
            brg.field.pdiv = 25;
            break;
        case SPI_SPEED_1MHZ:
            brg.field.pdiv = 10;
            break;
        case SPI_SPEED_5MHZ:
            brg.field.pdiv = 2;
            break;
        case SPI_SPEED_10MHZ:
            brg.field.pdiv = 1;
            break;
        default:
            return -1;
    }

    /* setup & start the USIC channel */
    usic_init((usic_channel_t *)&spi_instance[spi],
              &_xmc_usic_ssc_master_controls,
              brg, fdr, USIC_MODE_SSC);

    return 0;
}

static void _spi_put(USIC_CH_TypeDef *usic, char out, bool eof)
{
    /* wait for current TBUF to become invalid */
    while (usic->TCSR & USIC_CH_TCSR_TDV_Msk) ;

    /* write byte to transmit buffer */
    usic->TBUF[eof ? 0x17 : 0x07] = out;
}

static void _spi_get(USIC_CH_TypeDef *usic, char *in)
{
    /* wait until we received a valid word */
    while (!(usic->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk | USIC_CH_RBUFSR_RDV1_Msk))) ;

    /* read received byte */
    *in = usic->RBUF;
}

int spi_transfer_byte(spi_t spi, char out, char *in)
{
    USIC_CH_TypeDef *usic = spi_instance[spi].usic;

    _spi_put(usic, out, true);

    /* wait until we received a valid word */
    while (!(usic->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk | USIC_CH_RBUFSR_RDV1_Msk))) ;

    if (in) {
        _spi_get(usic, in);
    }
    else {
        /* emulate a read from RBUF */
        usic->FMR = (1 << USIC_CH_FMR_CRDV0_Pos) | (1 << USIC_CH_FMR_CRDV1_Pos);
    }

    /*
     * check if the USIC master slave select signal is still active:
     * With manual slave select operation like RIOTs API demands it,
     * we might clear the slave select signal prematurely.
     */
    while (usic->PSR & USIC_CH_PSR_SSCMode_MSLS_Msk) ;

    return 1;
}

int spi_transfer_bytes(spi_t spi, char *out, char *in, unsigned int length)
{
    USIC_CH_TypeDef *usic = spi_instance[spi].usic;

    unsigned int fini = length - 1;

    if (in) {

        for (unsigned int i = 0; i < fini; i++) {
            _spi_put(usic, *(out + i), false);
            _spi_get(usic, in + i);
        }

        spi_transfer_byte(spi, *(out + fini), &in[fini]);

    }
    else {

        for (unsigned int i = 0; i < fini; i++) {
            _spi_put(usic, *(out + i), false);
        }

        spi_transfer_byte(spi, *(out + fini), NULL);

    }

    return length;
}

int spi_transfer_reg(spi_t dev, uint8_t reg, char out, char *in)
{
    USIC_CH_TypeDef *usic = spi_instance[dev].usic;

    while ((usic->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk | USIC_CH_RBUFSR_RDV1_Msk))) {
        usic->FMR = (1 << USIC_CH_FMR_CRDV0_Pos) | (1 << USIC_CH_FMR_CRDV1_Pos);
    }

    /* write register address */
    _spi_put(usic, reg, false);

    /* wait until we received a valid word */
    while (!(usic->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk | USIC_CH_RBUFSR_RDV1_Msk))) ;
    /* emulate read */
    usic->FMR = (1 << USIC_CH_FMR_CRDV0_Pos) | (1 << USIC_CH_FMR_CRDV1_Pos);

    /* transfer register content */
    spi_transfer_byte(dev, out, in);

    return 1;
}

int spi_transfer_regs(spi_t dev, uint8_t reg, char *out, char *in, unsigned int length)
{
    USIC_CH_TypeDef *usic = spi_instance[dev].usic;

    /* write register address */
    _spi_put(usic, reg, false);

    /* wait until we received a valid word */
    while (!(usic->RBUFSR & (USIC_CH_RBUFSR_RDV0_Msk | USIC_CH_RBUFSR_RDV1_Msk))) {
    }
    /* emulate read */
    usic->FMR = (1 << USIC_CH_FMR_CRDV0_Pos) | (1 << USIC_CH_FMR_CRDV1_Pos);

    /* transfer register content */
    spi_transfer_bytes(dev, out, in, length);

    return length;
}


int spi_acquire(spi_t spi)
{
    if (spi >= SPI_NUMOF) {
        return -1;
    }
    mutex_lock(&locks[spi]);
    return 0;
}

int spi_release(spi_t spi)
{
    if (spi >= SPI_NUMOF) {
        return -1;
    }
    mutex_unlock(&locks[spi]);
    return 0;
}



void spi_poweron(spi_t dev)
{

}

void spi_poweroff(spi_t dev)
{

}

void spi_transmission_begin(spi_t dev, char reset_val)
{

}

int spi_init_slave(spi_t dev, spi_conf_t conf, char (*cb)(char))
{
    return 0;
}

#endif /* SPI_NUMOF */
