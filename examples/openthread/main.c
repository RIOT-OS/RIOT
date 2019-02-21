/*
 * Copyright (C) 2017 Baptiste CLENET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       OpenThread test application
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 */

#include <stdio.h>

#include "ot.h"

int main(void)
{
    puts("This a test for OpenThread");
    /* Example of how to call OpenThread stack functions */
    puts("Get PANID ");
    uint16_t panid = 0;
    uint8_t res = ot_call_command("panid", NULL, (void*)&panid);
    printf("Current panid: 0x%x (res:%x)\n", panid, res);

    return 0;
}
