/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_common_rp2
 * @{
 *
 * @file
 * @brief       Reset into the USB bootloader of the RP2040 and RP2350 boot ROM
 *
 * @author      Christopher Buechse
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "cpu.h"
#include "irq.h"
#include "usb_board_reset.h"

/* The ROM constants of the Pico SDK are used without the rest of the SDK */
#define NO_PICO_PLATFORM    1
#ifdef CPU_FAM_RP2350
#  define PICO_RP2040       0
#else
#  define PICO_RP2040       1
#endif
#ifndef _u
#  define _u(x)             x ## u
#endif
#include "boot/bootrom_constants.h"
#if !PICO_RP2040
#  include "boot/picoboot_constants.h"
#endif

static inline void *_rom_hword_as_ptr(uintptr_t addr)
{
    return (void *)(uintptr_t)*(volatile uint16_t *)addr;
}

#if PICO_RP2040
typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);
typedef void (*rom_reset_usb_boot_fn)(uint32_t gpio_activity_pin_mask,
                                      uint32_t disable_interface_mask);

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
#else
typedef void *(*rom_table_lookup_fn)(uint32_t code, uint32_t mask);
typedef int (*rom_reboot_fn)(uint32_t flags, uint32_t delay_ms,
                             uint32_t p0, uint32_t p1);

#ifdef RP2350_USE_RISCV
/* on RISC-V, the lookup function is called via its entry in the table */
#  define ROM_TABLE_LOOKUP  BOOTROM_TABLE_LOOKUP_ENTRY_OFFSET
#  define ROM_FUNC_FLAGS    RT_FLAG_FUNC_RISCV
#else
/* RIOT runs in the secure state on the Arm cores */
#  define ROM_TABLE_LOOKUP  BOOTROM_TABLE_LOOKUP_OFFSET
#  define ROM_FUNC_FLAGS    RT_FLAG_FUNC_ARM_SEC
#endif

void usb_board_reset_in_bootloader(void)
{
    rom_table_lookup_fn lookup = _rom_hword_as_ptr(ROM_TABLE_LOOKUP);
    rom_reboot_fn reboot = lookup(ROM_FUNC_REBOOT, ROM_FUNC_FLAGS);

    irq_disable();
    /* reboot into BOOTSEL mode after 10 ms, no activity LED, enable both
     * the mass storage and the PICOBOOT interface */
    reboot(REBOOT2_FLAG_REBOOT_TYPE_BOOTSEL | REBOOT2_FLAG_NO_RETURN_ON_SUCCESS,
           10, 0, 0);
    while (1) {}
}
#endif
