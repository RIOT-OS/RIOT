/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_qn908x
 * @{
 *
 * @file
 * @brief           Wrapper header for SDK drivers.
 *
 * Vendor SDK drivers include "fsl_common.h" with many definitions and extra
 * dependencies on more headers that are not used in the RIOT-OS port. This
 * header is a wrapper intended to keep the SDK headers unchanged and provide
 * the essential definitions needed by other SDK modules.
 *
 * @author          iosabi <iosabi@protonmail.com>
 */

#ifndef FSL_COMMON_H
#define FSL_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "irq.h"
#include "vectors_cortexm.h"

#include "QN908XC.h"
#include "QN908XC_features.h"

#ifdef __cplusplus
extern "C" {
#endif

/* RIOT-OS equivalent functions. */
#define DisableGlobalIRQ irq_disable
#define EnableGlobalIRQ irq_restore

#define __Vectors cortex_vector_base

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FSL_COMMON_H */
