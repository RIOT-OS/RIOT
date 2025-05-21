/*
 * Copyright (C) 2017 JP Bonn
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           Functions to read CPU cycle counter
 *
 * @author          JP Bonn
 */


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Returns a count of the number of clock cycles executed by the
 *          processor core on which the hart is running from an arbitrary
 *          start time in the past.
 */
uint64_t get_cycle_count(void);

#ifdef __cplusplus
}
#endif

/** @} */
