/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_zynq7000
 * @{
 *
 * @file
 * @brief       Implementation of the kernels reboot interface
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 *
 * @}
 */

#include <stdio.h>

#include "arch/reboot_arch.h"
#include "cpu.h"
#include "hwtimer.h"

int reboot_arch(int mode)
{
    printf("Going into reboot, mode %i\n", mode);
    /* wait 1 ms to make sure the printf is finished */
    hwtimer_wait(HWTIMER_TICKS(1000));
    NVIC_SystemReset();
    return -1;
}
