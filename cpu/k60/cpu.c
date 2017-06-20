/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include "cpu.h"
#include "board.h"
#include "periph/init.h"

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Implementation of K60 CPU initialization.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* trigger static peripheral initialization */
    periph_init();
}

/** @} */
