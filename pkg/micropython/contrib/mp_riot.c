/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython RIOT port core glue implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "py/builtin.h"
#include "py/gc.h"
#include "py/lexer.h"
#include "py/nlr.h"
#include "py/runtime.h"

#include "thread.h"
#include "xtimer.h"

#define MP_RIOT_TICKLEN 10000U

static void _mp_riot_tick(void *arg)
{
    mp_handle_pending(true);
    xtimer_set(arg, MP_RIOT_TICKLEN);
}

static xtimer_t _tick_timer = { .callback = _mp_riot_tick, .arg = &_tick_timer };

void mp_riot_init(char *heap, size_t heap_size)
{
    #if MICROPY_ENABLE_GC
    gc_init(heap, heap + heap_size);
    #endif

    mp_init();
}

mp_lexer_t *mp_lexer_new_from_file(qstr filename)
{
    puts("mp_lexer_new_from_file() stub");
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path)
{
    puts("mp_import_stat() stub");
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs)
{
    puts("mp_builtin_open() stub");
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val)
{
    printf("micropython: FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    while (1) {}
}

static void _call_from_isr(mp_obj_t callback)
{
    char *saved_stack_top = MP_STATE_THREAD(stack_top);
    size_t saved_stack_limit = MP_STATE_THREAD(stack_limit);

    MP_STATE_THREAD(stack_top) = thread_isr_stack_start() + ISR_STACKSIZE;
    MP_STATE_THREAD(stack_limit) = ISR_STACKSIZE;

    gc_lock();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_0(callback);
        nlr_pop();
    }
    else {
        puts("micropython: uncaught exception in ISR");
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
    }
    gc_unlock();

    MP_STATE_THREAD(stack_top) = saved_stack_top;
    MP_STATE_THREAD(stack_limit) = saved_stack_limit;
}

void mp_riot_isr_callback(void *arg)
{
    _call_from_isr(*(mp_obj_t *)arg);
}
