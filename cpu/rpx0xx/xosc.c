/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       Implementation of the crystal oscillator (XOSC)
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <assert.h>

#include "macros/units.h"
#include "vendor/RP2040.h"
#include "io_reg.h"

static inline uint32_t _xosc_conf_sartup_delay(uint32_t f_crystal_mhz, uint32_t t_stable_ms)
{
    return (((f_crystal_mhz / 1000) * t_stable_ms) + 128) / 256;
}

void xosc_start(uint32_t f_ref)
{
    assert(f_ref == MHZ(12));
    uint32_t delay = _xosc_conf_sartup_delay(f_ref, 1);
    io_reg_write_dont_corrupt(&XOSC->STARTUP.reg, delay << XOSC_STARTUP_DELAY_Pos,
                              XOSC_STARTUP_DELAY_Msk);
    io_reg_write_dont_corrupt(&XOSC->CTRL.reg, XOSC_CTRL_ENABLE_ENABLE << XOSC_CTRL_ENABLE_Pos,
                              XOSC_CTRL_ENABLE_Msk);
    while (!XOSC->STATUS.bit.STABLE) { }
}

void xosc_stop(void)
{
    io_reg_write_dont_corrupt(&XOSC->CTRL.reg, XOSC_CTRL_ENABLE_DISABLE << XOSC_CTRL_ENABLE_Pos,
                              XOSC_CTRL_ENABLE_Msk);
}
