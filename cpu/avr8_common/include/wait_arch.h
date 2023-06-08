/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_avr8_common
 * @{
 *
 * @file
 * @brief       AVR specific busy wait
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 */

#ifndef WAIT_ARCH_H
#define WAIT_ARCH_H

#include <stdint.h>

#include "board.h"
#include "periph_conf.h"
#include "time_units.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef DOXYGEN

#define HAS_WAIT_US_SPINNING

static inline void wait_us_spinning(uint32_t usecs)
{
    uint32_t cycles = usecs * (uint64_t)CLOCK_CORECLOCK / US_PER_SEC;

    __builtin_avr_delay_cycles(cycles);
}

#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* WAIT_ARCH_H */
