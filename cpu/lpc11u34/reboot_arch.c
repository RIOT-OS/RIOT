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
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include <stdio.h>

#include "arch/reboot_arch.h"
#include "cpu.h"

int reboot_arch(int mode)
{
    printf("Going into reboot, mode %i\n", mode);

    NVIC_SystemReset();

    return 0;
}
