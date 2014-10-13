/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        lpm_arch.c
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include "arch/lpm_arch.h"

void lpm_arch_init(void)
{
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    return 0;
}

void lpm_arch_awake(void)
{
}

void lpm_arch_begin_awake(void)
{
}

void lpm_arch_end_awake(void)
{
}
