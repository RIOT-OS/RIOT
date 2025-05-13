/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       RIOT bus wrapper API implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>

#include "atwinc15x0_internal.h"
#include "bus_wrapper/include/nm_bus_wrapper.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define NM_BUS_MAX_TRX_SZ   256

tstrNmBusCapabilities egstrNmBusCapabilities =
{
    NM_BUS_MAX_TRX_SZ
};

sint8 nm_bus_init(void *arg)
{
    (void)arg;

    assert(atwinc15x0);
    assert(gpio_is_valid(atwinc15x0->params.ssn_pin));

#if !defined(CPU_ESP32) && !defined(CPU_ESP8266)
    gpio_init(atwinc15x0->params.ssn_pin, GPIO_OUT);
    gpio_set(atwinc15x0->params.ssn_pin);
#endif

    nm_bsp_reset();
    nm_bsp_sleep(1);

    return 0;
}

sint8 nm_bus_ioctl(uint8 cmd, void* params)
{
    assert(atwinc15x0);

    sint8 res = 0;
    tstrNmSpiRw *spi_params = (tstrNmSpiRw *)params;

    switch (cmd)
    {
        case NM_BUS_IOCTL_RW:
            spi_acquire(atwinc15x0->params.spi, atwinc15x0->params.ssn_pin,
                        SPI_MODE_0, atwinc15x0->params.spi_clk);
            spi_transfer_bytes(atwinc15x0->params.spi,
                               atwinc15x0->params.ssn_pin, 0,
                               spi_params->pu8InBuf,
                               spi_params->pu8OutBuf,
                               spi_params->u16Sz);
            spi_release(atwinc15x0->params.spi);
            break;

        default:
            res = M2M_ERR_BUS_FAIL;
            DEBUG("invalid ioctl cmd\n");
            break;
    }

    return res;
}

sint8 nm_bus_deinit(void)
{
    return 0;
}

sint8 nm_bus_reinit(void *arg)
{
    (void)arg;
    return 0;
}
