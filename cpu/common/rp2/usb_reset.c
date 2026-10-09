/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_common_rp2
 * @{
 *
 * @file
 * @brief       Reset into the USB bootloader of the RP2040 boot ROM
 *
 * @author      Christopher Buechse
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "irq.h"
#include "usb_board_reset.h"

/* The ROM constants of the Pico SDK are used without the rest of the SDK */
#define NO_PICO_PLATFORM    1
#define PICO_RP2040         1
#define _u(x)               x ## u
#include "boot/bootrom_constants.h"

typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);
typedef void (*rom_reset_usb_boot_fn)(uint32_t gpio_activity_pin_mask,
                                      uint32_t disable_interface_mask);

static inline void *_rom_hword_as_ptr(uintptr_t addr)
{
    return (void *)(uintptr_t)*(volatile uint16_t *)addr;
}

void usb_board_reset_in_bootloader(void)
{
    rom_table_lookup_fn lookup = _rom_hword_as_ptr(BOOTROM_TABLE_LOOKUP_OFFSET);
    rom_reset_usb_boot_fn reset_usb_boot =
        lookup(_rom_hword_as_ptr(BOOTROM_FUNC_TABLE_OFFSET), ROM_FUNC_RESET_USB_BOOT);

    irq_disable();
    /* no activity LED, enable both the mass storage and the PICOBOOT
     * interface */
    reset_usb_boot(0, 0);
    while (1) {}
}
