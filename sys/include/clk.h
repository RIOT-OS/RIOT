/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_clk System core clock
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       System core clock utility functions
 */

#ifndef CLK_H
#define CLK_H

#include <assert.h>
#include <stdint.h>
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the current system core clock frequency in Hz.
 *
 * @returns current system core clock frequency in Hz
 */
static inline uint32_t coreclk(void) {
#if defined(CLOCK_CORECLOCK)
    return CLOCK_CORECLOCK;
#else
    extern uint32_t cpu_coreclk;
    assert(cpu_coreclk != 0);
    return cpu_coreclk;
#endif
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CLK_H */
