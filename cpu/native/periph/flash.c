/*
 * Copyright (C) 2014 Frank Holtz
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * Native CPU periph/flash.h implementation
 *
 * The implementation emulates flash ROM by allocating RAM.
 * When using the "-f" argument, flash contents are synchronized to a
 * given file.
 *
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 * @ingroup _native_cpu
 * @defgroup _native_flash
 * @file
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "cpu-conf.h"
#include "periph/flash.h"

#include "native_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define FLASH_ERROR_SIMULATION (1)

static int _native_flash_initialized = 0;
/* file descriptor for flash content in file system */
int _native_flash_fd;
/* simulated flash */
static uint8_t _native_flash_content[FLASH_NUM_PAGES * FLASH_PAGE_SIZE];

/**
 * @brief Check if given address in valid range
 *
 * @return             Error code FLASH_ERROR_SUCCESS or FLASH_ERROR_ADDR_RANGE
 */
static uint8_t flash_check_address(void *address)
{
    if ((address < (void *)&_native_flash_content)
        || ((address > (void *)&_native_flash_content[(FLASH_NUM_PAGES * FLASH_PAGE_SIZE) - 1]))) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    return FLASH_ERROR_SUCCESS;
}

/**
 * @brief store simulated flash content in file
 *
 * @param[in]  page    Page number
 *
 * @return             Error code FLASH_ERROR_SUCCESS or FLASH_ERROR_VERIFY
 */
static uint8_t flash_write_page_to_file(flash_page_number_t page, flash_page_number_t num)
{
    /* Check range */
    if (page + num > FLASH_NUM_PAGES) {
        DEBUG("Write overrides array boundary\n");
        return FLASH_ERROR_VERIFY;
    }

    if (_native_flash_fd >= 0) {
        ssize_t numbytes = 0;

        _native_syscall_enter();

        if (real_lseek(_native_flash_fd, 0, SEEK_END) < page * FLASH_PAGE_SIZE) {
            /* Write complete array */
            real_lseek(_native_flash_fd, 0, SEEK_SET);
            numbytes = real_write(_native_flash_fd,
                                    &_native_flash_content[0],
                                    FLASH_NUM_PAGES * FLASH_PAGE_SIZE);
        }
        else {
            /* Write block */
            size_t pos = real_lseek(_native_flash_fd,
                                    page * FLASH_PAGE_SIZE, SEEK_SET);

            if (pos == page * FLASH_PAGE_SIZE) {
                numbytes = real_write(_native_flash_fd,
                            &_native_flash_content[FLASH_PAGE_SIZE * page],
                            num * FLASH_PAGE_SIZE);
            }
        }

        _native_syscall_leave();

        DEBUG("Write %d bytes to %s\n", numbytes, _native_flash_emulation_file_path);
    }

    return FLASH_ERROR_SUCCESS;
}

uint8_t flash_init(void)
{
    DEBUG("flash_init\n");
    _native_flash_fd = -1;

    /* Format flash */
    /* TODO: move below to else in case no file is associated */
    for (flash_page_number_t i = 0; i < FLASH_NUM_PAGES; i++) {
        flash_erase_page(i);
    }

    if (_native_flash_emulation_file_path != NULL) {

        _native_syscall_enter();
        _native_flash_fd = real_open(_native_flash_emulation_file_path, O_RDWR | O_CREAT, 0600);

        if (_native_flash_fd >= 0) {
            real_lseek(_native_flash_fd, 0, SEEK_SET);
            /* TODO: check numbytes, could be lower than requested */
            ssize_t numbytes = real_read(_native_flash_fd, _native_flash_content,
                                    sizeof(_native_flash_content));
            DEBUG("Read %d bytes from %s\n", numbytes, _native_flash_emulation_file_path);
        }

        _native_syscall_leave();
    }

    _native_flash_initialized = 1;
    DEBUG("Flash emulation initialized.\n");
    return FLASH_ERROR_SUCCESS;
}

flash_page_number_t flash_get_page_number(void *address, flash_page_size_t *page_offset)
{
    flash_page_number_t page_number = (flash_page_number_t)(((uint8_t *)address -
                                      &_native_flash_content[0]) / FLASH_PAGE_SIZE);

    if (page_offset != NULL) {
        *page_offset = (flash_page_number_t) (
                        ((uint8_t *)address - &_native_flash_content[0])
                        % FLASH_PAGE_SIZE);
    }

    return page_number;
}

void *flash_get_address(flash_page_number_t page)
{
    return (void *)&_native_flash_content[page * FLASH_PAGE_SIZE];
}

uint8_t flash_memcpy(void *dest, const void *src, size_t n)
{
    /* Check alignment */
#if FLASH_WRITE_ALIGN > 1
    flash_page_size_t page_offset;
    flash_get_page_number(dest, &page_offset);

    if ((n % FLASH_WRITE_ALIGN != 0) || (page_offset % FLASH_WRITE_ALIGN != 0)) {
        DEBUG("Unaligned access n=%d offset=%d\n", n, page_offset);
        return FLASH_ERROR_ALIGNMENT;
    }
#endif

    uint8_t ret = flash_memcpy_fast(dest, src, n);
    return (ret);
}

uint8_t flash_memcpy_fast(void *dest, const void *src, size_t n)
{
    /* Check memory range */
    if ((flash_check_address(dest) > FLASH_ERROR_SUCCESS)
        || (flash_check_address((void *)((uint8_t *)dest + n)) > FLASH_ERROR_SUCCESS)) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    memcpy(dest, src, n);

    /* Write to disk */
    flash_page_size_t page_offset = 0;
    flash_page_number_t page_num = flash_get_page_number(dest, &page_offset);
    flash_page_number_t num_pages = n / FLASH_PAGE_SIZE;

    if (n % FLASH_PAGE_SIZE > 0) {
        num_pages++;
    }

    return flash_write_page_to_file(page_num, num_pages);
}

uint8_t flash_erase_page(flash_page_number_t page)
{
    /* check argument */
    if (page > FLASH_NUM_PAGES) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    /* erase content */
    DEBUG("Erase page %d\n", page);

    for (ssize_t i = page * FLASH_PAGE_SIZE; i < ((page + 1) * FLASH_PAGE_SIZE); i++) {
#if FLASH_ERASED_BIT_VALUE == 0
        _native_flash_content[i] = 0;
#else
        _native_flash_content[i] = 0xff;
#endif
    }

    return (flash_write_page_to_file(page, 1));
}

uint8_t flash_erase_page_by_address(void *address)
{
    if (flash_check_address(address) > FLASH_ERROR_SUCCESS) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    flash_page_size_t page_offset;
    return (flash_erase_page(flash_get_page_number(address, &page_offset)));
}
