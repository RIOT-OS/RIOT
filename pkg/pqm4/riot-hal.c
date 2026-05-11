/*
 * SPDX-FileCopyrightText: 2026 Mert Cakir <mert-cakir@outlook.com>
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: CC0-1.0
 */

/**
 * @ingroup pkg_pqm4
 * @{
 *
 * @file
 * @brief   RIOT-side adapter for pqm4's mupq/common/hal.h contract.
 *
 * @author  Mert Cakir <mert-cakir@outlook.com>
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "thread.h"

enum clock_mode {
    CLOCK_FAST,
    CLOCK_BENCHMARK
};

void hal_setup(const enum clock_mode clock)
{
    (void)clock;
    /* RIOT board init has already set the system clock via cpu_init(). */
}

void hal_send_str(const char *in)
{
    puts(in);
}

uint64_t hal_get_time(void)
{
    /* Stub. The pqm4 crypto code does not call this; only the pqm4
     * benchmark harness does, which is not linked into RIOT
     * applications. */
    return 0;
}

size_t hal_get_stack_size(void)
{
    return thread_get_stacksize(thread_get_active());
}

void hal_spraystack(void)
{
    /* Intentional stub. Use RIOT's thread_measure_stack_free() if you
     * need actual peak-stack measurement. */
}

size_t hal_checkstack(void)
{
    return 0;
}

/** @} */
