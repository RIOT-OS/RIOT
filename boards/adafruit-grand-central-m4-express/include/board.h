/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-grand-central-m4-express
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Adafruit Grand Central M4 Express
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   LED pin definitions and handlers
 * @{
 */

#define LED0_PIN    GPIO_PIN(PB, 1)     /**< User LED red (D13) */
#define LED1_PIN    GPIO_PIN(PC, 30)    /**< TX LED yellow */
#define LED2_PIN    GPIO_PIN(PC, 31)    /**< RX LED yellow */

#define LED0_PORT   PORT->Group[PB]     /**< Port of User LED red (D13) */
#define LED0_MASK   (1 << 1)            /**< Mask of User LED red (D13) */
#define LED1_PORT   PORT->Group[PC]     /**< Port of TX LED yellow */
#define LED1_MASK   (1 << 30)           /**< Mask of TX LED yellow */
#define LED2_PORT   PORT->Group[PC]     /**< Port of RX LED yellow */
#define LED2_MASK   (1 << 31)           /**< Mask of RX LED yellow */

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
#define WS281X_PARAM_PIN    GPIO_PIN(PC, 24)    /**< GPIO pin connected to the data pin */
#endif
#ifndef WS281X_PARAM_NUMOF
#define WS281X_PARAM_NUMOF  (1U)                /**< Number of LEDs chained */
#endif
/** @} */

/**
 * @name MTD configuration
 * @{
 */
#define MTD_0 mtd_dev_get(0)    /**< MTD device for the 8 MByte QSPI Flash */
#define MTD_1 mtd_dev_get(1)    /**< MTD device for the SD Card */

#define CONFIG_SDCARD_GENERIC_MTD_OFFSET    1   /**< MTD_1 is used for SD Card */
/** @} */

/**
 * @name   SD-Card interface configuration
 *
 * The SD-Card interface is connected to SPI_DEV(1).
 *
 * @{
 */
#if MODULE_SDCARD_SPI || DOXYGEN
#define SDCARD_SPI_PARAM_SPI    SPI_DEV(1)
#define SDCARD_SPI_PARAM_CLK    GPIO_PIN(PB, 27)
#define SDCARD_SPI_PARAM_MOSI   GPIO_PIN(PB, 26)
#define SDCARD_SPI_PARAM_MISO   GPIO_PIN(PB, 29)
#define SDCARD_SPI_PARAM_CS     GPIO_PIN(PB, 28)
#define SDCARD_SPI_PARAM_POWER  GPIO_UNDEF
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
