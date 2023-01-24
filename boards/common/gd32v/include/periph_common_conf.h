/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *               2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_gd32v
 * @{
 *
 * @file
 * @brief       Common peripheral configuration for GD32VF103 boards
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_COMMON_CONF_H
#define PERIPH_COMMON_CONF_H

#include "macros/units.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_CORECLOCK         MHZ(108)    /**< CPU clock frequency in Hz */

#define CLOCK_AHB               CLOCK_CORECLOCK /**< Equal to the CPU clock */
#define CLOCK_APB1              CLOCK_AHB/2     /**< Half AHB clock */
#define CLOCK_APB2              CLOCK_AHB       /**< Equal to the AHB clock */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_COMMON_CONF_H */
/** @} */
