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

void cpu_init(void) {
  /* initialize the Cortex-M core */
  //cortexm_init();
  gpio_reset();
  //_cpu_reset();

  /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
  early_init();

  clock_reset();
  cpu_clock_init();

  /* trigger static peripheral initialization */
  periph_init();
}
