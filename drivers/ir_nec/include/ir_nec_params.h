/*
 * Copyright (C) 2022 Dario Petrillo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ir_nec
 *
 * @{
 * @file
 * @brief       Default configuration
 *
 * @author      Dario Petrillo <dario.pk1@gmail.com>
 */

#include "board.h"
#include "ir_nec.h"
#include "ir_nec_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters
 * @{
 */
#ifndef IR_NEC_PARAM_PIN
#define IR_NEC_PARAM_PIN    GPIO_UNDEF
#endif

#ifndef IR_NEC_PARAMS
#define IR_NEC_PARAMS   { .pin = IR_NEC_PARAM_PIN }
#endif
/**@}*/

/**
 * @brief   Configuration struct
 */
static const ir_nec_params_t ir_nec_params[] =
{
    IR_NEC_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
