/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry_init RIOT Registry initialization
 * @ingroup     sys
 * @brief       RIOT Registry Init module providing init functions
 * @{
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 *
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "registry.h"

#include "auto_init_utils.h"
#if IS_USED(MODULE_AUTO_INIT)
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

AUTO_INIT(registry_init, 2000);
