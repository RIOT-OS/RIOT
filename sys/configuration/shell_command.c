/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_configuration_shell_command
 * @{
 *
 * @file
 * @brief       RIOT configuration shell command
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "configuration.h"
#if MODULE_CONFIGURATION_BACKEND_RIOTCONF
#include "configuration_backend_riotconf.h"
#endif
#if MODULE_RIOTCONF_SLOT_PRINT
#include "riotconf/slot.h"
#endif
#include "shell.h"

static int _sc_config_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s\n"
               "  [load]\n"
               "  [save]\n"
               "  [reset]\n"
#if MODULE_CONFIGURATION_BACKEND_RIOTCONF && MODULE_RIOTCONF_SLOT_PRINT
               "  [print]\n"
#endif
               "\n",
               argv[0]);
        return -1;
    }
    if (!strcmp(argv[1], "save")) {
        CONF_KEY(root_key, 0, CONFIGURATION_KEY_BUF_MAX);
        configuration_lock(&root_key);
        int ret = configuration_export(&root_key);
        configuration_unlock(&root_key);
        if (ret) {
            puts("data could not be saved");
            return -1;
        }
        puts("data is saved successfully to the persistent backend");
    }
    else if (!strcmp(argv[1], "load")) {
        CONF_KEY(root_key, 0, CONFIGURATION_KEY_BUF_MAX);
        configuration_lock(&root_key);
        int ret = configuration_import(&root_key);
        configuration_unlock(&root_key);
        if (ret) {
            puts("data could not be loaded");
            return -1;
        }
        puts("data is loaded successfully from the persistent backend");
    }
    else if (!strcmp(argv[1], "reset")) {
        CONF_KEY(root_key, 0, CONFIGURATION_KEY_BUF_MAX);
        configuration_lock(&root_key);
        int ret = configuration_reset(&root_key);
        configuration_unlock(&root_key);
        if (ret) {
            puts("data could not be reset");
            return -1;
        }
        puts("data is reset successfully");
    }
#if MODULE_CONFIGURATION_BACKEND_RIOTCONF && MODULE_RIOTCONF_SLOT_PRINT
    else if (!strcmp(argv[1], "print")) {
        riotconf_slot_t slot = configuration_backend_riotconf_slot_current();
        if (argc >= 3) {
            slot = atoi(argv[2]);
        }
        riotconf_slot_print(slot);
    }
#endif
    else {
        printf("unknown configuration subcommand: %s\n", argv[1]);
        return -1;
    }
    return 0;
}

SHELL_COMMAND(config, "configuration", _sc_config_cmd);
