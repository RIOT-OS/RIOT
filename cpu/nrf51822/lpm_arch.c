/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file        lpm_arch.c
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "arch/lpm_arch.h"

void lpm_arch_init(void)
{
    /* TODO: needs to be implemented */
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    (void) target;
    /* TODO: needs to be implemented */
    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    /* TODO: needs to be implemented */
    return 0;
}

void lpm_arch_awake(void)
{
    /* TODO: needs to be implemented */
}

void lpm_arch_begin_awake(void)
{
    /* TODO: needs to be implemented */
}

void lpm_arch_end_awake(void)
{
    /* TODO: needs to be implemented */
}
