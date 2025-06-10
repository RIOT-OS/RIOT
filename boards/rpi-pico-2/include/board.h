/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         boards_rpi_pico_2
 * @{
 *
 * @file
 * @brief           Board specific definitions for the Raspberry Pi Pico 2
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "RP2350.h"
#include "cpu.h"
#include "cpu_conf.h"
#include "periph_conf.h"
#include "periph_cpu.h"

/** GPIO Pin ID for the onboard LED */
#define LED0_PIN_ID 25u
#define LED0_ON         gpio_set(LED0_PIN_ID)
#define LED0_OFF        gpio_clear(LED0_PIN_ID)
#define LED0_TOGGLE     gpio_toggle(LED0_PIN_ID)
#define LED0_NAME       "LED(Green)"

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize the board, called from the cpu startup code */
void board_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
