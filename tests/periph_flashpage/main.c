/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Manual test application for flashpage peripheral drivers
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "od.h"
#include "shell.h"
#include "periph/flashpage.h"
#include "unaligned.h"

#define LINE_LEN            (16)

/* For MSP430 cpu's the last page holds the interrupt vector, although the api
   should not limit erasing that page, we don't want to break when testing */
#if defined(CPU_CC430) || defined(CPU_MSP430FXYZ)
#define TEST_LAST_AVAILABLE_PAGE    (FLASHPAGE_NUMOF - 2)
#else
#define TEST_LAST_AVAILABLE_PAGE    (FLASHPAGE_NUMOF - 1)
#endif

/* When writing raw bytes on flash, data must be correctly aligned. */
#define ALIGNMENT_ATTR __attribute__((aligned(FLASHPAGE_WRITE_BLOCK_ALIGNMENT)))

/* We must not write chunks smaller than FLASHPAGE_WRITE_BLOCK_SIZE */
#if FLASHPAGE_WRITE_BLOCK_SIZE > 64
#define RAW_BUF_SIZE FLASHPAGE_WRITE_BLOCK_SIZE
#else
#define RAW_BUF_SIZE 64
#endif

/*
 * @brief   Allocate an aligned buffer for raw writings
 */
static char raw_buf[RAW_BUF_SIZE] ALIGNMENT_ATTR;

#ifdef MODULE_PERIPH_FLASHPAGE_PAGEWISE
/**
 * @brief   Allocate space for 1 flash page in RAM
 *
 * @note    The flash page in RAM must be correctly aligned, even in RAM, when
 *          using flashpage. This is because some architecture uses
 *          32 bit alignment implicitly and there are cases (stm32l4) that
 *          requires 64 bit alignment.
 */
static uint8_t page_mem[FLASHPAGE_SIZE] ALIGNMENT_ATTR;
#endif

static int getpage(const char *str)
{
    int page = atoi(str);
    if ((page >= (int)FLASHPAGE_NUMOF) || (page < 0)) {
        printf("error: page %i is invalid\n", page);
        return -1;
    }
    return page;
}

#ifdef FLASHPAGE_SIZE
static void dumpchar(uint8_t mem)
{
    if (mem >= ' ' && mem <= '~') {
        printf("  %c  ", mem);
    }
    else {
        printf("  ?? ");
    }
}

static void memdump(void *addr, size_t len)
{
    unsigned pos = 0;
    uint8_t *mem = (uint8_t *)addr;

    while (pos < (unsigned)len) {
        for (unsigned i = 0; i < LINE_LEN; i++) {
            printf("0x%02x ", mem[pos + i]);
        }
        puts("");
        for (unsigned i = 0; i < LINE_LEN; i++) {
            dumpchar(mem[pos++]);
        }
        puts("");
    }
}

static void dump_local(void)
{
    puts("Local page buffer:");
    memdump(page_mem, FLASHPAGE_SIZE);
}
#endif

static int cmd_info(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("Flash start addr:\t0x%08x\n", (int)CPU_FLASH_BASE);
#ifdef FLASHPAGE_SIZE
    printf("Page size:\t\t%i\n", (int)FLASHPAGE_SIZE);
#else
    puts("Page size:\t\tvariable");
#endif
    printf("Number of pages:\t%i\n", (int)FLASHPAGE_NUMOF);

#ifdef FLASHPAGE_RWWEE_NUMOF
    printf("RWWEE Flash start addr:\t0x%08x\n", (int)CPU_FLASH_RWWEE_BASE);
    printf("RWWEE Number of pages:\t%i\n", (int)FLASHPAGE_RWWEE_NUMOF);
#endif

#ifdef NVMCTRL_USER
    printf("AUX page size:\t%i\n", FLASH_USER_PAGE_AUX_SIZE + sizeof(nvm_user_page_t));
    printf("    user area:\t%i\n", FLASH_USER_PAGE_AUX_SIZE);
#endif

    return 0;
}

#ifdef FLASHPAGE_SIZE
static int cmd_dump(int argc, char **argv)
{
    int page;
    void *addr;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }
    addr = flashpage_addr(page);

    printf("Flash page %i at address %p\n", page, addr);
    memdump(addr, flashpage_size(page));

    return 0;
}

static int cmd_dump_local(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    dump_local();

    return 0;
}

static int cmd_read(int argc, char **argv)
{
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }

    flashpage_read(page, page_mem);
    printf("Read flash page %i into local page buffer\n", page);
    dump_local();

    return 0;
}
#endif

