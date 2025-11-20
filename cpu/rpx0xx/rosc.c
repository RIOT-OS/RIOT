/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
    io_reg_atomic_clear(&ROSC->FREQA, ROSC_FREQA_PASSWD_Msk);
    io_reg_atomic_clear(&ROSC->FREQB, ROSC_FREQB_PASSWD_Msk);
    /* apply settings with magic value 0x9696 */
    const uint32_t magic = 0x9696U;
    io_reg_write_dont_corrupt(&ROSC->FREQA, magic << ROSC_FREQA_PASSWD_Pos,
                              ROSC_FREQA_PASSWD_Msk);
    io_reg_write_dont_corrupt(&ROSC->FREQB, magic << ROSC_FREQB_PASSWD_Pos,
                              ROSC_FREQB_PASSWD_Msk);

    /* default divider is 16 */
    io_reg_write_dont_corrupt(&ROSC->DIV, 16 << ROSC_DIV_DIV_Pos, ROSC_DIV_DIV_Msk);
    io_reg_atomic_set(&ROSC->CTRL, ROSC_CTRL_ENABLE_ENABLE << ROSC_CTRL_ENABLE_Pos);
    while (!(ROSC->STATUS & ROSC_STATUS_STABLE_Msk)) { }
}

/**
 * @brief   Turn off the ROSC to save power.
 *          The system clock must be switched to to another lock source
 *          before the ROSC is stopped, other wise the chip will be lock up.
 */
void rosc_stop(void)
{
    io_reg_atomic_set(&ROSC->CTRL, ROSC_CTRL_ENABLE_DISABLE << ROSC_CTRL_ENABLE_Pos);
}
