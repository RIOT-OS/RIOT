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

  /* Reenable the LED0 pin
   * Otherwise the LED will not work after a reset
   * This is needed, esp. when the LED is used via
   * the define macros */
  gpio_init(LED0_PIN_ID, GPIO_OUT);
}

void cpu_init(void) {
  /* initialize the Cortex-M core, once UART support is moved
   * to shared driver as currently this will cause unhandled interrupts */
  /* cortexm_init(); */

  gpio_reset();

  clock_reset();

  cpu_clock_init();

  early_init();

  /* trigger static peripheral initialization */
  periph_init();
}

/** @} */
