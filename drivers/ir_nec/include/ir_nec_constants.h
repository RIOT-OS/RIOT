/*
 * Copyright (C) 2022 Dario Petrillo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ir_nec
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Dario Petrillo <dario.pk1@gmail.com>
 */

#ifndef IR_NEC_CONSTANTS_H
#define IR_NEC_CONSTANTS_H

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

#endif /* IR_NEC_CONSTANTS_H */
/** @} */
