/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf51822
 * @{
 *
 * @file
 * @brief           CPU specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef CPU_PERIPH_H_
#define CPU_PERIPH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define mandatory GPIO types for NRF51822 CPUs
 * @{
 */
typedef int gpio_t;
typedef int gpio_mux_t;
/** @} */

/**
 * @brief   Mandatory macro for defining GPIO pins
 *
 * The port definition is used (and zeroed) to suppress compiler warnings
 */
#define GPIO(x,y)           ((x & 0) | y)

#ifdef __cplusplus
}
#endif

#endif /* CPU_PERIPH_H_ */
/** @} */
