/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython machine module type implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "periph/pm.h"
#include "periph/cpuid.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "modmachine.h"

#if MICROPY_PY_MACHINE

static mp_obj_t machine_reset(void)
{
    pm_reboot();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_obj, machine_reset);

static mp_obj_t machine_reset_cause(void)
{
    printf("Warning: %s is not implemented\n", __func__);
    return MP_OBJ_NEW_SMALL_INT(42);
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_reset_cause_obj, machine_reset_cause);

#ifdef MODULE_PERIPH_CPUID
static mp_obj_t machine_unique_id(void)
{
    uint8_t chipid[CPUID_LEN];
    cpuid_get(chipid);
    return mp_obj_new_bytes(chipid, CPUID_LEN);
}
static MP_DEFINE_CONST_FUN_OBJ_0(machine_unique_id_obj, machine_unique_id);
#endif

static const mp_rom_map_elem_t machine_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_umachine) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&machine_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset_cause), MP_ROM_PTR(&machine_reset_cause_obj) },
#ifdef MODULE_PERIPH_CPUID
    { MP_ROM_QSTR(MP_QSTR_unique_id), MP_ROM_PTR(&machine_unique_id_obj) },
#endif

    { MP_ROM_QSTR(MP_QSTR_Pin), MP_ROM_PTR(&machine_pin_type) },
#if MICROPY_PY_MACHINE_SPI
    { MP_ROM_QSTR(MP_QSTR_SPI), MP_ROM_PTR(&machine_hw_spi_type) },
#endif
#if MICROPY_PY_MACHINE_ADC
    { MP_ROM_QSTR(MP_QSTR_ADC), MP_ROM_PTR(&machine_adc_type) },
#endif

    /* reset causes */
    /*{ MP_ROM_QSTR(MP_QSTR_PWRON_RESET), MP_ROM_INT(REASON_DEFAULT_RST) },*/
};

static MP_DEFINE_CONST_DICT(machine_module_globals, machine_module_globals_table);

const mp_obj_module_t mp_module_machine = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&machine_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_machine, mp_module_machine);

#endif /* MICROPY_PY_MACHINE */
