/*
 * tmp117_params.h
 *
 *  Created on: 13 mai 2026
 *      Author: leo
 */

#pragma once

#include "board.h"
#include "tmp117.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default configuration parameters for TMP117 sensors
 * @{
 */
#ifndef TMP117_PARAM_I2C
#  define TMP117_PARAM_I2C      (I2C_DEV(0))
#endif
#ifndef TMP117_PARAM_ADDR
#  define TMP117_PARAM_ADDR     (0x48)
#endif
#ifndef TMP117_PARAM_CONV_MODE
#  define TMP117_PARAM_CONV_MODE	TMP117_CONV_CC
#endif
#ifndef TMP117_PARAM_CONV_CYCLE
#  define TMP117_PARAM_CONV_CYCLE   TMP117_CONV_CYCLE_1_S
#endif
#ifndef TMP117_PARAM_AVG
#  define TMP117_PARAM_AVG     		TMP117_AVG_8
#endif


#ifndef TMP117_PARAMS
#  define TMP117_PARAMS       { .i2c  = TMP117_PARAM_I2C, \
		.addr = TMP117_PARAM_ADDR, \
		.conv_mode = TMP117_PARAM_CONV_MODE, \
		.conv_cycle = TMP117_PARAM_CONV_CYCLE, \
		.avg = TMP117_PARAM_AVG }
#endif
/** @} */

/**
 * @brief   Allocation of TMP117 configuration
 */
static const tmp117_params_t tmp117_params[] =
{
		TMP117_PARAMS
};

#ifdef __cplusplus
}
#endif
