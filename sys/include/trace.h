/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Freie Universität Berlin
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @brief       Trace program flows
 *
 * This module allows recording program flow traces. It is meant for debugging
 * in multi-threaded applications or when ISR's are involved.
 *
 * The `trace()` function takes an arbitrary (user chosen) uint32 value.
 * Calling the function is safe from anywhere (user code, ISR, ...) and safely
 * logs the function call time and user value in a trace buffer.
 *
 * At any point, `trace_dump()` can be used to print the trace buffer.
 *
 * The buffer has a default size of 512 entries, which can be overridden by
 * defining CONFIG_TRACE_BUFSIZE. It can be cleared using `trace_reset()`.
 * The trace buffer works like a ring-buffer. If it is full, it will start
 * overwriting from the beginning.
 *
 * Tracing is made thread safe by disabling interrupts for critical sections.
 *
 * It does incur some overhead (at least a function call, getting the current
 * time, a pair of enable/disable interrupts and a couple of memory accesses).
 *
 * Example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "trace.h"
 * ...
 * trace(<user chosen uint32 value);
 *
 * trace_dump();
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @brief       Execution tracing module API
 *
 * @file
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

#ifndef TRACE_H
#define TRACE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Add entry to trace buffer
 *
 * Adds the current time (e.g., xtimer_now_usec()) and @p val to the trace
 * buffer.
 *
 * The value parameter is not used by the trace module itself. The caller is
 * supposed to provide a meaningful value.
 * It could be e.g., `0` at the beginning of a task and `1`at the end.
 *
 * @param[in]   val     user defined value
 */
void trace(uint32_t val);

/**
 * @brief   Print the current trace buffer
 *
 * Will print the number of the trace log entry, the timestamp (first entry) or
 * relative time since last entry, and the value supplied to the `trace()` call
 * of each entry.
 *
 * Example output (after adding two traces, 3us apart, with values 0 and 1):
 *
 *     n=   0 t=  1815312 v=0x00000000
 *     n=   1 t=+       3 v=0x00000001
 */
void trace_dump(void);

/**
 * @brief   Empty the trace buffer
 */
void trace_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* TRACE_H */
/** @} */
