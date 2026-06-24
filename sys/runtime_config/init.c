/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_runtime_config_init
 * @brief       Runtime configuration init module providing init functions
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include "runtime_config.h"

#include "auto_init_utils.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Runtime configuration auto init priority
 */
#ifndef AUTO_INIT_PRIO_MOD_RUNTIME_CONFIG
#  define AUTO_INIT_PRIO_MOD_RUNTIME_CONFIG 2000
#endif

XFA_USE_CONST(runtime_config_namespace_t *, _runtime_config_namespaces_xfa);

void runtime_config_init(void)
{
    /* set namespace_id to its index value */
    for (size_t i = 0; i < XFA_LEN(runtime_config_namespace_t *, _runtime_config_namespaces_xfa); i++) {
        runtime_config_namespace_t *namespace = _runtime_config_namespaces_xfa[i];

        *(runtime_config_namespace_id_t *)&namespace->id = i;
    }
}

AUTO_INIT(runtime_config_init, AUTO_INIT_PRIO_MOD_RUNTIME_CONFIG);
