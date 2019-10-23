/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bh1750fvi
 * @{
 *
 * @file
 * @brief       Command definition for the BH1750FVI light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BH1750FVI_INTERNAL_H
#define BH1750FVI_INTERNAL_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Result divisor (1.2 times 65535)
 *
 * The 16-bit RAW results have to be divided by 1.2. We do this by using fixed
 * floating point arithmetic by multiplying divisor and RAW value by 65535 (
 * uint16_t max).
 */
#define RES_DIV                 (78642)

/**
 * @name    Opcodes
 * @{
 */
#define OP_POWER_DOWN           (0x00)
#define OP_POWER_ON             (0x01)
#define OP_RESET                (0x03)
#define OP_CONT_HRES1           (0x10)
#define OP_CONT_HRES2           (0x11)
#define OP_CONT_LRES            (0x13)
#define OP_SINGLE_HRES1         (0x20)
#define OP_SINGLE_HRES2         (0x21)
#define OP_SINGLE_LRES          (0x23)
#define OP_CHANGE_TIME_H_MASK   (0x40)
#define OP_CHANGE_TIME_L_MASK   (0x60)
/** @} */

/**
 * @name    Measurement delays (in us)
 * @{
 */
#define DELAY_HMODE             (120000)    /**< typ. 120ms in H-mode */
#define DELAY_LMODE             (16000)     /**< typ. 16ms in L-mode */
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* BH1750FVI_INTERNAL_H */
/** @} */
