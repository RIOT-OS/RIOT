/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp32_heltec-lora32-v2
 * @brief       Board specific definitions for Heltec WiFi LoRa 32 V2 board
 * @{
 *
 * Heltec WiFi LoRa 32 V2 is an ESP32 development board with 8 MB Flash that
 * uses the EPS32 chip directly. It integrates a SemTech SX1276 or SX1278 for
 * LoRaWAN communication in the 433 MHz or the 868/915 MHz band, respectively.
 * Additionally, it has an OLED display connected via I2C on board.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_application_specific_configurations
 * "application-specific board configuration".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

/**
 * @name    Button pin definitions
 * @{
 */

/**
 * @brief   Default button GPIO pin definition
 *
 * Generic ESP32 boards have a BOOT button connected to GPIO0, which can be
 * used as button during normal operation. Since the GPIO0 pin is pulled up,
 * the button signal is inverted, i.e., pressing the button will give a
 * low signal.
 */
#define BTN0_PIN        GPIO0

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.
 */
#define BTN0_MODE       GPIO_IN

/**
 * @brief   Default interrupt flank definition for the button GPIO
 */
#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK  GPIO_FALLING
#endif

/**
 * @brief   Definition for compatibility with previous versions
 */
#define BUTTON0_PIN     BTN0_PIN

/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * @{
 */
#define LED0_PIN        GPIO25
#define LED0_ACTIVE     (1)     /**< LED is high active */
/** @} */

/**
 * @name        SX127X
 *
 * SX127X configuration.
 * @{
 */
#define SX127X_PARAM_SPI                (SPI_DEV(0))
#define SX127X_PARAM_SPI_NSS            GPIO18
#define SX127X_PARAM_RESET              GPIO14
#define SX127X_PARAM_DIO0               GPIO26
#define SX127X_PARAM_DIO1               GPIO_UNDEF /* GPIO35 has no pulldown, leads to init error */
#define SX127X_PARAM_DIO2               GPIO_UNDEF /* GPIO34 has no pulldown, leads to init error */
#define SX127X_PARAM_DIO3               GPIO_UNDEF
#define SX127X_PARAM_DIO_MULTI          GPIO_UNDEF
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
