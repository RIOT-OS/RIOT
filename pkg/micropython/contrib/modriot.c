/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython RIOT module bindings
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "py/mpconfig.h"

#if MICROPY_PY_RIOT

#include "py/runtime.h"

#include "thread.h"

static mp_obj_t mod_thread_getpid(void) {
    return MP_OBJ_NEW_SMALL_INT(thread_getpid());
}
static MP_DEFINE_CONST_FUN_OBJ_0(mod_thread_getpid_obj, mod_thread_getpid);

static const mp_rom_map_elem_t mp_module_riot_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_riot) },
    { MP_ROM_QSTR(MP_QSTR_thread_getpid), MP_ROM_PTR(&mod_thread_getpid_obj) },
};

static MP_DEFINE_CONST_DICT(mp_module_riot_globals, mp_module_riot_globals_table);

const mp_obj_module_t mp_module_riot = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_riot_globals,
};

MP_REGISTER_MODULE(MP_QSTR_riot, mp_module_riot);

#endif /* MICROPY_PY_RIOT */
