/*
 * Copyright 2020 Francois Berder
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_mips_pic32_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief        common periph/pm functions
 *
 * @author       Francois Berder <fberder@outlook.fr >
 *
 * @}
 */

#include "periph/pm.h"

void pm_reboot(void)
{
    /* Unlock OSCCON */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    /* Set SWRST bit to arm reset */
    RSWRSTSET = 1;

    /* Read RSWRST register to trigger reset */
    RSWRST;

    while(1);
}
