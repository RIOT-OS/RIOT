/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

#pragma once

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  Command-line EUI provider for native
 * @author Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    parse a string as an EUI-64 and add it to the list of EUI-64s
 *
 * @param[in] s     EUI-64 as hexadecimal string representation
 */
void native_cli_add_eui64(const char *s);

/**
 * @brief    Get a command-line provided EUI-64
 *
 * @param index     index of ZEP device
 * @param[out] addr user supplied EUI-64
 *
 * @return 0 on success, negatvie if no more EUIs are available.
 */
int native_cli_get_eui64(uint8_t index, eui64_t *addr);

#ifdef __cplusplus
}
#endif

/** @} */
