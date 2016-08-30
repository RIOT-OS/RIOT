/*
 * Copyright (C) 2015 Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     fs
 * @brief
 * @{
 *
 * @brief       Simulator of flash storage
 * @author      Lucas Jenß <lucas@x3ro.de>
 */

#ifndef _FS_FLASH_SIM_H
#define _FS_FLASH_SIM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum flash_sim_error {
    E_SUCCESS = 0,
    E_INVALID_WRITE = 1, /* Trying to set a bit from 0 -> 1 w/ a program operation */
    E_INVALID_PARAMS = 2,     /* Invalid parameters were passed to the function */
    E_OUT_OF_RANGE = 3,
    E_FILE_ERROR = 4,
    E_NOT_INITIALIZED = 5

} flash_sim_error_t;

typedef struct flash_sim {
    uint32_t page_size;
    uint32_t block_size;
    uint64_t storage_size;

    // Computed properties
    uint32_t pages;

    // "Private" properties
    FILE *_fp;
} flash_sim;



flash_sim_error_t flash_sim_init(flash_sim *fs);
flash_sim_error_t flash_sim_format(flash_sim *fs);
flash_sim_error_t flash_sim_read(flash_sim *fs, void *buffer, uint32_t page);
flash_sim_error_t flash_sim_write(flash_sim *fs, const void *buffer, uint32_t page);
flash_sim_error_t flash_sim_erase(flash_sim *fs, uint32_t block);



#ifdef __cplusplus
}
#endif

#endif
/** @} */
