/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the CFLAGS handling
 *
 * @author      Gaëtan Harter <gaetan.harter@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

/* Define a CFLAGS string with spaces from outside docker */
/* DOCKER_ENVIRONMENT_CMDLINE=$'-e CFLAGS=-DSTRING_FROM_DOCKER=\'\\\"with\ space\\\"\''*/
#ifndef STRING_FROM_DOCKER
#define STRING_FROM_DOCKER ""
#endif

int main(void)
{
    puts("The output of the configuration variables:");
    printf("SUPER_STRING: %s\n", SUPER_STRING);
    printf("DEFINED_AFTER_MAKEFILE_INCLUDE: %u\n", DEFINED_AFTER_MAKEFILE_INCLUDE);
    printf("CFLAGS_STRING_FROM_DOCKER: %s\n", STRING_FROM_DOCKER);
    return 0;
}
