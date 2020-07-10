/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2020 Google LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_esp-ethernet-kit-v1_0
 * @file
 * @{
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/* include common board definitions as last step */
#include "board_common.h"

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

#endif /* BOARD_H */
/** @} */
