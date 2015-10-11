/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Attilio Dona' <attilio.dona>
 * @}
 */

#include <assert.h>
#include "cpu.h"

void cortexm_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void) {

    /* initializes the Cortex-M core */
    cortexm_init();

    PRCMCC3200MCUInit();

    /* 1 priority group */
    MAP_IntPriorityGroupingSet(0);

}
