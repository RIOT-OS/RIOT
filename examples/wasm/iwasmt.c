/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * Copyright (C) 2020 TU Bergakademie Freiberg Karl Fessel
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

//
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#include "wasm_export.h"
#pragma GCC diagnostic pop

#include <thread.h>

/*provide some test program*/
#include "blob/main.wasm.h"

#define DEFAULT_THREAD_STACKSIZE (6 * 1024)
#define DEFAULT_THREAD_PRIORITY 50

static int app_argc;
static char **app_argv;

/* execs the main function in an instantiated module */
static int iwasm_instance_exec_main(wasm_module_inst_t module_inst, int argc, char **argv)
{
    const char *exception = 0;
    int ret = 0;
    wasm_application_execute_main(module_inst, argc, argv);
    if ((exception = wasm_runtime_get_exception(module_inst))){
        puts(exception);
        return -1;
    }
    if(argc > 0) ret = *((int*)argv);
    return ret;
}

/* execs the main function of a loaded module */
int iwasm_module_exec_main(wasm_module_t wasm_module , int argc, char **argv)
{
    wasm_module_inst_t wasm_module_inst = NULL;
    char error_buf[128];
    int ret = 0;

    /* instantiate the module */
    if (!(wasm_module_inst = wasm_runtime_instantiate(wasm_module, 8 * 1024,
        8 * 1024, error_buf, sizeof(error_buf)))) {
        puts(error_buf);
        return -1;
    }
    ret = iwasm_instance_exec_main(wasm_module_inst, argc, argv);
    /* destroy the module instance */
    wasm_runtime_deinstantiate(wasm_module_inst);
    return ret;
}

/* bytecode needs to be writeable*/
int iwasm_bytecode_exec_main(void *bytecode, size_t bytecode_len, int argc, char **argv){
//  (uint8_t *) bytecode;
    int ret = 0;
    wasm_module_t wasm_module = NULL;

    {   /* load WASM module */
        char error_buf[128];
        if (!(wasm_module = wasm_runtime_load((uint8_t * )bytecode, bytecode_len,
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
/* chose allocator non defaults to system allocator */
#define FUNC_ALLOC
//#define POOL_ALLOC

    memset(&init_args, 0, sizeof(RuntimeInitArgs));
#ifdef POOL_ALLOC
    static char global_heap_buf[256 * 1024] = { 0 };//(256 kB)

    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
#elif defined(FUNC_ALLOC)
    init_args.mem_alloc_type = Alloc_With_Allocator;
/*deactivate Wpedantic for some lines*/
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    init_args.mem_alloc_option.allocator.malloc_func = malloc;
    init_args.mem_alloc_option.allocator.realloc_func = realloc;
    init_args.mem_alloc_option.allocator.free_func = free;
#pragma GCC diagnostic pop
#else
    init_args.mem_alloc_type = Alloc_With_System_Allocator;
#endif

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        puts("Init runtime environment failed.");
        return false;
    }
    return true;
}

void * iwasm_thread(void *arg1)
{
    (void) arg1; /*unused*/
    uint8_t *wasm_buf = NULL;
    unsigned wasm_buf_size = 0;

    if(!iwasm_runtime_init())
        return (void *)-1;

    /* we need the bytecode to be writable while loading
     * loading adds size information to stack POPs */
    //static uint8_t wasm_test_file[] = main_wasm;
    wasm_buf = malloc(main_wasm_len);
    memcpy(wasm_buf, main_wasm, main_wasm_len);
    /* no copy need if architecture has const in writable mem */
    /* wasm_buf = (uint8_t *) main_wasm; */
    wasm_buf_size = main_wasm_len;

    iwasm_bytecode_exec_main(wasm_buf, wasm_buf_size, app_argc, app_argv);

    free(wasm_buf);
    wasm_runtime_destroy();
    return NULL;
}

bool iwasm_start_thread(void)
{
    struct{
        char *  stack;
        int  stacksize;
        uint8_t  priority;
        int  flags;
        thread_task_func_t task_func;
        void *  arg;
        const char * name;
    } b = {
        .stacksize = DEFAULT_THREAD_STACKSIZE,
        .priority = 8,
        .flags = 0,
        .task_func = iwasm_thread,
        .arg = NULL,
        .name = "simple_wamr"
    };

    b.stack=malloc(b.stacksize);
    kernel_pid_t tpid = thread_create (b.stack, b.stacksize, b.priority, b.flags, b.task_func, b.arg, b.name);

    return tpid != 0 ? true : false;;
}

/* this seems to be a very direct interpretation of i like to have a wamr_run */
int wamr_run(void *bytecode, size_t bytecode_len, int argc, char **argv){
    return iwasm_bytecode_exec_main(bytecode, bytecode_len, argc, argv);
}

#define telltruth(X) ((X) ? "true" : "false")
int main(void)
{
    printf("iwasm_thread_initilised: %s\n",telltruth(iwasm_start_thread()));
}
