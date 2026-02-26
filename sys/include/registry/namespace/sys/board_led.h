/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_namespace_sys_board_led RIOT Registry Schema: BOARD_LED
 * @ingroup     sys
 * @brief       RIOT Registry BOARD_LED Schema representing the basic structure of a BOARD LED
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"

/* BOARD-LED */
#if IS_USED(MODULE_REGISTRY_NAMESPACE_SYS_BOARD_LED) || IS_ACTIVE(DOXYGEN)

extern const registry_parameter_t registry_sys_board_led_enabled;
extern registry_schema_t registry_sys_board_led;

typedef struct {
    clist_node_t node;
    bool enabled;
} registry_sys_board_led_instance_t;

typedef const enum {
    REGISTRY_SYS_BOARD_LED_ENABLED,
} registry_sys_board_led_indices_t;

#endif

#ifdef __cplusplus
}
#endif
/** @} */
