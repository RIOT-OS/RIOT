/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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

/**
 * @brief   Default heap size for MicroPython on RIOT
 *
 * Use a reasonable default heap size.
 */
#ifndef MP_RIOT_HEAPSIZE
  #define MP_RIOT_HEAPSIZE      (16U*1024)
#endif

/**
 * @brief   Safe area to leave free on the stack for interrupts and other
 *          unexpected stack usage
 */
#ifndef MP_STACK_SAFEAREA
  #define MP_STACK_SAFEAREA     (128U)
#endif

/**
 * @brief   Initialize RIOT MicroPython port
 *
 * @param[in]   heap        ptr to heap MicroPython should use
 * @param[in]   heap_size   size of heap
 */
void mp_riot_init(char* heap, size_t heap_size);

/** @} */
