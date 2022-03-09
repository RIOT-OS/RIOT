/*
 * Copyright (C) 2013-2021 Cesanta Software Limited
 * All rights reserved
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http://www.gnu.org/licenses/>.
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, please contact us at https://cesanta.com/contact.html
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the Elk Javascript engine
 *
 *              Taken from README at https://github.com/cesanta/elk
 * @}
 */
#include <stdio.h>
#include "elk.h"

static char mem[256];

/* C function that adds two numbers. Will be called from JS */
int sum(int a, int b)
{
    return a + b;
}

int main(void)
{
    struct js *js = js_create(mem, sizeof(mem));  /* Create JS instance */
    jsval_t v = js_import(js, (uintptr_t)sum, "iii"); /* Import C function "sum" */
    js_set(js, js_glob(js), "f", v);              /* Under the name "f" */
    jsval_t result = js_eval(js, "f(3, 4);", ~0); /* Call "f" */
    printf("result: %s\n", js_str(js, result));   /* result: 7 */
    return 0;
}
