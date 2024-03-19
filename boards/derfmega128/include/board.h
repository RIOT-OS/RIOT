/*
 * Copyright (C) 2019 Alexander Chudov <chudov@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_deRFmega128
 * @{
 *
 * @file
 * @brief       Board specific definitions for the deRFmega128 modules
 *
 * @author      Alexander Chudov <chudov@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (CLOCK_CORECLOCK / 64)
#define XTIMER_BACKOFF      (40)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
