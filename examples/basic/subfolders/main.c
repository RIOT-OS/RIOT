/*
 * SPDX-FileCopyrightText: 2023 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
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
void folder_c(void);

int main(void)
{
    puts("./main.c");
    // call functions from RIOT module
    module_a();
    module_b();
    // call functions from subfolder
    folder_a();
    folder_b();
    folder_c();

    return 0;
}
