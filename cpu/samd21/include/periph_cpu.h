/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd21
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H
#define CPU_PERIPH_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Available ports on the SAMD21
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
    PC = 2,                 /**< port C */
};

/**
 * @brief   PWM channel configuration data structure
 */
typedef struct {
    gpio_t pin;                 /**< GPIO pin */
    gpio_mux_t mux;             /**< pin function multiplex value */
    uint8_t chan;               /**< TCC channel to use */
} pwm_conf_chan_t;

/**
 * @brief   PWM device configuration data structure
 */
typedef struct {
    Tcc *dev;                   /**< TCC device to use */
    pwm_conf_chan_t chan[3];    /**< channel configuration */
} pwm_conf_t;

/**
 * @brief   UART device configuration
 */
typedef struct {
    SercomUsart *dev;       /**< pointer to the used UART device */
    gpio_t rx_pin;          /**< pin used for RX */
    gpio_t tx_pin;          /**< pin used for TX */
    gpio_mux_t mux;         /**< alternative function for pins */
} uart_conf_t;

/**
 * @brief   Return the numeric id of a SERCOM device derived from its address
 *
 * @param[in] sercom    SERCOM device
 *
 * @return              numeric id of the given SERCOM device
 */
static inline int _sercom_id(SercomUsart *sercom)
{
    return ((((uint32_t)sercom) >> 10) & 0x7) - 2;
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H */
/** @} */
