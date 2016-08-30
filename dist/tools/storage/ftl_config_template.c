/*
 * Copyright (C) 2016 Lucas Jenß <lucas@x3ro.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       FTL configuration example
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "storage/ftl.h"

int flash_driver_write(const char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
    // TODO: Remove this and implement interface to the flash storage driver.
    buffer = buffer;
    page = page;
    offset = offset;
    length = length;
    return -1;
}

int flash_driver_read(char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
    // TODO: Remove this and implement interface to the flash storage driver.
    buffer = buffer;
    page = page;
    offset = offset;
    length = length;
    return -1;
}

int flash_driver_erase(uint32_t block) {
    // TODO: Remove this and implement interface to the flash storage driver.
    block = block;
    return -1;
}

int flash_driver_bulk_erase(uint32_t block, uint32_t length) {
    // TODO: Remove this and implement interface to the flash storage driver.
    //       If bulk-erase is not supported by your flash device, you should remove
    //       this function.
    block = block;
    length = length;
    return -1;
}

unsigned char subpage_buffer[${subpage_size}];
unsigned char ecc_buffer[${ecc_size}];

ftl_device_s device = {
    .total_pages = ${total_pages},
    .page_size = ${page_size},
    .subpage_size = ${subpage_size},
    .pages_per_block = ${pages_per_block},
    .ecc_size = ${ecc_size},
    .partition_count = ${partition_count},

    ._write = flash_driver_write,
    ._read = flash_driver_read,
    ._erase = flash_driver_erase,
    ._bulk_erase = flash_driver_bulk_erase,

    ._subpage_buffer = subpage_buffer,
    ._ecc_buffer = ecc_buffer
};

${partitions}

${partition_list}

int main(void)
{
    device.partitions = partitions;
    int ret = ftl_init(&device);
    assert(ret == 0);

    printf("You are running RIOT.\n");
    printf("Storage pages configured: %d\n", device.total_pages);

    return 0;
}
