/*
 * Copyright (C) 2016 Inria
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo144-f413
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the nucleo144-f413 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "periph_clk_f4_100.h"
#include "periph_timer_25.h"
#include "periph_uart_144_362.h"
#include "periph_pwm_144_14.h"
#include "periph_spi_144_1.h"
#include "periph_i2c_144_1.h"
#include "periph_adc_144_accccc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    RTC configuration
 * @{
 */
#define RTC_NUMOF           (1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
