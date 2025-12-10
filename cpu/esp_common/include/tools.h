/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of some tools for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <stdint.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Hexdump of memory
 * @param[in]  addr      start address in memory
 * @param[in]  num       number of items to dump
 * @param[in]  width     format of the items
 * @param[in]  per_line  number of items per line
 */
void esp_hexdump (const void* addr, uint32_t num, char width, uint8_t per_line);

#ifdef __cplusplus
}
#endif
