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

#define AUTO_INIT_PRIO_EXTERNAL_MODULE                          \
        AUTO_INIT_PRIO_ADD_AFTER_MODULE(AUTO_INIT_PRIO_GP_RNG,  \
                                        AUTO_INIT_PRIO_RANDOM,  \
                                        AUTO_INIT_PRIO_1)

AUTO_INIT_MODULE(external_module, AUTO_INIT_PRIO_EXTERNAL_MODULE, auto_init_external_module);

bool external_module_initialized = false;
char *external_module_message = "Linking worked";

void auto_init_external_module(void)
{
    external_module_initialized = true;
}
