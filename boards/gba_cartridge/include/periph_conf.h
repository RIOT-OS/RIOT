/*
 * Copyright (C) 2023 Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_gba_cartridge
 * @{
 *
 * @file
 * @brief
 *
 * @author
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK (16780000U) /* this board runs with 16.78MHz */
#define CLOCK_PCLK (CLOCK_CORECLOCK)
    /** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
