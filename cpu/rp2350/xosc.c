/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <assert.h>

#include "RP2350.h"
#include "board.h"
#include "macros/units.h"
#include "periph_cpu.h"

// Based on datasheet 8.2.4 (1ms wait time)
#define STARTUP_DELAY 47
#define MAX_XOSC_COUNTER_SIZE 0xFFFF
#define Sleep100HzSpeed 60000
#define CYCLES_PER_MS (Sleep100HzSpeed / 1000)

void xosc_start(void) {
  // Set the FREQ_RANGE
  XOSC->CTRL = XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ;
  // Set the startup delay (default 1ms)
  XOSC->STARTUP = STARTUP_DELAY;
  // set enable bit
  atomic_set(&XOSC->CTRL,
             XOSC_CTRL_ENABLE_VALUE_ENABLE << XOSC_CTRL_ENABLE_LSB);

  while (!(XOSC->STATUS & XOSC_STATUS_STABLE_BITS)) {
    // Wait for the crystal to stabilize
  }
}

void xosc_sleep(int32_t milliseconds) {
  for (int32_t i = 0; i < milliseconds; i++) {
    XOSC->COUNT = CYCLES_PER_MS * milliseconds;
    while (XOSC->COUNT != 0) {
    }
  }
}

void xosc_stop(void) {
  // @TODO
}
