/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Test wrappers to allways use variants of stdio functions
 *              str and formaters reside in program memory
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 */

#include <stdio.h>

int main(void)
{
    char buffer[50];
    puts("Testing puts");
    printf("Testing %s\n", "printf");
    sprintf(buffer, "Testing %s\n", "sprintf");
    printf("%s",buffer);
    snprintf_P(buffer, sizeof("Testing snprintf\n"), "Testing %s\n", "snprintf");
    printf("%s",buffer);
    // vsprintf_P(buffer, _f, _ap);
    // vsnprintf_P(buffer, _n, _f, _ap);
    // fprintf_P(_F, _f, ...);
    // fputs_P(_str, _F);
    // vfscanf_P(_F, _f, _ap);
    // fscanf_P(_F, _f, ...);
    // scanf_P(_f, ...);
    // sscanf_P(_b, _f, ...);
    // vfprintf(_F, _f, _ap);

    return 0;
}
