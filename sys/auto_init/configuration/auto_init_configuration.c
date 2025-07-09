/*
 * Copyright 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_configuration
 * @{
 *
 * @file
 * @brief       Auto initialization for configuration subsystems
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#ifdef MODULE_CONFIGURATION

#include "auto_init_utils.h"
#include "configuration.h"
#if IS_USED(MODULE_RIOTCONF)
#include "configuration_backend_riotconf.h"
#endif

XFA_INIT_CONST(auto_init_module_t, auto_init_configuration_xfa);

#ifndef CONFIG_CONFIGURATION_ROOT_BACKEND_OPS
#if IS_USED(MODULE_RIOTCONF)
#define CONFIG_CONFIGURATION_ROOT_BACKEND_OPS &conf_backend_riotconf_ops
#else
#define CONFIG_CONFIGURATION_ROOT_BACKEND_OPS NULL
#endif
#endif

static const CONF_BACKEND(_root_backend,
                          CONFIG_CONFIGURATION_ROOT_BACKEND_OPS,
                          CONF_FMT_CBOR);

__attribute__((weak))
void auto_init_configuration_root_backend_init(void)
{
    *configuration_get_src_backend(configuration_get_root()) = &_root_backend;
    *configuration_get_dst_backend(configuration_get_root()) = &_root_backend;
}

void auto_init_configuration(void)
{
    for (unsigned i = 0; i < XFA_LEN(auto_init_module_t, auto_init_configuration_xfa); i++) {
        auto_init_module(&auto_init_configuration_xfa[i]);
    }
    auto_init_configuration_root_backend_init();
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CONFIGURATION */
