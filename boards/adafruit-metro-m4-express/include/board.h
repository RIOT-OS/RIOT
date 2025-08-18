/*
 * SPDX-FileCopyrightText: 2024 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_adafruit-metro-m4-express
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Adafruit Metro M4 Express
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#include "arduino_iomap.h"
#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   LED pin definitions and handlers
 * @{
 */

#define LED0_PIN    GPIO_PIN(PA, 16)    /**< User LED red (D13) */
#define LED1_PIN    GPIO_PIN(PA, 27)    /**< TX LED yellow */
#define LED2_PIN    GPIO_PIN(PB, 6)     /**< RX LED yellow */

#define LED0_PORT   PORT->Group[PA]     /**< Port of User LED red (D13) */
#define LED0_MASK   (1U << 16)          /**< Mask of User LED red (D13) */
#define LED1_PORT   PORT->Group[PA]     /**< Port of TX LED yellow */
#define LED1_MASK   (1U << 27)          /**< Mask of TX LED yellow */
#define LED2_PORT   PORT->Group[PB]     /**< Port of RX LED yellow */
#define LED2_MASK   (1U << 6)           /**< Mask of RX LED yellow */

#define LED0_ON     (LED0_PORT.OUTSET.reg = LED0_MASK)  /**< Switch on User LED red (D13) */
#define LED0_OFF    (LED0_PORT.OUTCLR.reg = LED0_MASK)  /**< Switch off User LED red (D13) */
#define LED0_TOGGLE (LED0_PORT.OUTTGL.reg = LED0_MASK)  /**< Toggle User LED red (D13) */

#define LED1_ON     (LED1_PORT.OUTCLR.reg = LED1_MASK)  /**< Switch on TX LED yellow */
#define LED1_OFF    (LED1_PORT.OUTSET.reg = LED1_MASK)  /**< Switch off TX LED yellow */
#define LED1_TOGGLE (LED1_PORT.OUTTGL.reg = LED1_MASK)  /**< Toggle TX LED yellow */

#define LED2_ON     (LED2_PORT.OUTCLR.reg = LED2_MASK)  /**< Switch on RX LED yellow */
#define LED2_OFF    (LED2_PORT.OUTSET.reg = LED2_MASK)  /**< Switch off RX LED yellow */
#define LED2_TOGGLE (LED2_PORT.OUTTGL.reg = LED2_MASK)  /**< Toggle User RX LED yellow */

#ifndef WS281X_PARAM_PIN
#define WS281X_PARAM_PIN    GPIO_PIN(PB, 22)    /**< GPIO pin connected to the data pin */
#endif
#ifndef WS281X_PARAM_NUMOF
#define WS281X_PARAM_NUMOF  (1U)                /**< Number of LEDs chained */
#endif
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)    /**< MTD device for the 8 MiB QSPI Flash */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
