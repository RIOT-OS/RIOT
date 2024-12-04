/*
 * Copyright (C) 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_fe310
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author          Ken Rabold
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

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
#endif /* CPU_CONF_H */
