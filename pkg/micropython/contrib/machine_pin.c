/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2019 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython machine pin implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "periph/gpio.h"

#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "modmachine.h"

#define MP_PIN_PORT_A  0
#define MP_PIN_PORT_B  1
#define MP_PIN_PORT_C  2
#define MP_PIN_PORT_D  3
#define MP_PIN_PORT_E  4
#define MP_PIN_PORT_F  5
#define MP_PIN_PORT_G  6
#define MP_PIN_PORT_H  7
#define MP_PIN_PORT_I  8
#define MP_PIN_PORT_J  9
#define MP_PIN_PORT_K  10

const mp_obj_base_t machine_pin_obj_template = {&machine_pin_type};

static void machine_pin_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    machine_pin_obj_t *self = self_in;
    mp_printf(print, "<Pin %u>", (unsigned)self->pin);
}

/* pin.init(mode, *, value) */
static mp_obj_t machine_pin_obj_init_helper(machine_pin_obj_t *self, size_t n_args,
                                            const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_mode, ARG_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT, { .u_int = GPIO_IN } },
        { MP_QSTR_value, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none}},
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    /*get io mode */
    uint mode = args[ARG_mode].u_int;

    int ret = gpio_init(self->pin, mode);
    if (ret) {
        mp_raise_ValueError("invalid pin");
    }

    /* get initial value */
    if (args[ARG_value].u_obj != MP_OBJ_NULL) {
        (void)gpio_write(self->pin, mp_obj_is_true(args[ARG_value].u_obj));
    }

    return mp_const_none;
}

mp_obj_t mp_pin_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw,
                         const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    mp_obj_t *elem;
    mp_obj_get_array_fixed_n(args[0], 2, &elem);

    gpio_t wanted_pin = GPIO_PIN(mp_obj_get_int(elem[0]),
                                 mp_obj_get_int(elem[1]));

    machine_pin_obj_t *pin = m_new_obj(machine_pin_obj_t);
    pin->base = machine_pin_obj_template;
    pin->pin = wanted_pin;

    if (n_args > 1 || n_kw > 0) {
        /* pin mode given, so configure this GPIO */
        mp_map_t kw_args;
        mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
        machine_pin_obj_init_helper(pin, n_args - 1, args + 1, &kw_args);
    }

    return (mp_obj_t)pin;
}

/* fast method for getting/setting pin value */
static mp_obj_t machine_pin_call(mp_obj_t self_in, size_t n_args, size_t n_kw,
                                 const mp_obj_t *args)
{
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    machine_pin_obj_t *self = self_in;
    if (n_args == 0) {
        unsigned pin_val = gpio_read(self->pin);
        return MP_OBJ_NEW_SMALL_INT(pin_val);
    } else {
        gpio_write(self->pin, mp_obj_is_true(args[0]));
        return mp_const_none;
    }
}

/* pin.init(mode, pull) */
static mp_obj_t machine_pin_obj_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args)
{
    return machine_pin_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_pin_init_obj, 1, machine_pin_obj_init);

/* pin.value([value]) */
static mp_obj_t machine_pin_value(size_t n_args, const mp_obj_t *args)
{
    return machine_pin_call(args[0], n_args - 1, 0, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_pin_value_obj, 1, 2, machine_pin_value);

static mp_obj_t machine_pin_off(mp_obj_t self_in)
{
    machine_pin_obj_t *self = self_in;
    (void)gpio_write(self->pin, 0);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_off_obj, machine_pin_off);

static mp_obj_t machine_pin_on(mp_obj_t self_in)
{
    machine_pin_obj_t *self = self_in;
    (void)gpio_write(self->pin, 1);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_pin_on_obj, machine_pin_on);

static mp_uint_t machine_pin_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg,
                                   int *errcode)
{
    (void)errcode;
    machine_pin_obj_t *self = self_in;

    switch (request) {
    case MP_PIN_READ:
        return gpio_read(self->pin);
    case MP_PIN_WRITE:
        gpio_write(self->pin, arg);
        return 0;
    }

    return -1;
}

static const mp_rom_map_elem_t machine_pin_locals_dict_table[] = {
    /* instance methods */
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_pin_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),   MP_ROM_PTR(&machine_pin_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_off),     MP_ROM_PTR(&machine_pin_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_on),      MP_ROM_PTR(&machine_pin_on_obj) },

    /* class constants */
    { MP_ROM_QSTR(MP_QSTR_IN),      MP_ROM_INT(GPIO_IN) },
    { MP_ROM_QSTR(MP_QSTR_IN_PD),   MP_ROM_INT(GPIO_IN_PD) },
    { MP_ROM_QSTR(MP_QSTR_IN_PU),   MP_ROM_INT(GPIO_IN_PU) },
    { MP_ROM_QSTR(MP_QSTR_OUT),     MP_ROM_INT(GPIO_OUT) },
    { MP_ROM_QSTR(MP_QSTR_PORT_A),  MP_ROM_INT(MP_PIN_PORT_A) },
    { MP_ROM_QSTR(MP_QSTR_PORT_B),  MP_ROM_INT(MP_PIN_PORT_B) },
    { MP_ROM_QSTR(MP_QSTR_PORT_C),  MP_ROM_INT(MP_PIN_PORT_C) },
    { MP_ROM_QSTR(MP_QSTR_PORT_D),  MP_ROM_INT(MP_PIN_PORT_D) },
    { MP_ROM_QSTR(MP_QSTR_PORT_E),  MP_ROM_INT(MP_PIN_PORT_E) },
    { MP_ROM_QSTR(MP_QSTR_PORT_F),  MP_ROM_INT(MP_PIN_PORT_F) },
    { MP_ROM_QSTR(MP_QSTR_PORT_G),  MP_ROM_INT(MP_PIN_PORT_G) },
    { MP_ROM_QSTR(MP_QSTR_PORT_H),  MP_ROM_INT(MP_PIN_PORT_H) },
    { MP_ROM_QSTR(MP_QSTR_PORT_I),  MP_ROM_INT(MP_PIN_PORT_I) },
    { MP_ROM_QSTR(MP_QSTR_PORT_J),  MP_ROM_INT(MP_PIN_PORT_J) },
    { MP_ROM_QSTR(MP_QSTR_PORT_K),  MP_ROM_INT(MP_PIN_PORT_K) },
};

static MP_DEFINE_CONST_DICT(machine_pin_locals_dict, machine_pin_locals_dict_table);

static const mp_pin_p_t machine_pin_pin_p = {
    .ioctl = machine_pin_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_pin_type,
    MP_QSTR_Pin,
    MP_TYPE_FLAG_NONE,
    print, machine_pin_print,
    make_new, mp_pin_make_new,
    call, machine_pin_call,
    protocol, &machine_pin_pin_p,
    locals_dict, &machine_pin_locals_dict);
