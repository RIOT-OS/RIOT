/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf52
 * @{
 *
 * @file
 * @brief           nRF52 specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_
#define PERIPH_CPU_

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Redefine some peripheral names to unify them between nRF51 and 52
 * @{
 */
#define GPIO_BASE           (NRF_P0)
#define UART_IRQN           (UARTE0_UART0_IRQn)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_ */
/** @} */
