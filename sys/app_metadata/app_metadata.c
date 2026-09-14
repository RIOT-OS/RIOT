/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup sys_app_metadata
 * @{
 * @file
 * @brief   Prints application metadata such as BOARD, CPU, OS_VERSION.
 * @author  Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */

#include <stdio.h>

void app_metadata_print_json(void)
{
    puts("{\"cmd\": \"app_metadata_print_json()\"}");
    printf("{\"data\": {\"APP_NAME\": \"%s\"}}\n", RIOT_APPLICATION);
    printf("{\"data\": {\"BOARD\": \"%s\"}}\n", RIOT_BOARD);
    printf("{\"data\": {\"CPU\": \"%s\"}}\n", RIOT_CPU);
#ifdef APP_SHELL_FMT
    printf("{\"data\": {\"APP_SHELL_FMT\": \"%s\"}}\n", APP_SHELL_FMT);
#endif
    printf("{\"data\": {\"OS_VERSION\": \"%s\"}}\n", RIOT_VERSION);

    printf("{\"result\": \"SUCCESS\"}\n");
}
