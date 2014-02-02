/*
 * Copyright (C) 2014 INRIA
 *
 * The source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup boards
 * @{
 */

/**
 * @file
 * @brief       msba2 common config module functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 */

#include <stdint.h>
#include <string.h>
#include "config.h"
#include "flashrom.h"

void config_load(void) {
    extern char configmem[];
    /* cast it here for strict-aliasing */
    uint16_t* tmp = (uint16_t*) configmem;
    if (*tmp ==  CONFIG_KEY) {
        memcpy(&sysconfig, (configmem + sizeof(CONFIG_KEY)), sizeof(sysconfig));
    }
    else {
        config_save();
    }
}

uint8_t config_save(void) {
    configmem_t mem = { CONFIG_KEY, sysconfig  };
    return (flashrom_erase((uint8_t*) &configmem) && flashrom_write((uint8_t*) &configmem, (char*) &mem, sizeof(mem)));
}
