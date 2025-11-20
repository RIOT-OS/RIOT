/*
 * SPDX-FileCopyrightText: 2020 TU Bergakademie Freiberg Karl Fessel
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*provide some test program*/
#include "blob/test.wasm.h"
#include "blob/hello.wasm.h"

bool iwasm_runtime_init(void);
void iwasm_runtime_destroy(void);

/* wamr_run is a very direct interpretation of "i like to have a wamr_run" */
int wamr_run(void *bytecode, size_t bytecode_len, int argc, char **argv);

/* wamr_run_cp creates a copy bytecode and argv
 * if argc is 0 it is set to 1 and argv[0] is set to ""
 * to create some space for a return value */
int wamr_run_cp(const void *bytecode, size_t bytecode_len, int argc, const char **argv);

#define telltruth(X) ((X) ? "true" : "false")

int main(void)
{
    printf("iwasm_initilised: %s\n", telltruth(iwasm_runtime_init()));

    int app_argc = 2;
    const char *app_argv[] = {"test", "bob"};
    int ret = wamr_run_cp(test_wasm, test_wasm_len, app_argc, app_argv);
    printf("ret = %d\n", ret);
    ret = wamr_run_cp(hello_wasm, hello_wasm_len, app_argc, app_argv);
    printf("ret = %d\n", ret);

    iwasm_runtime_destroy();

}
