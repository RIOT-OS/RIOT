/*
 * SPDX-FileCopyrightText: 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Utilities for tests/lwip/
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Converts hex string to byte array.
 *
 * @param[out] out  Resulting byte array
 * @param[in] in    `\0` terminated string. Non-hex characters (all except 0-9, a-f, A-F)
 *                  will be ignored.
 *
 * @return  Length of @p out.
 */
size_t hex2ints(uint8_t *out, const char *in);

#ifdef __cplusplus
}
#endif

/** @} */
