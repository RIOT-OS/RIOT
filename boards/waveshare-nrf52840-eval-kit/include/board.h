/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waveshare_nrf52840_eval_kit
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Waveshare nRF52840 Eval Kit
 *
 * @author     Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 13)
#define LED1_PIN            GPIO_PIN(0, 14)
#define LED2_PIN            GPIO_PIN(1, 9)
#define LED3_PIN            GPIO_PIN(0, 16)

#define LED0_PORT           (NRF_P0)
#define LED1_PORT           (NRF_P0)
#define LED2_PORT           (NRF_P1)
#define LED3_PORT           (NRF_P0)

#define LED0_MASK           (1 << 13)
#define LED1_MASK           (1 << 14)
#define LED2_MASK           (1 << 9)
#define LED3_MASK           (1 << 16)

#define LED0_ON             (LED0_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED1_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED1_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED1_PORT->OUT   ^= LED1_MASK)

#define LED2_ON             (LED2_PORT->OUTCLR = LED2_MASK)
#define LED2_OFF            (LED2_PORT->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED2_PORT->OUT   ^= LED2_MASK)

#define LED3_ON             (LED3_PORT->OUTCLR = LED3_MASK)
#define LED3_OFF            (LED3_PORT->OUTSET = LED3_MASK)
#define LED3_TOGGLE         (LED3_PORT->OUT   ^= LED3_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 11)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(0, 18)
#define BTN1_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    SD Card configuration
 * @{
 */
#define SDCARD_SPI_PARAM_SPI         SPI_DEV(1)
#define SDCARD_SPI_PARAM_CS          GPIO_PIN(0, 12)
#define SDCARD_SPI_PARAM_CLK         GPIO_PIN(0, 17)
#define SDCARD_SPI_PARAM_MOSI        GPIO_PIN(0, 24)
#define SDCARD_SPI_PARAM_MISO        GPIO_PIN(0, 20)
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)          /**< MTD device for the SD Card */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
