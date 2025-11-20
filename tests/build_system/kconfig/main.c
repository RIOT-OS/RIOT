/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test for Kconfig configuration
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include "app.h"
#include "kernel_defines.h"
#include "external_module_1.h"
#include "external_module_2.h"
#include "external_pkg_1.h"
#include "external_pkg_2.h"

int main(void)
{
    puts(CONFIG_APP_MSG_1_TEXT);

    if (IS_ACTIVE(CONFIG_APP_MSG_2)) {
        puts("MSG_2 is active");
    }

    puts(CONFIG_EXTERNAL_MODULE_1_MESSAGE);
    puts(CONFIG_EXTERNAL_MODULE_2_MESSAGE);

    puts(CONFIG_EXTERNAL_PKG_1_MESSAGE);
    puts(CONFIG_EXTERNAL_PKG_2_MESSAGE);

    return 0;
}
