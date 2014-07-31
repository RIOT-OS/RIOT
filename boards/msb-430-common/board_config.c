/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup boards
 * @{
 */

/**
 * @file
 * @brief       msb-430 common config module functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include <stdint.h>
#include <string.h>
#include "board-conf.h"
#include "config.h"
#include "flashrom.h"

void config_load(void)
{
    if (*((uint16_t *) INFOMEM) ==  CONFIG_KEY) {
        memcpy(&sysconfig, (char *)(INFOMEM + sizeof(CONFIG_KEY)), sizeof(sysconfig));
    }
    else {
        config_save();
    }
}

uint8_t config_save(void)
{
    configmem_t mem = { CONFIG_KEY, sysconfig  };
    return (flashrom_erase((uint8_t *) INFOMEM) && flashrom_write((uint8_t *) INFOMEM, (uint8_t *) &mem, sizeof(mem)));
}
