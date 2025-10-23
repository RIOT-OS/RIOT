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
 *
 * @}
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "registry.h"
#include "registry/namespace/sys.h"

#include "registry/namespace/sys/board_led.h"

#if IS_USED(MODULE_REGISTRY_NAMESPACE_SYS_BOARD_LED) || IS_ACTIVE(DOXYGEN)

/* Mapping */
static void mapping(const registry_parameter_id_t parameter_id, const registry_instance_t *instance,
                    void **val, size_t *val_len)
{
    registry_sys_board_led_instance_t *_instance =
        (registry_sys_board_led_instance_t *)instance->data;

    switch (parameter_id) {
    case REGISTRY_SYS_BOARD_LED_ENABLED:
        *val = &_instance->enabled;
        *val_len = sizeof(_instance->enabled);
        break;
    }
}

/* Schema parameters */
const registry_parameter_t registry_sys_board_led_enabled = {
    .id = REGISTRY_SYS_BOARD_LED_ENABLED,
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "enabled",
#  endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "State of the board LED.",
#  endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schema = &registry_sys_board_led,
    .type = REGISTRY_TYPE_BOOL,
    .count = 1,
};

/* Schema */
registry_schema_t registry_sys_board_led = {
    .id = REGISTRY_SYS_BOARD_LED,
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "board_led",
#  endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#  if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "Representation of a board LED.",
#  endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .namespace = &registry_sys,
    .mapping = mapping,
    .groups = NULL,
    .groups_len = 0,
    .parameters = (const registry_parameter_t *[]){
        &registry_sys_board_led_enabled,
    },
    .parameters_len = 1,
};

#endif
