/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx126x
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SX126X_PARAMS_H
#define SX126X_PARAMS_H

#include "board.h"
#include "sx126x.h"
#include "sx126x_driver.h"

#ifdef MODULE_SHIELD_LC68
#  include "arduino_iomap.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_SHIELD_LLCC68
#  define SX126X_PARAM_SPI                  ARDUINO_SPI_D11D12D13
#  define SX126X_PARAM_SPI_NSS              ARDUINO_PIN_7
#  define SX126X_PARAM_RESET                ARDUINO_PIN_A0
#  define SX126X_PARAM_BUSY                 ARDUINO_PIN_3
#  define SX126X_PARAM_DIO1                 ARDUINO_PIN_5
#endif

/**
 * @name    Set default configuration parameters
 *
 * Default values are adapted for mbed shield used with to nucleo64 boards
 * @{
 */
#if !defined(SX126X_PARAM_SPI) || defined(DOXYGEN)
/**
 * @brief   SPI bus
 */
#  define SX126X_PARAM_SPI                  SPI_DEV(0)
#endif

#if !defined(SX126X_PARAM_SPI_NSS) || defined(DOXYGEN)
/**
 * @brief   SPI chip select pin
 */
#  define SX126X_PARAM_SPI_NSS              GPIO_PIN(0, 8)  /* D7 */
#endif

#if !defined(SX126X_PARAM_RESET) || defined(DOXYGEN)
/**
 * @brief   Reset pin
 */
#  define SX126X_PARAM_RESET                GPIO_PIN(0, 0)  /* A0 */
#endif

#if !defined(SX126X_PARAM_BUSY) || defined(DOXYGEN)
/**
 * @brief   Busy pin
 */
#  define SX126X_PARAM_BUSY                 GPIO_PIN(1, 3)  /* D3 */
#endif

#if !defined(SX126X_PARAM_DIO1) || defined(DOXYGEN)
/**
 * @brief   DIO1 interrupt pin
 */
#  define SX126X_PARAM_DIO1                 GPIO_PIN(1, 4)  /* D5 */
#endif

#if !defined(SX126X_PARAM_REGULATOR) || defined(DOXYGEN)
/**
 * @brief   Regulator type which can be
 *          SX126X_REG_MODE_LDO or SX126X_REG_MODE_DCDC
 */
#  define SX126X_PARAM_REGULATOR            SX126X_REG_MODE_DCDC
#endif

#if !defined(SX126X_PARAM_SET_RF_MODE_CB) || defined(DOXYGEN)
/**
 * @brief   Callback function to call when switching between RX and TX states
 */
#  define SX126X_PARAM_SET_RF_MODE_CB       NULL
#endif

#if !defined(SX126X_PARAM_TX_PA_MODE) || defined(DOXYGEN)
/**
 * @brief   Transmit power amplifier mode which can be
 *          SX126X_RF_MODE_TX_HPA or SX126X_RF_MODE_TX_LPA
 */
#  define SX126X_PARAM_TX_PA_MODE           SX126X_RF_MODE_TX_LPA
#endif

#ifndef SX126X_PARAM_TYPE
#  if IS_USED(MODULE_SX1261)
#    define SX126X_PARAM_TYPE SX126X_TYPE_SX1261
#  elif IS_USED(MODULE_SX1262)
#    define SX126X_PARAM_TYPE SX126X_TYPE_SX1262
#  elif IS_USED(MODULE_SX1268)
#    define SX126X_PARAM_TYPE SX126X_TYPE_SX1268
#  elif IS_USED(MODULE_LLCC68)
#    define SX126X_PARAM_TYPE SX126X_TYPE_LLCC68
#  elif IS_USED(MODULE_SX126X_STM32WL)
#    define SX126X_PARAM_TYPE SX126X_TYPE_STM32WL
#  else
#    error "You should select at least one of the SX126x variants."
#  endif
#endif

#if IS_USED(MODULE_SX126X_RF_SWITCH)
#  define SX126X_SET_RF_MODE  .set_rf_mode = SX126X_PARAM_SET_RF_MODE_CB,
#  define SX126X_TX_PA_MODE   .tx_pa_mode = SX126X_PARAM_TX_PA_MODE
#else
#  define SX126X_SET_RF_MODE
#  define SX126X_TX_PA_MODE
#endif

#ifndef SX126X_PARAMS
#  define SX126X_PARAMS           { .spi = SX126X_PARAM_SPI,            \
                                    .nss_pin = SX126X_PARAM_SPI_NSS,    \
                                    .reset_pin = SX126X_PARAM_RESET,    \
                                    .busy_pin = SX126X_PARAM_BUSY,      \
                                    .dio1_pin = SX126X_PARAM_DIO1,      \
                                    .type     = SX126X_PARAM_TYPE,      \
                                    .regulator = SX126X_PARAM_REGULATOR, \
                                    SX126X_SET_RF_MODE \
                                    SX126X_TX_PA_MODE}
#endif

/**@}*/

/**
 * @brief   Configuration struct
 */
static const sx126x_params_t sx126x_params[] =
{
    SX126X_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* SX126X_PARAMS_H */
/** @} */