#ifdef MODULE_PERIPH_FLASHPAGE_PAGEWISE
static int cmd_write(int argc, char **argv)
{
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }

    if (flashpage_write_and_verify(page, page_mem) != FLASHPAGE_OK) {
        printf("error: verification for page %i failed\n", page);
        return 1;
    }

    printf("wrote local page buffer to flash page %i at addr %p\n",
           page, flashpage_addr(page));
    return 0;
}
#endif

static uint32_t getaddr(const char *str)
{
    uint32_t addr = strtol(str, NULL, 16);

    return addr;
}

static int cmd_write_raw(int argc, char **argv)
{
#if (__SIZEOF_POINTER__ == 2)
    uint16_t addr;
#else
    uint32_t addr;
#endif

    if (argc < 3) {
        printf("usage: %s <addr> <data>\n", argv[0]);
        return 1;
    }

#if (__SIZEOF_POINTER__ == 2)
    addr = (uint16_t) getaddr(argv[1]);
#else
    addr = getaddr(argv[1]);
#endif
    /* try to align */
    memcpy(raw_buf, argv[2], strlen(argv[2]));

    flashpage_write((void*)addr, raw_buf, strlen(raw_buf));
#if (__SIZEOF_POINTER__ == 2)
    printf("wrote local data to flash address %#" PRIx16 " of len %u\n",
           addr, strlen(raw_buf));
#else
    printf("wrote local data to flash address %#" PRIx32 " of len %u\n",
           addr, strlen(raw_buf));
#endif
    return 0;
}

static int cmd_erase(int argc, char **argv)
{
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }
    flashpage_erase(page);

    printf("successfully erased page %i (addr %p)\n",
           page, flashpage_addr(page));
    return 0;
}

#ifdef FLASHPAGE_SIZE
static int cmd_edit(int argc, char **argv)
{
    int offset;
    size_t data_len;

    if (argc < 3) {
        printf("usage: %s <offset> <data>\n", argv[0]);
        return 1;
    }

    offset = atoi(argv[1]);
    if (offset >= (int)FLASHPAGE_SIZE) {
        printf("error: given offset is out of bounce\n");
        return -1;
    }
    data_len = strlen(argv[2]);
    if ((data_len + offset) > FLASHPAGE_SIZE) {
        data_len = FLASHPAGE_SIZE - offset;
    }

    memcpy(&page_mem[offset], argv[2], data_len);
    dump_local();

    return 0;
}
#endif

#ifdef MODULE_PERIPH_FLASHPAGE_PAGEWISE
static int cmd_test(int argc, char **argv)
{
    int page;
    char fill = 'a';

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage(argv[1]);
    if (page < 0) {
        return 1;
    }

    for (unsigned i = 0; i < sizeof(page_mem); i++) {
        page_mem[i] = (uint8_t)fill++;
        if (fill > 'z') {
            fill = 'a';
        }
    }

    if (flashpage_write_and_verify(page, page_mem) != FLASHPAGE_OK) {
        printf("error verifying the content of page %i\n", page);
        return 1;
    }

    printf("wrote local page buffer to flash page %i at addr %p\n",
           page, flashpage_addr(page));
    return 0;
}

/**
 * @brief   Does a write and verify test on last page available
 *
 * @note    Since every hardware can have different flash layouts for
 *          automated testing we always write to the last page available
 *          so we are independent of the size or layout
 */
static int cmd_test_last(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    char fill = 'a';

    for (unsigned i = 0; i < sizeof(page_mem); i++) {
        page_mem[i] = (uint8_t)fill++;
        if (fill > 'z') {
            fill = 'a';
        }
    }
#if defined(CPU_CC430) || defined(CPU_MSP430FXYZ)
    printf("The last page holds the ISR vector, so test page %d\n", TEST_LAST_AVAILABLE_PAGE);
#endif
    if (flashpage_write_and_verify(TEST_LAST_AVAILABLE_PAGE, page_mem) != FLASHPAGE_OK) {
        puts("error verifying the content of last page");
        return 1;
    }

    puts("wrote local page buffer to last flash page");
    return 0;
}
#endif

/**
 * @brief   Does a short raw write on last page available
 *
 * @note    Since every hardware can have different flash layouts for
 *          automated testing we always write to the last page available
 *          so we are independent of the size or layout
 */
static int cmd_test_last_raw(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    memset(raw_buf, 0, sizeof(raw_buf));

    /* try to align */
    memcpy(raw_buf, "test12344321tset", 16);
#if defined(CPU_CC430) || defined(CPU_MSP430FXYZ)
    printf("The last page holds the ISR vector, so test page %d\n", TEST_LAST_AVAILABLE_PAGE);
#endif

    /* erase the page first */
    flashpage_erase(TEST_LAST_AVAILABLE_PAGE);

    flashpage_write(flashpage_addr(TEST_LAST_AVAILABLE_PAGE), raw_buf, sizeof(raw_buf));

    /* verify that previous write_raw effectively wrote the desired data */
    if (memcmp(flashpage_addr(TEST_LAST_AVAILABLE_PAGE), raw_buf, strlen(raw_buf)) != 0) {
        puts("error verifying the content of last page");
        return 1;
    }

    puts("wrote raw short buffer to last flash page");
    return 0;
}

