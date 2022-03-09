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
 * @brief       Implementation of the ring oscillator (ROSC)
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "vendor/RP2040.h"
#include "io_reg.h"

/**
 * @brief   Start the ring oscillator in default mode.
 *          The ROSC is running at boot time but may be turned off
 *          to save power when switching to the accurate XOSC.
 *          The default ROSC provides an instable frequency of 1.8 MHz to 12 MHz.
 */
void rosc_start(void)
{
    /* set drive strengths to default 0 */
    io_reg_atomic_clear(&ROSC->FREQA.reg, ROSC_FREQA_PASSWD_Msk);
    io_reg_atomic_clear(&ROSC->FREQB.reg, ROSC_FREQB_PASSWD_Msk);
    /* apply settings with magic value 0x9696 */
    const uint32_t magic = 0x9696U;
    io_reg_write_dont_corrupt(&ROSC->FREQA.reg, magic << ROSC_FREQA_PASSWD_Pos,
                              ROSC_FREQA_PASSWD_Msk);
    io_reg_write_dont_corrupt(&ROSC->FREQB.reg, magic << ROSC_FREQB_PASSWD_Pos,
                              ROSC_FREQB_PASSWD_Msk);

    /* default divider is 16 */
    io_reg_write_dont_corrupt(&ROSC->DIV.reg, 16 << ROSC_DIV_DIV_Pos, ROSC_DIV_DIV_Msk);
    io_reg_atomic_set(&ROSC->CTRL.reg, ROSC_CTRL_ENABLE_ENABLE << ROSC_CTRL_ENABLE_Pos);
    while (!ROSC->STATUS.bit.STABLE) { }
}

/**
 * @brief   Turn off the ROSC to save power.
 *          The system clock must be switched to to another lock source
 *          before the ROSC is stopped, other wise the chip will be lock up.
 */
void rosc_stop(void)
{
    io_reg_atomic_set(&ROSC->CTRL.reg, ROSC_CTRL_ENABLE_DISABLE << ROSC_CTRL_ENABLE_Pos);
}
