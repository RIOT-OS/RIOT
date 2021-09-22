/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * Copyright (C) 2020 TU Bergakademie Freiberg Karl Fessel
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

//
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#include "wasm_export.h"
#pragma GCC diagnostic pop

#include <thread.h>



#define DEFAULT_THREAD_STACKSIZE (6 * 1024)
#define DEFAULT_THREAD_PRIORITY 50


/* execs the main function in an instantiated module */
static int iwasm_instance_exec_main(wasm_module_inst_t module_inst, int argc, char **argv)
{
    /* exception memory is part of instance -> no buffer need */
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
int iwasm_bytecode_exec_main(uint8_t * bytecode, size_t bytecode_len, int argc, char **argv){
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

#include "event.h"
event_queue_t iwasm_q = EVENT_QUEUE_INIT_DETACHED;

void * iwasm_thread(void *arg1)
{
    (void) arg1; /*unused*/

    if(!iwasm_runtime_init())
        return (void *)-1;

    event_queue_claim(&iwasm_q);
    event_loop(&iwasm_q);

    /*next lines should not be reached*/
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
    event_queue_init_detached(&iwasm_q);
    b.stack=malloc(b.stacksize);
    kernel_pid_t tpid = thread_create (b.stack, b.stacksize, b.priority, b.flags, b.task_func, b.arg, b.name);

    return tpid != 0 ? true : false;;
}

#include "mutex.h"

typedef struct run_bytecode_event { event_t super;
                                    void *bytecode;
                                    size_t bytecode_len;
                                    int argc;
                                    char **argv;
                                    mutex_t finish;
} run_bytecode_event_t;

void _wamr_run_call(event_t *event){
    run_bytecode_event_t * e = (run_bytecode_event_t *) event;
    e->argc = iwasm_bytecode_exec_main((uint8_t * )e->bytecode, e->bytecode_len, e->argc, e->argv);
    mutex_unlock(&e->finish);
}

/* this seems to be a very direct interpretation of "i like to have a wamr_run" */
int wamr_run(void *bytecode, size_t bytecode_len, int argc, char **argv){
    run_bytecode_event_t * e = malloc(sizeof(run_bytecode_event_t));
    if (!e) return -1;

    *e = (run_bytecode_event_t){ .super.handler = _wamr_run_call,
                                    .bytecode = bytecode,
                                    .bytecode_len = bytecode_len,
                                    .argc = argc, .argv = argv,
                                    .finish = MUTEX_INIT_LOCKED};
    event_post(&iwasm_q, (event_t*) e);

    mutex_lock(&e->finish);

    int ret = e->argc;
    free(e);
    return ret;
}


int wamr_run_cp(const void *bytecode, size_t bytecode_len, int argc, char *argv[]){
    /* we need the bytecode to be writable while loading
     * loading adds size information to stack POPs */
    uint8_t * wasm_buf = malloc(bytecode_len);
    if (!wasm_buf) return -1;

    memcpy(wasm_buf, bytecode, bytecode_len);
    /* no copy need if architecture has const in writable mem */
    /* wasm_buf = (uint8_t *) main_wasm; */

    unsigned wasm_buf_size = bytecode_len;

    /* iwasm uses argv[0] casted to an int to store mains return value */
    static char * empty = "";
    char ** parv;
    if(argc > 0){
        parv =  malloc(sizeof(argv[0]) * argc);
        if (!parv) return -1;
        memcpy(parv, argv, sizeof(argv[0]) * argc);
    }else{
        argc = 1;
        parv = malloc(sizeof(argv[0]) * argc);
        if (!parv) return -1;
        parv[0] = empty;
    }
    int ret = wamr_run(wasm_buf, wasm_buf_size, argc, parv);
    free(parv);
    free(wasm_buf);

    return ret;
}