#ifdef FLASHPAGE_RWWEE_NUMOF

static int getpage_rwwee(const char *str)
{
    int page = atoi(str);
    if ((page >= (int)FLASHPAGE_RWWEE_NUMOF) || (page < 0)) {
        printf("error: RWWEE page %i is invalid\n", page);
        return -1;
    }
    return page;
}

static int cmd_read_rwwee(int argc, char **argv)
{
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage_rwwee(argv[1]);
    if (page < 0) {
        return 1;
    }

    flashpage_rwwee_read(page, page_mem);
    printf("Read RWWEE flash page %i into local page buffer\n", page);
    dump_local();

    return 0;
}

static int cmd_write_rwwee(int argc, char **argv)
{
    int page;

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage_rwwee(argv[1]);
    if (page < 0) {
        return 1;
    }

    if (flashpage_rwwee_write_and_verify(page, page_mem) != FLASHPAGE_OK) {
        printf("error: verification for RWWEE page %i failed\n", page);
        return 1;
    }

    printf("wrote local page buffer to RWWEE flash page %i at addr %p\n",
           page, flashpage_rwwee_addr(page));
    return 0;
}

static int cmd_test_rwwee(int argc, char **argv)
{
    int page;
    char fill = 'a';

    if (argc < 2) {
        printf("usage: %s <page>\n", argv[0]);
        return 1;
    }

    page = getpage_rwwee(argv[1]);
    if (page < 0) {
        return 1;
    }

    fill += (page % ('z' - 'a')); // Make each page slightly different by changing starting char for easier comparison by eye

    for (unsigned i = 0; i < sizeof(page_mem); i++) {
        page_mem[i] = (uint8_t)fill++;
        if (fill > 'z') {
            fill = 'a';
        }
    }

    if (flashpage_rwwee_write_and_verify(page, page_mem) != FLASHPAGE_OK) {
        printf("error verifying the content of RWWEE page %i\n", page);
        return 1;
    }

    printf("wrote local page buffer to RWWEE flash page %i at addr %p\n",
           page, flashpage_rwwee_addr(page));
    return 0;
}

/**
 * @brief   Does a write and verify test on last page available
 *
 * @note    Since every hardware can have different flash layouts for
 *          automated testing we always write to the last page available
 *          so we are independent of the size or layout
 */
static int cmd_test_last_rwwee(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    char fill = 'a';

    for (unsigned i = 0; i < sizeof(page_mem); i++) {
        page_mem[i] = (uint8_t)fill++;
        if (fill > 'z') {
            fill = 'a';
        }
    }

    if (flashpage_rwwee_write_and_verify((int)FLASHPAGE_RWWEE_NUMOF - 1, page_mem) != FLASHPAGE_OK) {
        puts("error verifying the content of last RWWEE page");
        return 1;
    }

    puts("wrote local page buffer to last RWWEE flash page");
    return 0;
}

/**
 * @brief   Does a short raw write on last page available
 *
 * @note    Since every hardware can have different flash layouts for
 *          automated testing we always write to the last page available
 *          so we are independent of the size or layout
 */
static int cmd_test_last_rwwee_raw(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    /* try to align */
    memcpy(raw_buf, "test12344321tset", 16);

    /* erase the page first */
    flashpage_rwwee_write_page(((int)FLASHPAGE_RWWEE_NUMOF - 1), NULL);

    flashpage_rwwee_write(flashpage_rwwee_addr((int)FLASHPAGE_RWWEE_NUMOF - 1), raw_buf, strlen(raw_buf));

    /* verify that previous write_raw effectively wrote the desired data */
    if (memcmp(flashpage_rwwee_addr((int)FLASHPAGE_RWWEE_NUMOF - 1), raw_buf, strlen(raw_buf)) != 0) {
        puts("error verifying the content of last RWWEE page");
        return 1;
    }

    puts("wrote raw short buffer to last RWWEE flash page");
    return 0;
}

#endif

#ifdef NVMCTRL_USER
static int cmd_dump_config(int argc, char **argv)
{
    (void) argc;
    (void) argv;

#ifdef FLASH_USER_PAGE_SIZE
    od_hex_dump_ext((void*)NVMCTRL_USER, FLASH_USER_PAGE_SIZE, 0, NVMCTRL_USER);
#else
    od_hex_dump_ext((void*)NVMCTRL_USER, AUX_PAGE_SIZE * AUX_NB_OF_PAGES, 0, NVMCTRL_USER);
#endif

    return 0;
}

