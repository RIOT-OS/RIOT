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

#include "mp_riot.h"

void mp_riot_init(char *heap, size_t heap_size)
{
#if MICROPY_ENABLE_GC
    gc_init(heap, heap + heap_size);
#endif

    mp_init();
}

void mp_riot_deinit(void)
{
#if MICROPY_ENABLE_GC
    gc_sweep_all();
#endif

    mp_deinit();
}

mp_lexer_t *mp_lexer_new_from_file(qstr filename)
{
    return NULL;
}

mp_import_stat_t mp_import_stat(const char *path)
{
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs)
{
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

void nlr_jump_fail(void *val)
{
    printf("micropython: FATAL: uncaught exception %p\n", val);
    mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(val));
    while (1) {}
}

void mp_riot_call_function_safe(mp_obj_t callback)
{
    /* Runs in thread context (the REPL thread, via mp_handle_pending()), so we
     * use that thread's stack and stack limits as set up in main() and must not
     * touch MP_STATE_THREAD(stack_*) here. Only guard against an uncaught Python
     * exception escaping into the calling C code. */
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_call_function_0(callback);
        nlr_pop();
    }
    else {
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
    }
}
