/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_nucleo STM32 Nucleo Common
 * @ingroup     boards_common
 * @brief       Common support for all STM32 Nucleo boards
 * @{
 *
 * @file
 * @brief       Global common Nucleo board configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_NUCLEO_H
#define BOARD_NUCLEO_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#if (defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32L0)) && \
    !defined(CPU_MODEL_STM32F042K6) && !defined(CPU_MODEL_STM32F031K6)
#define XTIMER_WIDTH                (16)
#endif

#if defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32C0)
#define XTIMER_WIDTH                (16)
#endif

#if defined(CPU_FAM_STM32F1)
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (19)
#endif

#if defined(CPU_FAM_STM32L1)
#define XTIMER_BACKOFF              (11)
#endif

#if defined(CPU_FAM_STM32F4) || defined(CPU_MODEL_STM32F303ZE)
#define XTIMER_BACKOFF              (8)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_NUCLEO_H */
