/*
 * Copyright (C) 2014 Frank Holtz
 * Copyright (C) 2014 Ludwig Ortmann
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
 * @author      Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
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
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "cpu-conf.h"
#include "periph/flash.h"

#include "native_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int _native_flash_fd = -1;
volatile uint8_t *_native_flash_memory = NULL;
size_t _native_flash_size = (FLASH_NUM_PAGES *FLASH_PAGE_SIZE);


/************************************************************************/
/* internal API *********************************************************/
/************************************************************************/

/**
 * @brief Check if given address in valid range
 *
 * @return  Error code FLASH_ERROR_SUCCESS or FLASH_ERROR_ADDR_RANGE
 */
static uint8_t flash_check_address(uint8_t *address)
{
    if ((address < _native_flash_memory)
        || ((address >= _native_flash_memory + _native_flash_size))) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    return FLASH_ERROR_SUCCESS;
}

/************************************************************************/
/* periph/flash.h *******************************************************/
/************************************************************************/

uint8_t flash_init(void)
{
    DEBUG("flash initialized\n");
    return FLASH_ERROR_SUCCESS;
}

flash_page_number_t flash_get_page_number(void *address, flash_page_size_t *page_offset)
{
    flash_page_number_t page_number = (flash_page_number_t)(((uint8_t *)address -
                                      &_native_flash_memory[0]) / FLASH_PAGE_SIZE);

    if (page_offset != NULL) {
        *page_offset = (flash_page_number_t)(
                           ((uint8_t *)address - &_native_flash_memory[0])
                           % FLASH_PAGE_SIZE);
    }

    return page_number;
}

void *flash_get_address(flash_page_number_t page)
{
    return ((void *)(_native_flash_memory + (page * FLASH_PAGE_SIZE)));
}

uint8_t flash_memcpy(void *dest, const void *src, size_t n)
{
    /* Check memory range */
    if (flash_check_address((uint8_t *) dest) > FLASH_ERROR_SUCCESS) {
        DEBUG("attempted to write below first address\n");
        return FLASH_ERROR_ADDR_RANGE;
    }

    if (flash_check_address((((uint8_t *) dest) + n)) > FLASH_ERROR_SUCCESS) {
        DEBUG("attemted to write beyond last address\n");
        return FLASH_ERROR_ADDR_RANGE;
    }

    memcpy(dest, src, n);

    return (FLASH_ERROR_SUCCESS);
}

uint8_t flash_memcpy_fast(void *dest, const void *src, size_t n)
{
    /* Check alignment */
#if FLASH_WRITE_ALIGN>1
    if ((n % FLASH_WRITE_ALIGN != 0) ||
        ((size_t)src % FLASH_WRITE_ALIGN != 0) ||
        ((size_t)dest % FLASH_WRITE_ALIGN != 0)) {
        DEBUG("Unaligned access dest=%d, src=%d, n=%d\n", (size_t)mydst, (size_t)mysrc, n);
        return FLASH_ERROR_ALIGNMENT;
    }

#endif

    /* Check memory range */
    if (flash_check_address((uint8_t *) dest) > FLASH_ERROR_SUCCESS) {
        DEBUG("attempted to write below first address\n");
        return FLASH_ERROR_ADDR_RANGE;
    }

    if (flash_check_address((((uint8_t *) dest) + n)) > FLASH_ERROR_SUCCESS) {
        DEBUG("attemted to write beyond last address\n");
        return FLASH_ERROR_ADDR_RANGE;
    }

    memcpy(dest, src, n);

    return FLASH_ERROR_SUCCESS;
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
        _native_flash_memory[i] = FLASH_ERASED_WORD_VALUE;
    }

    return FLASH_ERROR_SUCCESS;
}

uint8_t flash_erase_page_by_address(void *address)
{
    if (flash_check_address(address) > FLASH_ERROR_SUCCESS) {
        return FLASH_ERROR_ADDR_RANGE;
    }

    flash_page_size_t page_offset;
    return (flash_erase_page(flash_get_page_number(address, &page_offset)));
}

/************************************************************************/
/* native internal API **************************************************/
/************************************************************************/

void _native_flash_init(void)
{
    DEBUG("_native_flash_init\n");
    off_t init_start = 0, init_length = _native_flash_size;

    /* open and mmap file to memory if path given, malloc otherwise */
    if (_native_flash_path != NULL) {
        DEBUG("_native_flash_init: opening file\n");
        _native_syscall_enter();

        /* try to open [existing] file */
        if ((_native_flash_fd = real_open(_native_flash_path,
                                          O_RDWR | O_CREAT, 0600)) == -1) {
            err(EXIT_FAILURE, "_native_flash_init: open");
        }

        /* make sure the file is large enough and adjust boundaries
         * for initialization */
        size_t size = 0;
        char buf[100];
        int ret;

        for (ret = 0; (ret = read(_native_flash_fd, buf, sizeof(buf))) > 0;) {
            size += ret;
        }

        if (ret == -1) {
            err(EXIT_FAILURE, "_native_flash_init: read");
        }

        init_start = size;

        if (size < _native_flash_size) {
            init_length = _native_flash_size - size;

            if (real_lseek(_native_flash_fd, _native_flash_size - 1,
                           SEEK_SET) == -1) {
                err(EXIT_FAILURE, "_native_flash_init: lseek");
            }

            if (real_write(_native_flash_fd, "", 1) != 1) {
                err(EXIT_FAILURE, "_native_flash_init: write");
            }
        }
        else {
            init_length = 0;
        }

        /* try to map file into memory */
        if ((_native_flash_memory = real_mmap(NULL, _native_flash_size,
                                              PROT_READ | PROT_WRITE, MAP_SHARED, _native_flash_fd, 0)
            ) == MAP_FAILED) {
            err(EXIT_FAILURE, "_native_flash_init: mmap");
        }

        DEBUG("_native_flash_init: using file %s\n", _native_flash_path);

        _native_syscall_leave();
    }
    else {
        DEBUG("_native_flash_init: allocating memory\n");
        _native_syscall_enter();

        /* try and allocate memory */
        if ((_native_flash_memory = real_malloc(_native_flash_size)) == NULL) {
            err(EXIT_FAILURE, "_native_flash_init: malloc");
        }

        _native_syscall_leave();
        DEBUG("_native_flash_init: using memory %p\n", _native_flash_memory);
    }

    /* initialize [remaining] memory area */
    memset((void *)(_native_flash_memory + init_start), FLASH_ERASED_WORD_VALUE,
           init_length);
}
