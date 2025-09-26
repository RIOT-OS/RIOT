/*
 * Copyright (C) 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_DEFAULT_IRQ_PRIO 1u
#define CPU_IRQ_NUMOF 52u

#ifdef __cplusplus
}
#endif

/** @} */
