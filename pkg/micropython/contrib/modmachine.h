/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython machine module type declarations
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "py/obj.h"
#include "periph/gpio.h"

extern const mp_obj_type_t machine_pin_type;
extern const mp_obj_type_t machine_hw_spi_type;
extern const mp_obj_type_t machine_adc_type;

MP_DECLARE_CONST_FUN_OBJ_0(machine_info_obj);

typedef struct _machine_pin_obj_t {
    mp_obj_base_t base;
    gpio_t pin;
} machine_pin_obj_t;
