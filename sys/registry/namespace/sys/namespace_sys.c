/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry_schema RIOT Registry Schema
 * @ingroup     sys
 * @brief       RIOT Registry Schema module providing common sys configuration schemas for the RIOT Registry sys module
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "kernel_defines.h"
#include "registry.h"

#include "registry/namespace/sys.h"
#include "registry/namespace/sys/rgb_led.h"
#include "registry/namespace/sys/board_led.h"

static const registry_schema_t *_schemas[] = {
#if IS_USED(MODULE_REGISTRY_NAMESPACE_SYS_RGB_LED)
    &registry_sys_rgb_led,
#endif
#if IS_USED(MODULE_REGISTRY_NAMESPACE_SYS_BOARD_LED)
    &registry_sys_board_led,
#endif
};

registry_namespace_t registry_sys = {
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_NAME) || IS_ACTIVE(DOXYGEN)
    .name = "sys",
#endif /* CONFIG_REGISTRY_ENABLE_META_NAME */
#if IS_ACTIVE(CONFIG_REGISTRY_ENABLE_META_DESCRIPTION) || IS_ACTIVE(DOXYGEN)
    .description = "Sys namespace",
#endif /* CONFIG_REGISTRY_ENABLE_META_DESCRIPTION */
    .schemas = _schemas,
    .schemas_len = ARRAY_SIZE(_schemas),
};

REGISTRY_ADD_NAMESPACE(sys, registry_sys);
