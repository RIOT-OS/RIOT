/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_wemos_d1_r32
 * @brief       Board specific definitions for Wemos D1 R32 (ESPDuino-32) board
 * @{
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

/**
 * @name    LED (on-board) configuration
 * @{
 */
#define LED0_PIN        GPIO2   /**< LED is connected to GPIO2 */
#define LED0_ACTIVE     1       /**< LED is HIGH active */
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
