/*
 * SPDX-FileCopyrightText: 2022 Dario Petrillo
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ir_nec
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Dario Petrillo <dario.pk1@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#define IR_NEC_START_US 4500 /**< Width of the start pulse, in microseconds */
#define IR_NEC_ZERO_US 562   /**< Width of the zero pulse, in microseconds */
#define IR_NEC_ONE_US 1687   /**< Width of the one pulse, in microseconds */
#define IR_NEC_EPS_US 350    /**< Maximum timing deviation for value detection, in microseconds */

#ifdef __cplusplus
}
#endif

/** @} */
