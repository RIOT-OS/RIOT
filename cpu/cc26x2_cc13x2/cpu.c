/*
 * SPDX-FileCopyrightText: 2016 Leon George
 * SPDX-FileCopyrightText: 2018 Anton Gerasimov
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief       implementation of the CPU initialization
 *
 * @author      Leon M. George <leon@georgemail.eu>, Anton Gerasimov <tossel@gmail.com>
 * @}
 */

#include "cpu.h"
#include "kernel_init.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

#include "cc26x2_cc13x2_prcm.h"

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* trim device */
    setup_trim_device();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();
}
