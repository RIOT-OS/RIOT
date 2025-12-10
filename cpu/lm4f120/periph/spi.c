/*
 * SPDX-FileCopyrightText: 2017 Marc Poulhiès
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "board.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    /* initialize device lock */
    mutex_init(&locks[bus]);

    /* trigger pin initialization */
    spi_init_pins(bus);

    /* enable clock for SSI */
    ROM_SysCtlPeripheralEnable(spi_confs[bus].ssi_sysctl);

    /* configure SSI */
    ROM_SSIDisable(spi_confs[bus].ssi_base);
    ROM_SSIClockSourceSet(spi_confs[bus].ssi_base, SSI_CLOCK_SYSTEM);

    /* disable clock for SSI */
    ROM_SysCtlPeripheralDisable(spi_confs[bus].ssi_sysctl);
}

void spi_init_pins(spi_t bus)
{
    ROM_SysCtlPeripheralEnable(spi_confs[bus].gpio_sysctl);

    ROM_GPIOPinConfigure(spi_confs[bus].pins.clk);
    ROM_GPIOPinConfigure(spi_confs[bus].pins.fss);
    ROM_GPIOPinConfigure(spi_confs[bus].pins.rx);
    ROM_GPIOPinConfigure(spi_confs[bus].pins.tx);

    ROM_GPIOPinTypeSSI(spi_confs[bus].gpio_port, spi_confs[bus].pins.mask);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);
    /* lock bus */
    mutex_lock(&locks[bus]);
    /* enable clock for SSI */
    ROM_SysCtlPeripheralEnable(spi_confs[bus].ssi_sysctl);

    /* configure SSI device */
    ROM_SSIConfigSetExpClk(spi_confs[bus].ssi_base, ROM_SysCtlClockGet(),
                           mode,
                           SSI_MODE_MASTER,
                           clk,
                           8);

    ROM_SSIEnable(spi_confs[bus].ssi_base);
}

void spi_release(spi_t bus)
{
    /* disable device and release lock */
    ROM_SSIDisable(spi_confs[bus].ssi_base);
    ROM_SysCtlPeripheralDisable(spi_confs[bus].ssi_sysctl);

    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    char *inbuf = in;
    unsigned long int tmp_out;
    const unsigned char *outbuf =
        (out != NULL) ? (const unsigned char *)out : (const unsigned char *)&tmp_out;

    /* make sure at least one input or one output buffer is given */
    assert(in != NULL || out != NULL);

    /* ROM function only works with long int */
    unsigned long long_in;

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (; len > 0; len--) {
        /* casting const away is needed because TI interface is not const-aware */
        ROM_SSIDataPut(spi_confs[bus].ssi_base, (unsigned long int) (*outbuf));

        /* wait until tx over */
        while (ROM_SSIBusy(spi_confs[bus].ssi_base)) {}

        ROM_SSIDataGet(spi_confs[bus].ssi_base, &long_in);

        /* wait until rx over */
        while (ROM_SSIBusy(spi_confs[bus].ssi_base)) {}

        if (inbuf) {
            *inbuf = (char)long_in;
            inbuf++;
        }
        if (out) {
            outbuf++;
        }
    }
    if (!cont && cs != SPI_CS_UNDEF) {
        gpio_set((gpio_t)cs);
    }
}
