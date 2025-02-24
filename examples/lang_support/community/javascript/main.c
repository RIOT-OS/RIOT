/*
 * Copyright (C) 2017 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Example of how to use javascript on RIOT
 *
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

/* include header generated from main.js */
#include "blob/main.js.h"

int js_run(const jerry_char_t *script, size_t script_size)
{

    jerry_value_t parsed_code, ret_value;
    int res = 0;

    /* Initialize engine, no flags, default configuration */

    jerry_init(JERRY_INIT_EMPTY);

    /* Register the print function in the global object. */

    jerryx_handler_register_global((const jerry_char_t *) "print",
                                   jerryx_handler_print);

    /* Setup Global scope code */

    parsed_code = jerry_parse(NULL, 0, script, script_size, JERRY_PARSE_NO_OPTS);

    if (!jerry_value_is_error(parsed_code)) {
        /* Execute the parsed source code in the Global scope */
        ret_value = jerry_run(parsed_code);
        if (jerry_value_is_error(ret_value)) {
            printf("js_run(): Script Error!");
            res = -1;
        }
        jerry_release_value(ret_value);
    }

    jerry_release_value(parsed_code);

    /* Cleanup engine */
    jerry_cleanup();

    return res;
}

int main(void)
{
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);

    printf("Executing main.js:\n");

    js_run(main_js, main_js_len);

    return 0;
}
