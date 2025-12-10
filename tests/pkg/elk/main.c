/*
 * SPDX-FileCopyrightText: 2013-2021 Cesanta Software Limited
 * SPDX-License-Identifier: GPL-2.0-only
 * SPDX-FileCopyrightText: Alternatively, you can license this software under a
 * SPDX-FileCopyrightText: commercial license, please contact us at
 * SPDX-FileCopyrightText: https://cesanta.com/contact.html
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
