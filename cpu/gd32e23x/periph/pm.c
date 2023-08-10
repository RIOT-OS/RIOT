/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @brief       Implementation of the CPU power management for GD32E23x
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 * @}
 */

#include <stdint.h>
#include "periph/pm.h"
#include "periph/wdt.h"

// void pm_set_lowest(void)
// {
//     __asm__ volatile ("wfi");
// }

// void pm_reboot(void)
// {
//     wdt_setup_reboot(0, 1);
//     wdt_start();
//     wdt_kick();
//     while (1) {}
// }