static int cmd_test_config(int argc, char **argv)
{
    /* This test is sam0 specific and also tests
     * the unaligned writes for the sam0 flashpage
     * driver implementation
     */

    (void) argc;
    (void) argv;

    const uint16_t single_data = 0x1234;
    const uint8_t test_data[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE };

    assert(((int32_t)FLASH_USER_PAGE_AUX_SIZE - (int32_t)(sizeof(test_data) + 2 + 3)) > 0);

    puts("[START]");

    for (uint32_t dst_offset = 0; dst_offset < 4; dst_offset++) {
        /* destination base at 4 byte aligned address */
        uint32_t dst = (uint32_t)(FLASH_USER_PAGE_AUX_SIZE
                - (sizeof(test_data) + 2 + 3)) & ~((uint32_t)0x3);
        /* add data destination offset */
        dst += dst_offset;

        /* reset aux page */
        sam0_flashpage_aux_reset(NULL);

        /* check if the AUX page has been cleared */
        for (uint32_t i = 0; i < FLASH_USER_PAGE_AUX_SIZE; ++i) {
            if (*(uint8_t*)sam0_flashpage_aux_get(i) != 0xFF) {
                printf("dst_offset=%"PRIu32": user page not cleared at offset 0x%"PRIx32"\n", dst_offset, i);
                return -1;
            }
        }

        /* write test data */
        sam0_flashpage_aux_write(dst, test_data, sizeof(test_data));

        /* write single half-word */
        sam0_flashpage_aux_write(dst + sizeof(test_data), &single_data, sizeof(single_data));

        /* check if half-word was written correctly */
        uint16_t data_in = unaligned_get_u16(sam0_flashpage_aux_get(dst + sizeof(test_data)));
        if (data_in != single_data) {
            printf("dst_offset=%"PRIu32": %x != %x, offset = 0x%"PRIx32"\n", dst_offset, single_data, data_in, dst + sizeof(test_data));
            return -1;
        }

        /* check if test data was written correctly */
        if (memcmp(sam0_flashpage_aux_get(dst), test_data, sizeof(test_data))) {
            printf("dst_offset=%"PRIu32": write test_data failed, offset = 0x%"PRIx32"\n", dst_offset, dst);
            return -1;
        }
    }

    puts("[SUCCESS]");

    return 0;
}
#endif /* NVMCTRL_USER */

static const shell_command_t shell_commands[] = {
    { "info", "Show information about pages", cmd_info },
#ifdef MODULE_PERIPH_FLASHPAGE_PAGEWISE
    { "dump", "Dump the selected page to STDOUT", cmd_dump },
    { "dump_local", "Dump the local page buffer to STDOUT", cmd_dump_local },
    { "read", "Copy the given page to the local page buffer and dump to STDOUT", cmd_read },
    { "write", "Write the local page buffer to the given page", cmd_write },
#endif
    { "write_raw", "Write (ASCII, max 64B) data to the given address", cmd_write_raw },
    { "erase", "Erase the given page buffer", cmd_erase },
#ifdef MODULE_PERIPH_FLASHPAGE_PAGEWISE
    { "edit", "Write bytes to the local page buffer", cmd_edit },
    { "test", "Write and verify test pattern", cmd_test },
    { "test_last_pagewise", "Write and verify test pattern on last page available", cmd_test_last },
#endif
    { "test_last_raw", "Write and verify raw short write on last page available", cmd_test_last_raw },
#ifdef FLASHPAGE_RWWEE_NUMOF
    { "read_rwwee", "Copy the given page from RWWEE to the local page buffer and dump to STDOUT", cmd_read_rwwee },
    { "write_rwwee", "Write the local page buffer to the given RWWEE page", cmd_write_rwwee },
    { "test_rwwee", "Write and verify test pattern to RWWEE", cmd_test_rwwee },
    { "test_last_rwwee", "Write and verify test pattern on last RWWEE page available", cmd_test_last_rwwee },
    { "test_last_rwwee_raw", "Write and verify raw short write on last RWWEE page available", cmd_test_last_rwwee_raw },
#endif
#ifdef NVMCTRL_USER
    { "dump_config_page", "Dump the content of the MCU configuration page", cmd_dump_config },
    { "test_config_page", "Test writing config page. (!DANGER ZONE!)", cmd_test_config },
#endif
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("ROM flash read write test\n");
    puts("Please refer to the README.md for further information\n");

    cmd_info(0, NULL);

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
