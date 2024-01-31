/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * Copyright (C) 2020 TU Bergakademie Freiberg Karl Fessel
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wasm_export.h"

/* execs the main function in an instantiated module */
static int iwasm_instance_exec_main(wasm_module_inst_t module_inst, int argc, char **argv)
{
    /* exception memory is part of instance -> no buffer need */
    const char *exception = 0;
    int ret = 0;
    wasm_application_execute_main(module_inst, argc, argv);
    if ((exception = wasm_runtime_get_exception(module_inst))) {
        puts(exception);
        return -1;
    }
    if (argc > 0) {
        ret = *((int*)argv);
    }
    return ret;
}

/* execs the main function of a loaded module */
int iwasm_module_exec_main(wasm_module_t wasm_module, int argc, char **argv)
{
    wasm_module_inst_t wasm_module_inst = NULL;
    int ret = 0;

    {   /* instantiate the module */
        char error_buf[128];
        if (!(wasm_module_inst = wasm_runtime_instantiate(wasm_module, 8 * 1024,
            8 * 1024, error_buf, sizeof(error_buf)))) {
            puts(error_buf);
            return -1;
            }
    }
    ret = iwasm_instance_exec_main(wasm_module_inst, argc, argv);
    /* destroy the module instance */
    wasm_runtime_deinstantiate(wasm_module_inst);
    return ret;
}

/* bytecode needs to be writeable*/
int iwasm_bytecode_exec_main(uint8_t *bytecode, size_t bytecode_len, int argc, char **argv)
{
    int ret = 0;
    wasm_module_t wasm_module = NULL;

    {   /* load WASM module */
        char error_buf[128];
        if (!(wasm_module = wasm_runtime_load(bytecode, bytecode_len,
                                              error_buf, sizeof(error_buf)))) {
            puts(error_buf);
            return -1;
        }
    }
    ret = iwasm_module_exec_main(wasm_module, argc, argv);
    /* unload WASM module */
    wasm_runtime_unload(wasm_module);
    return ret;
}

/* running this initialises the wamr runtime*/
bool iwasm_runtime_init(void)
{
    RuntimeInitArgs init_args;
    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    /* using default system allocator, pool allocator and
     * and allocator function hooks are available
     * see <wamr>/core/iwasm/include/wasm_export.h l99..*/
    init_args.mem_alloc_type = Alloc_With_System_Allocator;

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        puts("Init runtime environment failed.");
        return false;
    }
    return true;
}

void iwasm_runtime_destroy(void)
{
    wasm_runtime_destroy();
}

/* this seems to be a very direct interpretation of "i like to have a wamr_run" */
int wamr_run(void *bytecode, size_t bytecode_len, int argc, char **argv)
{
    return iwasm_bytecode_exec_main((uint8_t *)bytecode, bytecode_len, argc, argv);
}

/* this creates a copy of bytecode and argv
 * if argc is 0 it is set to 1 and argv[0] is set to ""
 * to create some space for a return value */
int wamr_run_cp(const void *bytecode, size_t bytecode_len, int argc, char *argv[])
{
    /* we need the bytecode to be writable while loading
     * loading adds size information to stack POPs */
    uint8_t * wasm_buf = malloc(bytecode_len);
    if (!wasm_buf) {
        return -1;
    }

    memcpy(wasm_buf, bytecode, bytecode_len);
    /* no copy need if architecture has const in writable mem */
    /* wasm_buf = (uint8_t *) main_wasm; */

    unsigned wasm_buf_size = bytecode_len;

    /* iwasm uses argv[0] cast to an int to store mains return value */
    static char * empty = "";
    char ** parv;
    if (argc > 0) {
        parv =  malloc(sizeof(argv[0]) * argc);
        if (!parv){
            free(wasm_buf);
            return -1;
        }
        memcpy(parv, argv, sizeof(argv[0]) * argc);
    }
    else {
        argc = 1;
        parv = malloc(sizeof(argv[0]) * argc);
        if (!parv) {
            free(wasm_buf);
            return -1;
        }
        parv[0] = empty;
    }
    int ret = wamr_run(wasm_buf, wasm_buf_size, argc, parv);
    free(parv);
    free(wasm_buf);

    return ret;
}
