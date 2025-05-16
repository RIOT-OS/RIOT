/*
 * Copyright (C) 2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_malloc_monitor_internals Heap Memory Usage Monitor internals
 * @ingroup     sys_malloc_monitor
 * @{
 *
 * @brief       internals for monitoring heap memory usage (calls to malloc/calloc/realloc/free)
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */


#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "architecture.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Record malloc/calloc/realloc call increasing heap usage.
 *
 * @param[in]   ptr         pointer to newly allocated memory
 * @param[in]   size        size of newly allocated memory
 * @param[in]   pc          PC of calling function
 * @param[in]   func_prefix prefix identifying memory function, one of "m","c","re"
 *
 * @internal
 */
void malloc_monitor_add(void *ptr, size_t size, uinttxtptr_t pc, char *func_prefix);

/**
 * @brief Record free/realloc call decreasing heap usage.
 *
 * @param[in]   ptr    pointer to memory that is being freed
 * @param[in]   pc     PC of calling function
 *
 * @internal
 */
void malloc_monitor_rm(void *ptr, uinttxtptr_t pc);

/**
 * @brief Record realloc call either increasing or decreasing heap usage.
 *
 * @param[in]   ptr_old     pointer to previously allocated memory
 * @param[in]   ptr_new     pointer to newly allocated memory
 * @param[in]   size_new    size of newly allocated memory
 * @param[in]   pc          PC of calling function
 *
 * @internal
 */
void malloc_monitor_mv(void *ptr_old, void *ptr_new, size_t size_new, uinttxtptr_t pc);

#ifdef __cplusplus
}
#endif


/**
 * @}
 */
