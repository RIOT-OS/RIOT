/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_isl29020
 * @{
 *
 * @file
 * @brief       Definitions for the ISL29020 light sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name ISL29020 registers
 * @{
 */
#define ISL29020_REG_CMD        0x00
#define ISL29020_REG_LDATA      0x01
#define ISL29020_REG_HDATA      0x02
/** @} */

/**
 * @name Masks for the ISL29020 command register
 * @{
 */
#define ISL29020_CMD_EN         0x80
#define ISL29020_CMD_MODE       0x40
#define ISL29020_CMD_LIGHT      0x20
#define ISL29020_CMD_RES        0x1c
#define ISL29020_CMD_RAGNE      0x03
/** @} */

/**
 * @name Resolution options
 * @{
 */
#define ISL29020_RES_INT_16     0x00
#define ISL29020_RES_INT_12     0x04
#define ISL29020_RES_INT_8      0x08
#define ISL29020_RES_INT_4      0x0c
#define ISL29020_RES_EXT_ADC    0x10
#define ISL29020_RES_EXT_TIM    0x14
/** @} */

/**
 * @name Range options
 * @{
 */
#define ISL29020_RANGE_1        0x00
#define ISL29020_RANGE_2        0x01
#define ISL29020_RANGE_3        0x02
#define ISL29020_RANGE_4        0x03
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
