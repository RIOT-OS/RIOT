/*
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>

#include "cc2538_ssi.h"

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"

/* guard file in case no SPI device is defined */
#if SPI_NUMOF

/* clock sources for the SSI_CC register */
#define CS_SYS_DIV            0
#define CS_IO_DIV             1

#define SSI0_MASK             (1 << 0)
#define SSI1_MASK             (1 << 1)

#ifndef SPI_DATA_BITS_NUMOF
#define SPI_DATA_BITS_NUMOF   8
#endif

#define spin_until(condition) while (!(condition)) thread_yield()

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF] = {MUTEX_INIT};

int spi_init_master(spi_t dev, spi_conf_t conf, spi_speed_t speed)
{
    cc2538_ssi_t* ssi = spi_config[dev].dev;

    if (dev >= SPI_NUMOF) {
        return -1;
    }

    /* power on the SPI device */
    spi_poweron(dev);

    /* configure SCK, MISO and MOSI pin */
    spi_conf_pins(dev);

    /* Disable the SSI and configure it for SPI master mode */
    ssi->CR1 = 0;

    /* 3. Configure the SSI clock source */
    ssi->CC = CS_SYS_DIV;

    /* 4. Configure the clock prescale divisor by writing the SSI_CPSR register.
     * frequency of the SSIClk is defined by: SSIClk = SysClk / (CPSDVSR x (1 + SCR))
     */

    const int32_t speed_lut[] = {
        [SPI_SPEED_100KHZ] =   100000 /* Hz */,
        [SPI_SPEED_400KHZ] =   400000 /* Hz */,
        [SPI_SPEED_1MHZ  ] =  1000000 /* Hz */,
        [SPI_SPEED_5MHZ  ] =  5000000 /* Hz */,
        [SPI_SPEED_10MHZ ] = 10000000 /* Hz */,
    };

    int32_t SysClk = sys_clock_freq();
    int32_t f_desired = speed_lut[speed];
    int32_t f_actual;
    int32_t err;
    int32_t best_err = INT32_MAX;
    int32_t div1;
    int32_t div2;
    int32_t best_div1 = 2;
    int32_t best_div2 = 1;

    /* System clock is first divided by CPSDVSR, then by SCR */
    for (div1 = 2; div1 <= 254; div1++) {
        div2 = SysClk;
        int32_t denom = div1 * f_desired;
        div2 += denom / 2;
        div2 /= denom;

        if (div2 < 1) {
            div2 = 1;
        }
        else if (div2 > 256) {
            div2 = 256;
        }

        f_actual = SysClk / (div1 * div2);
        err = f_actual - f_desired;
        if (err < 0) {
            err = -err;
        }
        if (err <= best_err) {
            best_div1 = div1;
            best_div2 = div2;
            best_err = err;
        }
    }

    ssi->CPSR        = best_div1;     /* CPSDVSR */
    ssi->CR0bits.SCR = best_div2 - 1; /* Serial clock rate (SCR) */

    switch (conf) {
        case SPI_CONF_FIRST_RISING:
            ssi->CR0bits.SPO = 0;
            ssi->CR0bits.SPH = 0;
            break;

        case SPI_CONF_SECOND_RISING:
            ssi->CR0bits.SPO = 0;
            ssi->CR0bits.SPH = 1;
            break;

        case SPI_CONF_FIRST_FALLING:
            ssi->CR0bits.SPO = 1;
            ssi->CR0bits.SPH = 0;
            break;

        case SPI_CONF_SECOND_FALLING:
            ssi->CR0bits.SPO = 1;
            ssi->CR0bits.SPH = 1;
            break;
    }

    ssi->CR0bits.FRF = 0;                       /* SPI protocol mode */
    ssi->CR0bits.DSS = SPI_DATA_BITS_NUMOF - 1; /* The data size */

    ssi->CR1bits.SSE = 1;

    return 0;
}

int spi_init_slave(spi_t dev, spi_conf_t conf, char(*cb)(char data))
{
    /* slave mode is not (yet) supported */
    return -1;
}

