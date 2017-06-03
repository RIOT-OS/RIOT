/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_llwu Kinetis LLWU
 * @ingroup     cpu_kinetis
 * @brief       Kinetis low leakage wakeup unit (LLWU) driver

 * @{
 *
 * @file
 * @brief       Interface definition for the Kinetis LLWU driver.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef LLWU_H
#define LLWU_H

#include "cpu.h"
#include "bit.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initialize the Low-Leakage Wake Up (LLWU) hardware module
 */
void llwu_init(void);

/**
 * @brief Enable a wakeup module in the LLWU
 */
inline static void llwu_wakeup_module_enable(llwu_wakeup_module_t mod)
{
    assert(mod < LLWU_WAKEUP_MODULE_NUMOF);
    bit_set8(&LLWU->ME, mod);
}

/**
 * @brief Disable a wakeup module in the LLWU
 */
inline static void llwu_wakeup_module_disable(llwu_wakeup_module_t mod)
{
    assert(mod < LLWU_WAKEUP_MODULE_NUMOF);
    bit_clear8(&LLWU->ME, mod);
}

/**
 * @brief Set the mode for a wakeup pin in the LLWU
 *
 * If @p cb is NULL when the pin edge is detected, the CPU will wake up for a
 * few cycles which can allow other hardware modules to detect other interrupts.
 * This may be particularily useful when using the wakeup pin for communication
 * functions such as UART RX etc.
 *
 * @param[in]  pin  The pin to modify
 * @param[in]  edge Edge detection setting (rising, falling, both, none)
 * @param[in]  cb   Callback function to execute when woken with this pin
 * @param[in]  arg  Argument that will be passed to the callback
 */
void llwu_wakeup_pin_set(llwu_wakeup_pin_t pin, llwu_wakeup_edge_t edge, gpio_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* LLWU_H */
