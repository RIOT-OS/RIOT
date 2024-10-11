/*
 * Copyright (C)  2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef ARDUINO_PINMAP_H
#define ARDUINO_PINMAP_H

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
#endif /* ARDUINO_PINMAP_H */
