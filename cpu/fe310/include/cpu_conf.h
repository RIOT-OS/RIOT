/*
 * SPDX-FileCopyrightText: 2017 Ken Rabold
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_fe310
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author          Ken Rabold
 */

#include "cpu_conf_common.h"
#include "vendor/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Base address of the CLINT
 */
#define CLINT_BASE_ADDR     (CLINT_CTRL_ADDR)

/**
 * @brief Base address of the PLIC peripheral
 */
#define PLIC_BASE_ADDR      (PLIC_CTRL_ADDR)

/**
 * @brief Number of available PMP regions
 * Note, the upper 8 regions are hardwired to zero!
 */
#define NUM_PMP_ENTRIES 16

#ifdef __cplusplus
}
#endif

/** @} */
