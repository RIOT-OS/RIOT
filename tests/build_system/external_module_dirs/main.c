/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the EXTERNAL_MODULE_DIRS feature
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "external_module.h"

#ifdef MODULE_PRNG_TINYMT32
#error "Error: it included the default dependency"
#endif

#ifndef MODULE_PRNG_XORSHIFT
#error "Dependency not included"
#endif

int main(void)
{
    printf("Message: %s\n", external_module_message);
    if (!external_module_initialized) {
        puts("External module has not been initialized.");
        return 1;
    }
    puts("Initialization worked!");
    return 0;
}
