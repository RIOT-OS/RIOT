/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include "cpu.h"
#include "arch/lpm_arch.h"

void lpm_arch_init(void)
{
    /* TODO */
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    /* TODO */
    return 0;
}

enum lpm_mode lpm_arch_get(void)
{
    /* TODO */
    return 0;
}

void lpm_arch_awake(void)
{
    /* TODO*/
}

void lpm_arch_begin_awake(void)
{
    /* TODO */
}

void lpm_arch_end_awake(void)
{
    /* TODO */
}
