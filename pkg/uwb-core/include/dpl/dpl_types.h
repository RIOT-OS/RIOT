/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <stdint.h>
#include <math.h>

#include "os/os_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    PI approximated value macro definition
 */
#ifndef M_PI
#define M_PI 3.1415926535
#endif

/**
 * @name    Macro to wait forever on events and mutexes
 * @{
 */
#define DPL_TIMEOUT_NEVER   (OS_TIMEOUT_NEVER)
#define DPL_WAIT_FOREVER    (OS_WAIT_FOREVER)
/** @} */

/**
 * @name    Decawave porting layer (DPL) stack alignment requirement
 * @{
 */
#define DPL_STACK_ALIGNMENT (OS_ALIGNMENT)
/** @} */

/**
 * @brief dpl time type
 */
typedef os_time_t dpl_time_t;

/**
 * @brief dpl stack buffer type
 */
typedef os_stack_t dpl_stack_t;

/**
 * @brief dpl float 32 type
 */
typedef float dpl_float32_t;
/**
 * @brief dpl float 64 type
 */
typedef double dpl_float64_t;

/**
 * @name    Decawave porting layer (DPL) float type macros
 * @{
 */
#define DPL_FLOAT32_INIT(__X) ((float)__X)
#define DPL_FLOAT64_INIT(__X) ((double)__X)
#define DPL_FLOAT64TO32(__X) (float)(__X)
#define DPL_FLOAT32_I32_TO_F32(__X) (float)(__X)
#define DPL_FLOAT64_I32_TO_F64(__X) ((double)(__X))
#define DPL_FLOAT64_I64_TO_F64(__X) ((double)(__X))
#define DPL_FLOAT64_U64_TO_F64(__X) ((double)(__X))
#define DPL_FLOAT64_F64_TO_U64(__X) ((uint64_t)(__X))
#define DPL_FLOAT32_INT(__X) ((int32_t)__X)
#define DPL_FLOAT64_INT(__X) ((int64_t)__X)
#define DPL_FLOAT64_FROM_F32(__X) (double)(__X)
#define DPL_FLOAT32_FROM_F64(__X) (float)(__X)
#define DPL_FLOAT32_CEIL(__X) (ceilf(__X))
#define DPL_FLOAT64_CEIL(__X) (ceil(__X))
#define DPL_FLOAT32_FABS(__X) fabsf(__X)
#define DPL_FLOAT32_FMOD(__X, __Y) fmodf(__X, __Y)
#define DPL_FLOAT64_FMOD(__X, __Y) fmod(__X, __Y)
#define DPL_FLOAT32_NAN() nanf("")
#define DPL_FLOAT64_NAN() nan("")
#define DPL_FLOAT32_ISNAN(__X) isnan(__X)
#define DPL_FLOAT64_ISNAN(__X) DPL_FLOAT32_ISNAN(__X)
#define DPL_FLOAT32_LOG10(__X) (log10f(__X))
#define DPL_FLOAT64_LOG10(__X) (log10(__X))
#define DPL_FLOAT64_ASIN(__X) asin(__X)
#define DPL_FLOAT64_ATAN(__X) atan(__X)
#define DPL_FLOAT32_SUB(__X, __Y) ((__X)-(__Y))
#define DPL_FLOAT64_SUB(__X, __Y) ((__X)-(__Y))
#define DPL_FLOAT32_ADD(__X, __Y) ((__X)+(__Y))
#define DPL_FLOAT64_ADD(__X, __Y) ((__X)+(__Y))
#define DPL_FLOAT32_MUL(__X, __Y) ((__X)*(__Y))
#define DPL_FLOAT64_MUL(__X, __Y) ((__X)*(__Y))
#define DPL_FLOAT32_DIV(__X, __Y) ((__X)/(__Y))
#define DPL_FLOAT64_DIV(__X, __Y) ((__X)/(__Y))
#define DPL_FLOAT32_PRINTF_PRIM "%s%d.%03d"
#define DPL_FLOAT32_PRINTF_VALS(__X) (__X)<0?"-":"", (int)(fabsf(__X)), (int)(fabsf((__X)-(int)(__X))*1000)
#define DPL_FLOAT64_PRINTF_PRIM "%s%d.%06d"
#define DPL_FLOAT64_PRINTF_VALS(__X) (__X)<0?"-":"", (int)(fabs(__X)), (int)(fabs((__X)-(int)(__X))*1000000)
/** @} */

#ifdef __cplusplus
}
#endif
