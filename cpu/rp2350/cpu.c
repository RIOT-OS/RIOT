/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           CPU initialization implementation for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "cpu.h"

#include "RP2350.h"
#include "board.h"
#include "kernel_init.h"
#include "macros/units.h"
#include "periph/gpio.h"
#include "periph/init.h"
#include "periph_cpu.h"
#include "stdio_base.h"

#define DEBUG_WITH_OSC

void gpio_reset(void) {
  reset_component(RESET_PADS_BANK0, RESET_PADS_BANK0);
  reset_component(RESET_IO_BANK0, RESET_IO_BANK0);
}

#define GPIO_FUNC_SIO 5
// Get it, Pin + Init, hahahaha
void pinit(void) {
    // Set LED (25) and Pin 15 so we can debug with them
    IO_BANK0->GPIO15_CTRL = GPIO_FUNC_SIO;
    IO_BANK0->GPIO25_CTRL = GPIO_FUNC_SIO;

    // Clear the ISO bits for GPIO15 and GPIO25
    // Otherwise the GPIOs will not work
    atomic_clear(&PADS_BANK0->GPIO15, PADS_BANK0_ISO_BITS);
    atomic_clear(&PADS_BANK0->GPIO15, PADS_BANK0_GPIO0_IE_BITS);
    atomic_clear(&PADS_BANK0->GPIO25, PADS_BANK0_ISO_BITS);

    // Set the GPIO function for GPIO15 and GPIO25
    // GPIO25 is used for the LED
    // GPIO15 is used for debugging via Oscilloscope
    SIO->GPIO_OE_SET = 1<<15 | 1<<25;
    SIO->GPIO_OUT = 1<<15 | 1<<25;
    SIO->GPIO_OUT_CLR = 1UL << 25;
}

void cpu_init(void) {
  /* initialize the Cortex-M core, once UART support is moved
   * to shared driver as currently this will cause unhandled interrupts */
  /* cortexm_init(); */

  gpio_reset();
  pinit();

  clock_reset();

  cpu_clock_init();

  early_init();

  /* trigger static peripheral initialization */
  periph_init();
}

/** @} */
