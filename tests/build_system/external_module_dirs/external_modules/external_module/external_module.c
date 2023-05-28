/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the EXTERNAL_MODULE_DIRS feature
 * @note        Define a shared variable
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 *
 * @}
 */

#include "external_module.h"
#include "auto_init_priorities.h"
#include "auto_init_utils.h"

#define PRIO AUTO_INIT_PRIORITY_AFTER(AUTO_INIT_PRIO_MOD_RANDOM)

AUTO_INIT(auto_init_external_module, PRIO);

bool external_module_initialized = false;
char *external_module_message = "Linking worked";

void auto_init_external_module(void)
{
    external_module_initialized = true;
}
