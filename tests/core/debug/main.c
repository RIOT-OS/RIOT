/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @brief       Test debug.h with different configurations
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 */

#include <inttypes.h>

#include "tests.h"

int main(void)
{
    disabled();

    noprefix();
    noprefix_func();
    noprefix_thread();
    noprefix_thread_func();

    prefix();
    prefix_func();
    prefix_thread();
    prefix_thread_func();
}
