/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 * @file
 * @brief           CPU configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "RP2350.h"
#include "core_cm33.h"
#include "cpu_conf_common.h"

#define CPU_DEFAULT_IRQ_PRIO 1u
#define CPU_IRQ_NUMOF 52u

#ifdef __cplusplus
extern "C" {
#endif

/* Table 37 FIFO_ST, 1 if not empty*/
#define SIO_FIFO_READ_VALID_BIT 0
/* TABLE 37, 1 if not full */
#define SIO_FIFO_SEND_READY_BIT 1
#define core1_psm_bit 24

/**
 * The stack used by core 1, 16 times the thread stack size
 */
static volatile uint32_t core_1_stack[16*THREAD_STACKSIZE_DEFAULT];

/**
 * The function signature used for any function passed onto
 * core 1
 */
typedef void *(*core_1_fn_t)(void *arg);

/**
 * @brief Init Core 1
 * @param function The function to be loaded onto Core 1
 * @param arg The argument to pass onto the the function (NULL if none)
 */
void core1_init(core_1_fn_t function, void *arg);

/**
 * @brief This is the internal trampoline setting everything up
 *  before the entry function gets executed, it retrieves the required
 *  data from the stack and then jumps to the designated function
 */
void _core1_trampoline(void);

#ifdef __cplusplus
}
#endif

/** @} */
