/*
 * Copyright (C) 2023 Fabian Hüßler
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include "pio/pio.h" /* this is the internal PIO header of the rpx0xx */
#include "blink.pio.h" /* this is the auto generated header from blink.pio */

/**
 * @brief   Initialize the GPIO, which will be controlled by PIO
 *
 * Every PIO program should have a custom function to initialize GPIOs
 * as required for the use case. The first (0) mapped set-pin (GPIO @p pin)
 * is initially configured as low output pin.
 *
 * @param[in]       pio         PIO index of the PIO to use
 * @param[in]       sm          PIO state machine index of the SM to use
 * @param[in]       pin         GPIO pin to be mapped as first set-pin (base)
 */
static void pio_blink_init_pins(pio_t pio, pio_sm_t sm, gpio_t pin)
{
    pio_gpio_init_t init = {
        .gpio_direction = PIO_GPIO_INIT_OUT(0),
        .gpio_state = PIO_GPIO_INIT_LOW(0),
        .gpio_base = pin,
        .gpio_count = 1
    };
    pio_sm_set_set_pins_init(pio, sm, &init);
}

/**
 * @brief   Create an unmapped blinky PIO program
 *
 * A PIO program needs a kind of constructor (in OOP).
 *
 * @return  Blinky program
 */
pio_program_t pio_blink_export_program(void)
{
    /* Call auto-generated function to retrieve a PIO program handle.
       The program is not loaded yet. */
    return pio_blink_create_program();
}

/**
 * @brief   Write blink program to instruction memory
 *
 * A PIO program needs a function to write the instructions
 * to the allocated program memory.
 *
 * @param[in]       pio         PIO index
 * @param[in]       pro         Allocated program
 *
 * @return 0 on success or negative number on error
 */
int pio_blink_write_program(pio_t pio, pio_program_t *pro)
{
    pio_instr_t instr[] = PIO_BLINK_PROGRAM;
    int ret;
    if ((ret = pio_write_program(pio, pro, instr))) {
        return ret;
    }
    return 0;
}

/**
 * @brief   Initialize and start running a blinky PIO program from an
 *          allocated program @p pro and PIO @p pio and locked state machine @p sm
 *
 * A PIO program needs a function to initialize a PIO state machine to run a program.
 *
 * @param[in]       pio         PIO index
 * @param[in]       sm          PIO state machine index
 * @param[in]       pro         Allocated program
 * @param[in]       pin         GPIO pin to use for blinking
 *
 * @return  0 on successful program start or negative number on error
 */
int pio_blink_init(pio_t pio, pio_sm_t sm, const pio_program_t *pro,
                   gpio_t pin)
{
    pio_program_conf_t conf = PIO_BLINK_PROGRAM_CONF;
    int ret;
    if (pin >= 32) {
        return -ENOTSUP;
    }
    if ((ret = pio_sm_init_common(pio, sm, pro, &conf))) {
        return ret;
    }
    pio_sm_set_clkdiv(pio, sm, pio_sm_clkdiv(2000));
    pio_blink_init_pins(pio, sm, pin);
    pio_sm_restart(pio, sm);
    return 0;
}
