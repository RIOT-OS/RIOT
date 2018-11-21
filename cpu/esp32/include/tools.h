/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of some tools
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#ifndef TOOLS_H
#define TOOLS_H

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

#endif /* TOOLS_H */
