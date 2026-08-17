/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_noelv
 * @{
 *
 * @file
 * @brief       CPU specific configuration for the Gaisler NOEL-V
 * @author      Matvii Ivashchenko
 */

#include "cpu_conf_common.h"
#include "vendor/clint.h"
#include "vendor/plic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Base address of the RISC-V ACLINT (CLINT)
 *
 */
#define CLINT_BASE_ADDR         (0xe0000000UL)

/**
 * @brief   Base address of the PLIC
 *
 */
#define PLIC_CTRL_ADDR          (0xf8000000UL)

/**
 * @brief   Number of interrupt sources in the PLIC
 *
 */
#define PLIC_NUM_INTERRUPTS     (31U)

/**
 * @brief   Number of priority levels in the PLIC
 *
 */
#define PLIC_NUM_PRIORITIES     (7U)

/**
 * @brief   Number of available PMP regions
 */
#define NUM_PMP_ENTRIES         (16)

/**
 * @brief   MTIME clock frequency of the ACLINT
 *
 * GRLIB ACLINT runs mtime at CLOCK_CORECLOCK / 2.
 * Must match CONFIG_ZTIMER_USEC_BASE_FREQ in board.h.
 */
#define RTC_FREQ                (20000000UL)

#ifdef __cplusplus
}
#endif

/** @} */
