/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Print over USB CDC ACM
 *
 * @}
 */

#include <stdio.h>

#include "busy_wait.h"

int main(void)
{
    while (1) {
        puts("Hello from the RP2350 over USB CDC ACM!");
        busy_wait(1000000); /* ~1 s */
    }

    return 0;
}
