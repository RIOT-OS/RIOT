/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>
#include <stdint.h>

#define PWRCONT (*(volatile uint32_t *)0xC0010224)
#define PWRMODE (*(volatile uint32_t *)0xC0010228)

/**
 * @brief Do software reset the cpu
 */
void pm_reboot(void)
{
    /* Enable software reset (SWRSTENB = 1) */
    PWRCONT |= (1 << 3);

    /* Do software reset */
    PWRMODE |= (1 << 12);
}

/**
 * @brief to save cpu power
 */
void pm_set_lowest(void)
{

}
