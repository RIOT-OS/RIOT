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

void core1_main() {
    while (1) {
        xosc_sleep(500);
        LED0_TOGGLE;
    }
}

/* Table 37 FIFO_ST*/
#define SIO_FIFO_READ_VALID_BIT 0
#define core1_psm_bit 24
extern uint32_t _estack;  /* End of stack based on cortex_m.ld */

void core1_init() {
    /* First we need to get core1 out of reset (See 5.3)
     * for that we need to get it out of reset (See 7.4.4)
     * this allows proc1 to power on */
    atomic_set(&PSM->FRCE_ON, 1<<core1_psm_bit);
    /* Check whether PSM Done is set (See Table 533 / 7.4.4)*/
    while (~PSM->DONE & 1<<core1_psm_bit) {
        /* Wait for the reset to complete */
    }

    /* At this point Core1 is powered on but sleeping (See 5.2)*/

    /* Next we need to define the launching code by passing all relevant info
    * via the inter-processor FIFO (See 3.1.5).
    * While the definition of 5.3 isn't great since it relies on the SDK,
    * if we actually look at the SDK at pico_multicore/multicore.c
    * we can see that we simply have to pass a data struct
    * I still can't find the exact reason for the first 3 values though?
    */
   const uint32_t cmd_sequence[] = {
    0,
    0,
    1,
    /**
     * This might be a really bad way to do this but I'm a bit unsure how else
     * to tackle it, the vector_table uses macro magic and I'm unsure how to
     * get the address, however, the vector table should sit right at the front
     * of the ROM so *technically* it might be fine
     */
    (uintptr_t) ROM_START_ADDR,
    /**
     * The stack pointer position should begin right after the
     */
    (uintptr_t) (&_estack - 0x100),
    /** Pointer to main function for core1 */
    (uintptr_t) core1_main
    };

    uint32_t seq = 0;
    do {
        uint32_t cmd = cmd_sequence[seq];
        /* If the cmd is 0 we need to drain the READ FIFO first*/
        if(cmd == 0) {
            /**
             * The official SDK does this
             * ```c
             *     while (multicore_fifo_rvalid())
             *         (void) sio_hw->fifo_rd;
             * ```
             * fifo_rvalid checks whether rx fifo is empty and then the value
             * gets discarded
             */
            while(SIO->FIFO_ST & 1<<SIO_FIFO_READ_VALID_BIT) {
                (void) SIO->FIFO_RD; /* Table 39 FIFO_RD*/
            };

            /* SEV -> Set Event https://developer.arm.com/documentation/dui0473/m/arm-and-thumb-instructions/sev */
            __SEV();
        }

        /* @TODO: Send instruction */
    } while (seq < sizeof(cmd_sequence)/sizeof(cmd_sequence[0]));
}

void cpu_init(void) {
    /* initialize the Cortex-M core, once UART support is moved
     * to shared driver as currently this will cause unhandled interrupts */
    /* cortexm_init(); */

    /* Reset GPIO state */
    gpio_reset();

    /* Reset clock to default state */
    clock_reset();

    /* initialize the CPU clock */
    cpu_clock_init();

    /* initialize the early peripherals */
    early_init();

    /* trigger static peripheral initialization */
    periph_init();

    /* initialize the board */
    board_init();

    /* Init Core 1 */
    core1_init();
}

/** @} */
