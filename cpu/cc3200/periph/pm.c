/*
 * Copyright (C) 2019 Wlad Meixner
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       common periph/pm functions
 *
 * @author      Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @}
 */

#include "periph/pm.h"
#include "vendor/rom.h"

#ifdef PROVIDES_PM_SET_LOWEST_CORTEXM
/**
 * @brief put device into hibernation (currently with no way to wake up)
 */
void pm_set_lowest(void)
{
    /* FIXME: this will hibernate with no way to wake up for now */

    /* write to the hibernate register */
    HWREG(HIB3P3_BASE + HIB3P3_O_MEM_HIB_REQ) = 0x1;

    /* wait for 200 uSec */
    USEC_DELAY(PRCM_OP_USEC_DELAY);
}
#endif
