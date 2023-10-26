/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Application showcasing the use of subfolders in RIOT applications
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>

void module_a(void);
void module_b(void);
void folder_a(void);
void folder_b(void);

int main(void)
{
    puts("./main.c");
    // call functions from RIOT module
    module_a();
    module_b();
    // call functions from subfolder
    folder_a();
    folder_b();

    return 0;
}
