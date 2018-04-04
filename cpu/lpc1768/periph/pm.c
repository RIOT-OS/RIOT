/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the power management peripheral
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include "periph/pm.h"

void pm_set(unsigned mode)
{
    switch (mode) {
        case 0:
            /* enter power down mode */
            LPC_SC->PCON = 0x01;
            cortexm_sleep(1);
            break;
        case 1:
            /* enter deep sleep mode */
            LPC_SC->PCON = 0x00;
            cortexm_sleep(1);
            break;
        default:
            /* enter sleep mode */
            LPC_SC->PCON = 0x00;
            cortexm_sleep(0);
    }
}

void pm_off(void)
{
    /* enter deep power down mode */
    LPC_SC->PCON = 0x03;
    cortexm_sleep(1);
}
