/*
 * Copyright (C) 2020 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pwm_dac
 *
 * @{
 * @file
 * @brief       Definitions for boards that use PWM DAC
 *
 * This file is included by board that utilize PWM DAC, and serves two
 * purposes:
 *
 * * Provide a DAC_NUMOF -- it's not provided in <periph/dac.h> as that would
 *   create circular dependencies, but providing one in board.h is good enough
 *   given that it's really a board property.
 *
 * * Define HAVE_DAC_T -- not that it'd be defined so much differently, more to
 *   ensure that if this were to be used on a device that has actual DACs, the
 *   conflict shows early (at the preprocessor stage) and not at the linker
 *   stage where it's harder to debug.
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#ifndef PWM_DAC_FOR_BOARD_H
#define PWM_DAC_FOR_BOARD_H

#include <stddef.h>

extern const size_t pwm_dac_numof;
#define DAC_NUMOF pwm_dac_numof

#define HAVE_DAC_T
typedef size_t dac_t;

#endif /* PWM_DAC_FOR_BOARD_H */
