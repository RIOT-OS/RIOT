/*
 * Copyright (C) 2015  Kushal Singh <kushal.spiderman.singh@gmail.com
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
 * @brief       Basic Example to show dynamic linking in RIOT.
 *
 * @author      Kushal Singh <kushal.spiderman.singh@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "app/app.h"

#include "elf_loader.h"

typedef int (*f_ptr)(void);

int main(void)
{

    f_ptr ptr = (f_ptr) &app_elf+165 ;
    /* Process internal relocations */
    list_symbol_info((char *)app_elf);

    /* Modify branch instructions */
    elf_relocate_internal((char *)app_elf);

    printf("Entering the app_elf function ..\n");
    int res = ptr();
    printf("res=%d\n", res);

    return 0;
}