int spi_conf_pins(spi_t dev)
{
    if (dev >= SPI_NUMOF) {
        return -1;
    }

    switch ((uintptr_t)spi_config[dev].dev) {
        case (uintptr_t)SSI0:
            IOC_PXX_SEL[spi_config[dev].mosi_pin] = SSI0_TXD;
            IOC_PXX_SEL[spi_config[dev].sck_pin ] = SSI0_CLKOUT;
            IOC_PXX_SEL[spi_config[dev].cs_pin  ] = SSI0_FSSOUT;

            IOC_SSIRXD_SSI0 = spi_config[dev].miso_pin;
            break;

        case (uintptr_t)SSI1:
            IOC_PXX_SEL[spi_config[dev].mosi_pin] = SSI1_TXD;
            IOC_PXX_SEL[spi_config[dev].sck_pin ] = SSI1_CLKOUT;
            IOC_PXX_SEL[spi_config[dev].cs_pin  ] = SSI1_FSSOUT;

            IOC_SSIRXD_SSI1 = spi_config[dev].miso_pin;
            break;
    }

    IOC_PXX_OVER[spi_config[dev].mosi_pin] = IOC_OVERRIDE_OE;
    IOC_PXX_OVER[spi_config[dev].sck_pin ] = IOC_OVERRIDE_OE;
    IOC_PXX_OVER[spi_config[dev].cs_pin  ] = IOC_OVERRIDE_OE;
    IOC_PXX_OVER[spi_config[dev].miso_pin] = IOC_OVERRIDE_DIS;

    gpio_hardware_control(spi_config[dev].mosi_pin);
    gpio_hardware_control(spi_config[dev].miso_pin);
    gpio_hardware_control(spi_config[dev].sck_pin);
    gpio_hardware_control(spi_config[dev].cs_pin);

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

static char ssi_flush_input(cc2538_ssi_t *ssi)
{
    char tmp = 0;

    while (ssi->SRbits.RNE) {
        tmp = ssi->DR;
    }

    return tmp;
}

int spi_transfer_byte(spi_t dev, char out, char *in)
{
    cc2538_ssi_t* ssi = spi_config[dev].dev;
    char tmp;

    ssi_flush_input(ssi);

    /* transmit byte */
    spin_until(ssi->SRbits.TNF);
    ssi->DR = out;

    /* receive byte */
    spin_until(ssi->SRbits.RNE);
    tmp = ssi->DR;

    if (in) {
        *in = tmp;
    }

    return 1;
}

int spi_transfer_bytes(spi_t dev, char *out, char *in, unsigned int length)
{
    cc2538_ssi_t* ssi = spi_config[dev].dev;
    typeof(length) tx_n = 0, rx_n = 0;

    if (dev >= SPI_NUMOF) {
        return -1;
    }

    ssi_flush_input(ssi);

    /* transmit and receive bytes */
    while (tx_n < length) {
        spin_until(ssi->SRbits.TNF || ssi->SRbits.RNE);

        if (ssi->SRbits.TNF) {
            ssi->DR = out[tx_n];
            tx_n++;
        }
        else if (ssi->SRbits.RNE) {
            assert(rx_n < length);
            in[rx_n] = ssi->DR;
            rx_n++;
        }
    }

    /* receive remaining bytes */
    while (rx_n < length) {
        spin_until(ssi->SRbits.RNE);
        assert(rx_n < length);
        in[rx_n] = ssi->DR;
        rx_n++;
    }

    return rx_n;
}

void spi_transmission_begin(spi_t dev, char reset_val)
{
    /* slave mode is not (yet) supported */
}

void spi_poweron(spi_t dev)
{
    switch ((uintptr_t)spi_config[dev].dev) {
        case (uintptr_t)SSI0:
            /* enable SSI0 in all three power modes */
            SYS_CTRL_RCGCSSI |= SSI0_MASK;
            SYS_CTRL_SCGCSSI |= SSI0_MASK;
            SYS_CTRL_DCGCSSI |= SSI0_MASK;
            break;

        case (uintptr_t)SSI1:
            /* enable SSI1 in all three power modes */
            SYS_CTRL_RCGCSSI |= SSI1_MASK;
            SYS_CTRL_SCGCSSI |= SSI1_MASK;
            SYS_CTRL_DCGCSSI |= SSI1_MASK;
            break;
    }
}

void spi_poweroff(spi_t dev)
{
    switch ((uintptr_t)spi_config[dev].dev) {
        case (uintptr_t)SSI0:
            /* disable SSI0 in all three power modes */
            SYS_CTRL_RCGCSSI &= ~SSI0_MASK;
            SYS_CTRL_SCGCSSI &= ~SSI0_MASK;
            SYS_CTRL_DCGCSSI &= ~SSI0_MASK;
            break;

        case (uintptr_t)SSI1:
            /* disable SSI1 in all three power modes */
            SYS_CTRL_RCGCSSI &= ~SSI1_MASK;
            SYS_CTRL_SCGCSSI &= ~SSI1_MASK;
            SYS_CTRL_DCGCSSI &= ~SSI1_MASK;
            break;
    }
}

#endif /* SPI_NUMOF */
