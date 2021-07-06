/*
 * Copyright 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32v
 * @{
 *
 * @brief       Implementation of the CPU power management for Gigadevice GD32V
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <stdint.h>
#include "periph/pm.h"
#include "periph/wdt.h"

void pm_set_lowest(void)
{
    __asm__ volatile ("wfi");
}

void pm_reboot(void)
{
    wdt_setup_reboot(0, 1);
    wdt_start();
    wdt_kick();
    while (1) {}
}
