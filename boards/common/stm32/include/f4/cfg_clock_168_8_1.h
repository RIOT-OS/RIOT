/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Configure STM32F4 clock to 168MHz using PLL with LSE
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef F4_CFG_CLOCK_168_8_1_H
#define F4_CFG_CLOCK_168_8_1_H

#include "f4/cfg_clock_168_8_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LSE clock settings
 *
 * 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz)
 */
#define CLOCK_LSE           (1)

#ifdef __cplusplus
}
#endif

#endif /* F4_CFG_CLOCK_168_8_1_H */
/** @} */
