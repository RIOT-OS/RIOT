/*
 * Copyright (C) 2019 Yegor Yefremov
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_ttgo-t-beam
 * @brief       Board specific definitions for TTGO T-Beam board
 * @{
 *
 * TTGO T-Beam is an ESP32 development board with 8 MB Flash that
 * uses the EPS32 chip directly. It integrates a SemTech SX1276 or SX1278 for
 * LoRaWAN communication in the 433 MHz or the 868/915 MHz band, respectively.
 * Additionally, it has an OLED display connected via I2C on board.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_app_spec_conf
 * "application-specific board configuration".
 *
 * @file
 * @author      Yegor Yefremov <yegorslists@googlemail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    Button pin definitions
 * @{
 */
#if MODULE_ESP32_TTGO_T_BEAM_V1_0
#define BUTTON0_PIN     GPIO38
#else
#define BUTTON0_PIN     GPIO39
#endif
/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * @{
 */
#ifndef MODULE_ESP32_TTGO_T_BEAM_V1_0
#define LED0_PIN        GPIO14
#define LED0_ACTIVE     (1)     /**< LED is high active */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/**
 * @name        SX127X
 *
 * SX127X configuration.
 * @{
 */
#define SX127X_PARAM_SPI_NSS            GPIO18
#define SX127X_PARAM_RESET              GPIO23
#define SX127X_PARAM_DIO0               GPIO26
#define SX127X_PARAM_DIO1               GPIO_UNDEF /* Pin is not connected to the LoRa chip directly */
#define SX127X_PARAM_DIO2               GPIO_UNDEF /* Pin is not connected to the LoRa chip directly */
#define SX127X_PARAM_DIO3               GPIO_UNDEF
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

/**
 * @brief Initialize the board specific hardware
 */
void board_init(void);

#endif /* BOARD_H */
/** @} */
