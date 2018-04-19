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
    puts("If it compiles, it works!");
    printf("Message: %s\n", external_module_message);
    return 0;
}
