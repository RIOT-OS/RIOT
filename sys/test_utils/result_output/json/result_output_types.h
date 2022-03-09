/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef RESULT_OUTPUT_TYPES_H
#define RESULT_OUTPUT_TYPES_H

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable to add a whitespace after a whitespace after a symbol
 *
 * e.g.: {"key1":1,"key2":2}" -> {"key1": 1, "key2: 2}
 */
#ifndef CONFIG_TURO_JSON_WHITESPACE_AFTER_SYMBOL
#define CONFIG_TURO_JSON_WHITESPACE_AFTER_SYMBOL 1
#endif

/**
 * @brief   States of the TURO container
 * @{
 */
typedef enum {
    TURO_STATE_READY,       /**< state ready */
    TURO_STATE_NEED_COMMA,  /**< next entry will need a comma */
} turo_state_t;
/** @} */

/**
 * @brief   turo type
 * @internal
 */
struct turo {
    turo_state_t state; /**< current state */
};

#ifdef __cplusplus
}
#endif
#endif /* RESULT_OUTPUT_TYPES_H */
