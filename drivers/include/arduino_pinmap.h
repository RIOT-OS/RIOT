/*
 * SPDX-FileCopyrightText: 2023 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       Compatibility wrapper for `arduino_iomap.h`.
 *
 * Please update your code to include `arduino_iomap.h` instead.
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include "arduino_iomap.h"

#ifdef __cplusplus
extern "C" {
#endif

/* just a wrapper for arduino_iomap.h for backward compatibility, no code
 * here ... */

#ifdef __cplusplus
}
#endif

/** @} */
