/*
 * SPDX-FileCopyrightText: 2021 ML!PA Consulting GmbH
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_tiny_strerror   Tiny strerror() implementation
 * @ingroup     sys
 * @brief       Tiny strerror() implementation
 *
 * Usage
 * =====
 *
 * Select module `tiny_strerror` in order to use it.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * #include "tiny_strerror.h"
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @note        Using module `tiny_strerror_as_strerror` will replace all calls
 *              to `strerror()` by calls to `tiny_strerror()`, which may safe
 *              a bit of ROM.
 *
 * @note        Using module `tiny_strerror_minimal` will just print the error
 *              code value.
 *              This will save ~1k of ROM, but won't provide much more information.
 *
 * @warning     The module `tiny_strerror_minimal` is not thread-safe.
 *
 * @{
 *
 *
 * @file
 * @brief       Tiny strerror() implementation
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <errno.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get the name of an errno macro
 *
 * @param[in]   errnum  Error code to get the name of
 *
 * Example:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *     puts(tiny_strerror(ENOENT)); // prints "ENOENT"
 *     puts(tiny_strerror(-ENXIO)); // prints "-ENXIO"
 *     puts(tiny_strerror(0)); // prints "OK"
 *     puts(tiny_strerror(1337)); // prints "unknown"
 *     puts(tiny_strerror(-1337)); // prints "-unknown"
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @return  String holding the macro name of the error given by @p errnum
 *
 * @retval  `"OK"`          If @p errnum is `0`
 * @retval  `"unknown"`     If @p errnum is not matching any POSIX errno
 *                          constant
 * @retval  `"-unknown"`    If @p errnum is negative not matching any negative
 *                          POSIX errno constant
 */
const char *tiny_strerror(int errnum);

#ifdef __cplusplus
}
#endif

/** @} */
