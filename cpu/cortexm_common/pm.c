/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       common periph/pm functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "periph/pm.h"

#ifndef FEATURES_PERIPH_PM
void pm_set_lowest(void)
{
    /* Executes a device DSB (Data Synchronization Barrier) */
    __DSB();
    /* Enter standby mode */
    __WFI();
}
#endif

void pm_reboot(void)
{
    NVIC_SystemReset();
}
