/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_decadriver
 * @{
 *
 * @file
 * @brief       Porting functions for decadriver like sleeping
 *
 * This file implements the functions required by decadriver
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "ztimer.h"

#include "deca_interface.h"

#include "dw3000_hw.h"
#include "dw3000_spi.h"

decaIrqStatus_t decamutexon(void)
{
    bool s = dw3000_hw_interrupt_is_enabled();
    if (s) {
        dw3000_hw_interrupt_disable();
    }
    return s;
}

void decamutexoff(decaIrqStatus_t s)
{
    if (s) {
        dw3000_hw_interrupt_enable();
    }
}

void deca_sleep(unsigned int time_ms)
{
    ztimer_sleep(ZTIMER_MSEC, time_ms);
}

/* The deca_usleep function seems to be unused or rarely used, so we do not
 * want to activate `ZTIMER_USEC` for it. Instead, we sleep to the nearest
 * time in milliseconds (rounded up). */
void deca_usleep(unsigned long time_us)
{
    unsigned long time_ms = (time_us + 999) / 1000;
    if (time_ms == 0) {
        time_ms = 1;
    }
    ztimer_sleep(ZTIMER_MSEC, time_ms);
}

static const struct dwt_spi_s _dw3000_spi_fct = {
    .readfromspi = dw3000_spi_read,
    .writetospi = dw3000_spi_write,
    .writetospiwithcrc = dw3000_spi_write_crc,
    .setslowrate = dw3000_spi_speed_slow,
    .setfastrate = dw3000_spi_speed_fast,
};

#if MODULE_DECADRIVER_DW3000
extern const struct dwt_driver_s dw3000_driver;
#elif MODULE_DECADRIVER_DW3720
extern const struct dwt_driver_s dw3720_driver;
#endif

static const struct dwt_driver_s* _tmp_ptr[] = {
#if MODULE_DECADRIVER_DW3000
    &dw3000_driver,
#elif MODULE_DECADRIVER_DW3720
    &dw3720_driver
#endif
};

const struct dwt_probe_s dw3000_probe_interf = {
    .dw = NULL,
    .spi = (void*)&_dw3000_spi_fct,
    .wakeup_device_with_io = dw3000_hw_wakeup,
    .driver_list = (struct dwt_driver_s**)_tmp_ptr,
    .dw_driver_num = 1,
};
