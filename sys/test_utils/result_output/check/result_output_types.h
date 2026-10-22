/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_TURO_MAX_NESTING_LEVELS
#define CONFIG_TURO_MAX_NESTING_LEVELS 32 /**< max level of state nesting */
#endif

/**
 * @brief   States of the TURO container
 * @{
 */
typedef enum {
    TURO_STATE_UNKNOWN,     /**< unknown state */
    TURO_STATE_READY,       /**< state ready */
    TURO_STATE_CONTAINER,   /**< container open or closing */
    TURO_STATE_DICT_OPENED, /**< dictionary opened */
    TURO_STATE_ARRAY_OPENED /**< array opening */
} turo_state_t;
/** @} */

/**
 * @brief   turo type
 * @internal
 */
struct turo {
    size_t idx;                                             /**< index for states */
    turo_state_t states[CONFIG_TURO_MAX_NESTING_LEVELS];    /**< state buffer */
};

#ifdef __cplusplus
}
#endif
