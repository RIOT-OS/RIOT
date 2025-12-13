/*
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke <nandojve@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       External Bus Interface test application for ATxmega A1x
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#if defined(MODULE_ATXMEGA_EBI)

#include "buildinfo/board.h"
#include "board.h"
#include "cpu_ebi.h"
#include "errno.h"
#include "cpu_tests.h"

#if   defined(BOARD_ATXMEGA_A1U_XPRO)
/**
 * 512k bytes installed
 * 256k bytes are truly addressable because of board layout
 * 64k  bytes reserved for RIOT-OS
 * 192k bytes are addressable by hugemem methods.
 *      Free for user
 *      This is tested here
 */
#define BOARD_EBI_RAM_BASE      (0x10000)
#define BOARD_EBI_RAM_SIZE      (0x30000)
#elif defined(BOARD_ATXMEGA_A1_XPLAINED)
/**
 * 8MB  bytes installed
 * 64k  bytes reserved for RIOT-OS
 *
 * The remaining memory can be addressable by hugemem methods.
 * It is free for user and it is tested here
 */
#define BOARD_EBI_RAM_BASE      (0x10000)
#define BOARD_EBI_RAM_SIZE      (0x7F0000)
#else
/**
 * 128k bytes installed
 * 64k  bytes reserved for RIOT-OS
 * 64k  bytes free for user and it is tested here
 */
#define BOARD_EBI_RAM_BASE      (0x10000)
#define BOARD_EBI_RAM_SIZE      (0x10000)
#endif

/**
 * @brief Test the EBI data bus wired to the SRAM
 *
 * This function will perform a walking 1s to locate any shorts or open leads
 * to the SRAM device.
 *
 * @param base Test Area base address of the external memory device
 *
 * @retval 0 on success, and @ref errno on failure
 */
static int8_t ebi_test_data_bus(hugemem_ptr_t base)
{
    hugemem_ptr_t   p;
    uint8_t         i;

    /* Write walking 1s */
    for (p = base, i = 0; i < 32; i++) {
        hugemem_write32(p, 1UL << i);
        p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
    }

    /* Read walking 1s, write walking 0s */
    for (p = base, i = 0; i < 32; i++) {
        uint32_t        expected = 1UL << i;
        uint32_t        actual;

        actual = hugemem_read32(p);
        if (actual != expected) {
            return -EIO;
        }

        hugemem_write32(p, ~expected);
        p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
    }

    /* Read walking 0s */
    for (p = base, i = 0; i < 32; i++) {
        uint32_t        actual;
        uint32_t        expected = ~(1UL << i);

        actual = hugemem_read32(p);
        if (actual != expected) {
            return -EIO;
        }

        p = (hugemem_ptr_t)((uint32_t)p + sizeof(uint32_t));
    }

    return 0;
}

/**
 * @brief Test the EBI address bus wired to the SRAM
 *
 * This function will perform an address bus test to locate any shorts or open
 * leads to the SRAM device.
 *
 * @param base Test Area base address of the external memory device
 * @param size Size of the external memory device
 *
 * @retval 0 on success, and @ref errno on failure
 */
static int8_t ebi_test_addr_bus(hugemem_ptr_t base, uint32_t size)
{
    uint32_t        offset;
    uint8_t         i;

    /* Initialize all power-of-two locations with 0x55 */
    hugemem_write8(base, 0x55);
    for (offset = 1; offset < size; offset <<= 1) {
        hugemem_ptr_t   p;

        p = (hugemem_ptr_t)((uint32_t)base + offset);
        hugemem_write8(p, 0x55);
    }

    /* Check for address lines stuck high */
    hugemem_write8(base, 0xaa);
    for (i = 0, offset = 1; offset < size; offset <<= 1, i++) {
        hugemem_ptr_t   p;
        uint8_t         actual;

        p = (hugemem_ptr_t)((uint32_t)base + offset);
        actual = hugemem_read8(p);
        if (actual != 0x55) {
            return -EIO;
        }
    }

    /* Check for address lines stuck low or shorted */
    hugemem_write8(base, 0x55);
    for (i = 0, offset = 1; offset < size; offset <<= 1, i++) {
        hugemem_ptr_t   p;
        uint32_t        offset2;
        uint8_t         j;
        uint8_t         actual;

        p = (hugemem_ptr_t)((uint32_t)base + offset);
        hugemem_write8(p, 0xaa);

        actual = hugemem_read8(base);
        if (actual != 0x55) {
            return -EIO;
        }

        for (j = 0, offset2 = 1; offset2 < size; offset2 <<= 1, j++) {
            hugemem_ptr_t   q;

            if (offset2 == offset) {
                continue;
            }

            q = (hugemem_ptr_t)((uint32_t)base + offset2);
            actual = hugemem_read8(q);
            if (actual != 0x55) {
                return -EIO;
            }
        }

        hugemem_write8(p, 0x55);
    }

    return 0;
}

/**
 * @brief Perform a SRAM data integrity test
 *
 * This function will perform a SRAM data integrity test by writing 0s and 1s
 * to the entire external device.
 *
 * @param base Test Area base address of the external memory device
 * @param size Size of the external memory device
 *
 * @retval 0 on success, and @ref errno on failure
 */
static int8_t ebi_test_data_integrity(hugemem_ptr_t base, uint32_t size)
{
    uint32_t        offset;
    uint32_t        pattern;

    /* Fill memory with a known pattern. */
    for (pattern = 1, offset = 0; offset < size; pattern++,
        offset += sizeof(uint32_t)) {
        hugemem_ptr_t   p;

        p = (hugemem_ptr_t)((uint32_t)base + offset);
        hugemem_write32(p, pattern);
    }

    /* Check each location and invert it for the second pass. */
    for (pattern = 1, offset = 0; offset < size; pattern++,
        offset += sizeof(uint32_t)) {
        hugemem_ptr_t   p;
        uint32_t        actual;
        uint32_t        expected;

        p = (hugemem_ptr_t)((uint32_t)base + offset);

        actual = hugemem_read32(p);
        if (actual != pattern) {
            return -EIO;
        }

        expected = ~pattern;
        hugemem_write32(p, expected);
    }

    /* Check each location for the inverted pattern and zero it. */
    for (pattern = 1, offset = 0; offset < size; pattern++,
        offset += sizeof(uint32_t)) {
        hugemem_ptr_t   p;
        uint32_t        actual;
        uint32_t        expected;

        p = (hugemem_ptr_t)((uint32_t)base + offset);

        expected = ~pattern;
        actual = hugemem_read32(p);
        if (actual != expected) {
            return -EIO;
        }
    }

    return 0;
}

void ebi_tests(void)
{
    uint8_t ret;

    puts("bus connectivity");
    ret = ebi_test_data_bus((hugemem_ptr_t)BOARD_EBI_RAM_BASE);
    if (ret) {
        goto out;
    }

    puts("addrresing");
    ret = ebi_test_addr_bus((hugemem_ptr_t)BOARD_EBI_RAM_BASE,
    BOARD_EBI_RAM_SIZE);
    if (ret) {
        goto out;
    }

    puts("data integrity");
    ret = ebi_test_data_integrity((hugemem_ptr_t)BOARD_EBI_RAM_BASE,
    BOARD_EBI_RAM_SIZE);
    if (ret) {
        goto out;
    }

out:
    if (ret) {
        puts("TEST FAILED");
    }
    else {
        puts("TEST SUCCEEDED");
    }
}

#endif /* MODULE_ATXMEGA_EBI */
