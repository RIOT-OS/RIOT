/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython xtimer module bindings
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "py/mpconfig.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "py/objstr.h"
#include "mp_riot.h"

#include "container.h"
#include "xtimer.h"

#if MICROPY_PY_XTIMER

typedef struct _mp_obj_xtimer_t {
    mp_obj_base_t base;
    xtimer_t t;
    mp_sched_node_t sched_node;
    mp_obj_t callback;
} mp_obj_xtimer_t;

/* Runs in thread context once mp_handle_pending() drains the scheduler. */
static void _xtimer_sched_callback(mp_sched_node_t *node)
{
    mp_obj_xtimer_t *o = container_of(node, mp_obj_xtimer_t, sched_node);

    mp_riot_call_function_safe(o->callback);
}

/* Runs in ISR context; defer the actual Python callback out of the interrupt
 * by scheduling it. Running the MicroPython VM directly from here would use the
 * tiny ISR stack and is not reentrancy-safe. */
static void _xtimer_isr(void *arg)
{
    mp_obj_xtimer_t *o = arg;

    mp_sched_schedule_node(&o->sched_node, _xtimer_sched_callback);
}

static mp_obj_t mp_xtimer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
                                   const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    if (!mp_obj_is_callable(args[0])) {
        mp_raise_TypeError("callback argument must be callable");
    }

    mp_obj_xtimer_t *o = m_new_obj(mp_obj_xtimer_t);
    o->base.type = type;
    memset(&o->t, '\0', sizeof(o->t));
    o->sched_node.callback = NULL;
    o->callback = args[0];
    o->t.callback = _xtimer_isr;
    o->t.arg = o;
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t mp_xtimer_set(mp_obj_t self_in, mp_obj_t val_in)
{
    mp_obj_xtimer_t *o = MP_OBJ_TO_PTR(self_in);
    mp_int_t val = mp_obj_get_int(val_in);

    if (val <= 0) {
        mp_raise_ValueError("xtimer.set(): offset must be positive");
    }

    xtimer_set(&o->t, val);

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_xtimer_set_obj,
                          mp_xtimer_set);

static const MP_DEFINE_STR_OBJ(xtimer_module_doc_obj,
                               "RIOT xtimer module - microsecond hardware timers");

static const MP_DEFINE_STR_OBJ(xtimer_type_doc_obj,
                               "xtimer(callback) - create a one-shot timer; "
                               "callback runs in thread context (scheduled)");

static const mp_rom_map_elem_t xtimer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&mp_xtimer_set_obj) },
    { MP_ROM_QSTR(MP_QSTR___doc__), MP_ROM_PTR(&xtimer_type_doc_obj) },
};

static MP_DEFINE_CONST_DICT(xtimer_locals_dict, xtimer_locals_dict_table);
static MP_DEFINE_CONST_OBJ_TYPE(
    xtimer_type,
    MP_QSTR_xtimer,
    MP_TYPE_FLAG_NONE,
    make_new, mp_xtimer_make_new,
    locals_dict, &xtimer_locals_dict);

static const mp_rom_map_elem_t mp_module_xtimer_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_xtimer) },
    { MP_ROM_QSTR(MP_QSTR___doc__), MP_ROM_PTR(&xtimer_module_doc_obj) },
    { MP_ROM_QSTR(MP_QSTR_xtimer), MP_ROM_PTR(&xtimer_type) },
};

static MP_DEFINE_CONST_DICT(mp_module_xtimer_globals, mp_module_xtimer_globals_table);

const mp_obj_module_t mp_module_xtimer = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_xtimer_globals,
};

MP_REGISTER_MODULE(MP_QSTR_xtimer, mp_module_xtimer);

#endif /* MICROPY_PY_XTIMER */
