/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Utilities for tests/lwip/
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef UTILITIES_H
#define UTILITIES_H

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

#endif /* UTILITIES_H */
/** @} */
