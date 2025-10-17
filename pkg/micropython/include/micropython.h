/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *               2019 Inria
 *               2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_micropython
 *
 * @{
 *
 * @file
 * @brief       MicroPython RIOT specific API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MP_RIOT_HEAPSIZE
/* use a reasonable default heap size */
#define MP_RIOT_HEAPSIZE (16U*1024)
#endif

#ifndef MP_STACK_SAFEAREA
#define MP_STACK_SAFEAREA (128U)
#endif

/**
 * @brief   Initialize RIOT MicroPython port
 *
 * @param[in]   heap        ptr to heap MicroPython should use
 * @param[in]   heap_size   size of heap
 */
void mp_riot_init(char* heap, size_t heap_size);

/**
 * @brief   Execute a string as MicroPython code
 *
 * The string will be executed on the global MicroPython instance.
 *
 * @param[in]   src pointer to Python code
 * @param[in]   len length of src
 */
void mp_do_str(const char *src, int len);

/** @} */
