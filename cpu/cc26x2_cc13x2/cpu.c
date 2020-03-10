/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2018 Anton Gerasimov
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief       implementation of the CPU initialization
 *
 * @author      Leon M. George <leon@georgemail.eu>, Anton Gerasimov <tossel@gmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

#ifdef MODULE_CC26X2_CC13X2_RF
#include <ti/drivers/power/PowerCC26X2.h>

#include "NoRTOS.h"
#endif

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* If we're using RF module initialize NoRTOS and TI drivers */
#ifdef MODULE_CC26X2_CC13X2_RF
    /* Get current NoRTOS configuration */
    NoRTOS_Config cfg;
    NoRTOS_getConfig(&cfg);

    /* Change interrupt used for Swi scheduling to 15 (SysTick),
     * SysTick isn't used by RIOT. */
    cfg.swiIntNum = 15;

    /* Affect the changes */
    NoRTOS_setConfig(&cfg);

    /* Start power driver */
    Power_init();

    /* Start NoRTOS */
    NoRTOS_start();
#endif

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}
