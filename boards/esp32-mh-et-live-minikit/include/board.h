/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_mh-et-live-minikit
 * @brief       Board specific definitions for MH-ET LIVE MiniKit for ESP32
 * @{
 *
 * The MH-ET LIVE MiniKit for ESP32 uses the ESP32-WROOM-32 module. It is
 * a very interesting development kit as it is available in the stackable
 * Wemos D1 Mini format. Thus, all shields for Wemos D1 mini (ESP8266
 * platform) can also be used with ESP32. All GPIOs are broken out so that
 * it can be configured very flexibly.
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

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/**
 * @name    LED (on-board) configuration
 *
 * @{
 */
#define LED0_PIN        GPIO2
#define LED0_ACTIVE     1       /**< LED is high active */

#define LED_BLUE_PIN    GPIO2
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

/* include definitions for optional hardware modules */
#include "board_modules.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the board specific hardware
 */
static inline void board_init(void) {
    /* there is nothing special to initialize on this board */
    board_init_common();
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
