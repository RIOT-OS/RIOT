/*
 * Copyright 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
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

XFA_INIT_CONST(auto_init_module_t, auto_init_configuration_xfa);

void auto_init_configuration(void)
{
    for (unsigned i = 0; i < XFA_LEN(auto_init_module_t, auto_init_configuration_xfa); i++) {
        auto_init_module(&auto_init_configuration_xfa[i]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_CONFIGURATION */
