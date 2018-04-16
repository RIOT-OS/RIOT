/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        pm.c
 * @brief       Implementation of the CPU power management for SiFive FE310
 *
 * @author      Ken Rabold
 * @}
 */

#include "periph/pm.h"

void pm_set_lowest(void)
{
    /* __asm__("wfi"); */
}

void pm_off(void)
{
}

void pm_reboot(void)
{
}
