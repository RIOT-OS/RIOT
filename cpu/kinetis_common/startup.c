/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common
 * @{
 *
 * @file
 * @brief       Startup code for Freescale Kinetis MCUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "cpu_conf.h"
#include "wdog.h"

/**
 * @name Memory markers as defined in the linker script
 * @{
 */
extern uint32_t _sfixed[];
extern uint32_t _efixed[];
extern uint32_t _etext[];
extern uint32_t _srelocate[];
extern uint32_t _erelocate[];
extern uint32_t _szero[];
extern uint32_t _ezero[];
extern uint32_t _sstack[];
extern uint32_t _ramcode_start[];
extern uint32_t _ramcode_end[];
extern uint32_t _ramcode_load[];
extern uint32_t _vector_ram_start[];
extern uint32_t _vector_ram_end[];
extern uint32_t _vector_rom[];
/** @} */

/**
 * @brief functions for initializing the board, std-lib and kernel
 */
extern void board_init(void);
extern void kernel_init(void);
extern void __libc_init_array(void);

/**
 * @brief This function is the entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 0. disable the Watchdog Timer
 * 1. load data section from flash to ram
 * 2. overwrite uninitialized data section (BSS) with zeros
 * 3. initialize the newlib
 * 4. initialize the board (sync clock, setup std-IO)
 * 5. initialize and start RIOTs kernel
 */
void reset_handler(void)
{
    uint32_t *dst;
    uint32_t *src = _etext;

    /* disable the WDOG */
    wdog_disable();

    /* load .data section from flash to ram */
    for (dst = _srelocate; dst < _erelocate;) {
        *(dst++) = *(src++);
    }

    /* default .bss section to zero */
    for (dst = _szero; dst < _ezero;) {
        *(dst++) = 0;
    }

    /* copy .ramcode from flash to RAM */
    src = _ramcode_load;
    for (dst = _ramcode_start; dst < _ramcode_end;) {
        *(dst++) = *(src++);
    }

    /*
     * Copy ISR vector from flash to RAM.
     *
     * To use this CPU feature, define RAMVECT_SIZE=0x400 when building and write
     * the new vector table address in RAM to SCB->VTOR.
     */
    src = _vector_rom;
    for (dst = _vector_ram_start; dst < _vector_ram_end;) {
        *(dst++) = *(src++);
    }

    /* initialize the board and startup the kernel */
    board_init();
    /* initialize std-c library (this should be done after board_init) */
    __libc_init_array();
    /* startup the kernel */
    kernel_init();
}

/* fcfield table */
__attribute__((weak, section(".fcfield")))
const uint8_t flash_configuration_field[] = {
    0xff,    /* backdoor comparison key 3., offset: 0x0 */
    0xff,    /* backdoor comparison key 2., offset: 0x1 */
    0xff,    /* backdoor comparison key 1., offset: 0x2 */
    0xff,    /* backdoor comparison key 0., offset: 0x3 */
    0xff,    /* backdoor comparison key 7., offset: 0x4 */
    0xff,    /* backdoor comparison key 6., offset: 0x5 */
    0xff,    /* backdoor comparison key 5., offset: 0x6 */
    0xff,    /* backdoor comparison key 4., offset: 0x7 */
    0xff,    /* non-volatile p-flash protection 1 - low register, offset: 0x8 */
    0xff,    /* non-volatile p-flash protection 1 - high register, offset: 0x9 */
    0xff,    /* non-volatile p-flash protection 0 - low register, offset: 0xa */
    0xff,    /* non-volatile p-flash protection 0 - high register, offset: 0xb */
    0xfe,    /* non-volatile flash security register, offset: 0xc */
    0xff,    /* non-volatile flash option register, offset: 0xd */
    0xff,    /* non-volatile eeram protection register, offset: 0xe */
    0xff,    /* non-volatile d-flash protection register, offset: 0xf */
};
