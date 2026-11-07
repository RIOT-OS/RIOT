/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the EFM32 CPU information database.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "em_device.h"

/**
 * @brief   Start of the RAM, as defined by the linker script.
 */
extern uint8_t _sram;

/**
 * @brief   End of the RAM, as defined by the linker script.
 */
extern uint8_t _eram;

/**
 * @brief   Start of the ROM, as defined by the linker script.
 */
extern uint8_t _rom_start_addr;

/**
 * @brief   Length of the ROM, as defined by the linker script.
 */
extern uint8_t _rom_length;

/**
 * @name    Peripherals as reported by the vendor headers
 *
 * @{
 */
#if defined(_SILICON_LABS_32B_SERIES_2)
#  if defined(SEMAILBOX_COUNT) && (SEMAILBOX_COUNT > 0)
#    define VENDOR_CRYPTO   1
#    define VENDOR_TRNG     1
#  endif
#else
#  if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
#    define VENDOR_CRYPTO   1
#  endif
#  if defined(TRNG_COUNT) && (TRNG_COUNT > 0)
#    define VENDOR_TRNG     1
#  endif
#endif

#if defined(_SILICON_LABS_EFR32_RADIO_TYPE)
#  define VENDOR_RADIO      1
#endif

#ifndef VENDOR_CRYPTO
#  define VENDOR_CRYPTO     0
#endif
#ifndef VENDOR_TRNG
#  define VENDOR_TRNG       0
#endif
#ifndef VENDOR_RADIO
#  define VENDOR_RADIO      0
#endif
/** @} */

#if TEST_EFM32_SERIES != _SILICON_LABS_32B_SERIES
#  error "EFM32_SERIES does not match _SILICON_LABS_32B_SERIES."
#endif

#if TEST_EFM32_FLASH_START != FLASH_BASE
#  error "EFM32_FLASH_START does not match FLASH_BASE."
#endif

#if TEST_EFM32_FLASH_SIZE != FLASH_SIZE
#  error "EFM32_FLASH_SIZE does not match FLASH_SIZE."
#endif

#if TEST_EFM32_SRAM_START != SRAM_BASE
#  error "EFM32_SRAM_START does not match SRAM_BASE."
#endif

#if TEST_EFM32_SRAM_SIZE != SRAM_SIZE
#  error "EFM32_SRAM_SIZE does not match SRAM_SIZE."
#endif

#if TEST_EFM32_CRYPTO != VENDOR_CRYPTO
#  error "EFM32_CRYPTO does not match the vendor headers."
#endif

#if TEST_EFM32_TRNG != VENDOR_TRNG
#  error "EFM32_TRNG does not match the vendor headers."
#endif

#if TEST_EFM32_RADIO != VENDOR_RADIO
#  error "EFM32_RADIO does not match the vendor headers."
#endif

static int _test_flash(void)
{
    puts("Testing the flash layout.");

    /* the linker script derives its ROM region from the database */
    volatile uintptr_t start = (uintptr_t)&_rom_start_addr;
    volatile uintptr_t length = (uintptr_t)&_rom_length;

    printf("Flash: 0x%08" PRIxPTR " - 0x%08" PRIxPTR "\n", start,
           start + length);

    if (start != (uintptr_t)TEST_EFM32_FLASH_START) {
        puts("Linker start of ROM does not match EFM32_FLASH_START.");
        return 1;
    }

    if (length != (uintptr_t)TEST_EFM32_FLASH_SIZE) {
        puts("Linker length of ROM does not match EFM32_FLASH_SIZE.");
        return 1;
    }

    return 0;
}

static int _test_sram(void)
{
    puts("Testing the SRAM layout.");

    /* the linker script derives its RAM region from the database */
    volatile uintptr_t start = (uintptr_t)&_sram;
    volatile uintptr_t end = (uintptr_t)&_eram;

    printf("SRAM: 0x%08" PRIxPTR " - 0x%08" PRIxPTR "\n", start, end);

    if (start != (uintptr_t)TEST_EFM32_SRAM_START) {
        puts("Linker start of RAM does not match EFM32_SRAM_START.");
        return 1;
    }

    if ((end - start) != (uintptr_t)TEST_EFM32_SRAM_SIZE) {
        puts("Linker length of RAM does not match EFM32_SRAM_SIZE.");
        return 1;
    }

    return 0;
}

int main(void)
{
    int failures = 0;

    printf("Series: %d\n", TEST_EFM32_SERIES);
    printf("Architecture: %s\n", TEST_EFM32_ARCHITECTURE);

    printf("Crypto: %s\n", TEST_EFM32_CRYPTO ? "y" : "n");
    printf("TRNG: %s\n", TEST_EFM32_TRNG ? "y" : "n");
    printf("Radio: %s\n", TEST_EFM32_RADIO ? "y" : "n");

    failures += _test_flash();
    failures += _test_sram();

    if (failures != 0) {
        printf("%d test(s) failed.\n", failures);

        puts("[FAILURE]");
    }
    else {
        puts("[SUCCESS]");
    }

    return 0;
}
