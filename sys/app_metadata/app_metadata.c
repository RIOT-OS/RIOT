/**
Copyright (C) 2019, HAW Hamburg.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_app_metadata
 * @{
 * @file
 * @brief   Prints application metadata such as BOARD, CPU, OS_VERSION.
 * @author  Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */

#include <stdio.h>

#include "buildinfo/app.h"
#include "buildinfo/board.h"
#include "buildinfo/cpu.h"
#include "buildinfo/riotver.h"

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
