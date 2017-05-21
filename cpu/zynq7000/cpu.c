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
 * @brief       Implementation of the CPU initialisation
 *
 * @author      Simon Vincent <simon.vincent@xsilon.com>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "zynq_interrupts.h"

/**
 * @brief   Configure clock sources and the cpu frequency
 */
static void clk_init(void)
{
    /* Currently done via Xilinx bootloader */
}

void cpu_init(void)
{
    /* Initialise clock sources and generic clocks */
    clk_init();
    init_interrupts();
}
