/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_clk System core clock
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       System core clock utility functions
 */

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
