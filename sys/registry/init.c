/*
 * SPDX-FileCopyrightText: 2023-2026 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_registry_init
 * @brief       RIOT Registry Init module providing init functions
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#include "registry.h"

#include "auto_init_utils.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Registry auto init priority
 */
#ifndef AUTO_INIT_PRIO_MOD_REGISTRY
#  define AUTO_INIT_PRIO_MOD_REGISTRY 2000
#endif

XFA_USE_CONST(registry_namespace_t *, _registry_namespaces_xfa);

void registry_init(void)
{
    /* set namespace_id to its index value */
    for (size_t i = 0; i < XFA_LEN(registry_namespace_t *, _registry_namespaces_xfa); i++) {
        registry_namespace_t *namespace = _registry_namespaces_xfa[i];

        *(registry_namespace_id_t *)&namespace->id = i;
    }
}

AUTO_INIT(registry_init, AUTO_INIT_PRIO_MOD_REGISTRY);
