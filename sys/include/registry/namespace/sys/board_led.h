/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifndef REGISTRY_NAMESPACE_SYS_BOARD_LED_H
#define REGISTRY_NAMESPACE_SYS_BOARD_LED_H

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

#endif /* REGISTRY_NAMESPACE_SYS_BOARD_LED_H */
/** @} */
