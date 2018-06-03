/*
 * Copyright (C) 2015 Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_storage
 * @brief
 * @{
 *
 * @brief       Simulates flash memory for easier testing of storage subsystems
 * @author      Lucas Jenß <lucas@x3ro.de>
 */

#ifndef _FS_FLASH_SIM_H
#define _FS_FLASH_SIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "storage/ftl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configuration for the flash simulation layer
 */
typedef struct flash_sim {
    uint32_t page_size;         //!< Page size to be emulated
    uint32_t block_size;        //!< Block size (must be a multiple of the page size)
    uint64_t storage_size;      //!< Total amount of storage to be made available

    uint32_t pages;             //!< Number of pages (computed from the above)
    FILE *_fp;                  //!< File descriptor for the file simulating the flash device
} flash_sim;


/**
 * @brief Initialize the flash simulation layer with the given configuration struct
 * @param  fs Configuration
 * @return    0 on success
 * @return    -EINVAL if the given configuration struct was invalid
 * @return    -EBADFD if there was an error with the file used for simulated storage
 * @return    Any error #flash_sim_format may return
 */
int flash_sim_init(flash_sim *fs);

/**
 * @brief Erases all blocks of the simulated flash device
 * @param  fs Configuration
 *
 * @return    0 on success
 * @return    -ENODEV if device has not been initialized
 * @return    -EBADFD if there was an error with the file used for simulated storage
 */
int flash_sim_format(flash_sim *fs);

/**
 * @brief Reads the given page into the provided buffer
 * @param  fs     Configuration
 * @param  buffer Target buffer (must be >= the size of a page)
 * @param  page   Absolute page index to be read
 *
 * @return        0 on success
 * @return        -ENODEV if device has not been initialized
 * @return        -EFAULT if given page was out of range
 */
int flash_sim_read(const flash_sim *fs, void *buffer, uint32_t page);

/**
 * @brief Partially reads data from the given page into the target buffer
 * @param  fs     Configuration
 * @param  buffer Target buffer (must be >= the provided length)
 * @param  page   Absolute page index to be read
 * @param  offset Offset inside the page from which to start reading
 * @param  length Amount of bytes to read
 *
 * @return        0 on success
 * @return        Any error #flash_sim_read may return
 */
int flash_sim_read_partial(const flash_sim *fs,
                                         void *buffer,
                                         uint32_t page,
                                         uint32_t offset,
                                         uint32_t length);

/**
 * @brief Writes the provided buffer to the given page
 * @param  fs     Configuration
 * @param  buffer Source buffer (must be >= the size of a page)
 * @param  page   Absolute page index to be written to
 *
 * @return        0 on success
 * @return        -ENODEV if device has not been initialized
 * @return        -EIO if trying to perform an invalid write operation (e.g., setting bits
 *                back to 1 without erasing first)
 * @return        -EBADFD if there was an error with the file used for simulated storage
 * @return        -EFAULT if given page was out of range
 * @return        Any error #flash_sim_read may return
 */
int flash_sim_write(const flash_sim *fs, const void *buffer, uint32_t page);

/**
 * @brief Partially writes data from the provided buffer to the given page
 * @param  fs     Configuration
 * @param  buffer Target buffer (must be >= the provided length)
 * @param  page   Absolute page index to be written to
 * @param  offset Offset inside the page indicating where to start writing
 * @param  length Amount of bytes to write
 *
 * @return        0 on success
 * @return        Any error #flash_sim_read may return
 * @return        Any error #flash_sim_write may return
 */
int flash_sim_write_partial(const flash_sim *fs,
                                          const void *buffer,
                                          uint32_t page,
                                          uint32_t offset,
                                          uint32_t length);

/**
 * @brief Erase the given block
 * @param  fs    Configuration
 * @param  block Absolute block index
 *
 * @return       0 on success
 * @return       -ENODEV if device has not been initialized
 * @return       -EBADFD if there was an error with the file used for simulated storage
 * @return       -EFAULT if given page was out of range
 */
int flash_sim_erase(const flash_sim *fs, uint32_t block);

int flash_sim_ftl_write(flash_sim *fs,
                  const char *buffer,
                  uint32_t page,
                  uint32_t offset,
                  uint16_t length);

int flash_sim_ftl_read(flash_sim *fs,
                 char *buffer,
                 uint32_t page,
                 uint32_t offset,
                 uint16_t length);

int flash_sim_ftl_erase(flash_sim *fs, uint32_t block);



#ifdef __cplusplus
}
#endif

#endif
/** @} */
