/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2020 Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_esp32_esp-ethernet-kit
 * @file
 * @{
 */

#include <stdint.h>

/* include common board definitions as last step */
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !MODULE_ESP_ETH || DOXYGEN
/**
 * @name    Button pin definitions
 * @{
 */
/**
 * @brief   Default button GPIO pin definition
 *
 * The button is only available when Ethernet is not used, as is shares its pin
 * with the Phy clock.
 */
#define BTN0_PIN        GPIO0

/**
 * @brief   Default button GPIO mode definition
 *
 * Since the GPIO of the button is pulled up with an external resistor, the
 * mode for the GPIO pin has to be GPIO_IN.  */
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
#endif /* !MODULE_ESP_ETH || DOXYGEN */

/**
 * @name    ESP32 Ethernet (EMAC) configuration
 * @{
 */
#define EMAC_PHY_IP101G         1                   /**< IP101G used as PHY interface */
#define EMAC_PHY_ADDRESS        1                   /**< PHY1 used as base address */
#define EMAC_PHY_SMI_MDC_PIN    23                  /**< SMI MDC pin */
#define EMAC_PHY_SMI_MDIO_PIN   18                  /**< SMI MDC pin */
#define EMAC_PHY_CLOCK_MODE     ETH_CLOCK_GPIO0_IN  /**< external 50 MHz clock */
#define EMAC_PHY_POWER_PIN      GPIO5               /**< PHY RESET_N connected to pin 5 */
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
