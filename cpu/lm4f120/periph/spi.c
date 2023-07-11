/*
 * Copyright (C) 2017 Marc Poulhiès
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Marc Poulhiès <dkm@kataplop.net>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "macros/math.h"
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

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;
    /* SSIClk = SysClk / (CPSDVSR * (1 + SCR)), with
     * CPSDVSR = 2..254 and even,
     *  SCR = 0..255 */

    uint32_t sys_ctl_clock = ROM_SysCtlClockGet();

    /* bound divider to 65024 (254 * (1 + 255)) */
    if (freq >= DIV_ROUND_UP(sys_ctl_clock, 65024)) {
        return (spi_clk_t){ .err = -EDOM };
    }

    uint8_t cpsdvsr = 2, scr = 0;
    uint32_t divider = DIV_ROUND_UP(sys_ctl_clock, freq);
    if (divider % 2) {
        divider++;
    }
    while (divider / cpsdvsr > 256) {
        cpsdvsr += 2;
    }
    scr = divider / cpsdvsr - 1;

    return (spi_clk_t){
        .bit_rate = DIV_ROUND_UP(sys_ctl_clock, (cpsdvsr * (1 + scr)))
    };
}

int32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    if (clk.err) {
        return -EINVAL;
    }
    return clk.bit_rate;
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);
    if (clk.err) {
        return;
    }

    /* lock bus */
    mutex_lock(&locks[bus]);

    /* enable clock for SSI */
    ROM_SysCtlPeripheralEnable(spi_confs[bus].ssi_sysctl);

    /* configure SSI device */
    ROM_SSIConfigSetExpClk(spi_confs[bus].ssi_base, ROM_SysCtlClockGet(),
                           mode,
                           SSI_MODE_MASTER,
                           clk.bit_rate,
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
