/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <diskio.h>
#include <assert.h>

#include "storage/ftl.h"

int write(const unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length);

int read(unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length);

int erase(uint32_t block);

unsigned char subpage_buffer[512];
unsigned char ecc_buffer[6];

ftl_device_s device = {
    .total_pages = 32768,
    .page_size = 512,
    .subpage_size = 512,
    .pages_per_block = 1024,
    .ecc_size = 6,
    .partition_count = 2,

    ._write = write,
    ._read = read,
    ._erase = erase,
    ._bulk_erase = NULL,

    ._subpage_buffer = subpage_buffer,
    ._ecc_buffer = ecc_buffer
};


ftl_partition_s index_partition = {
    .device = &device,
    .base_offset = 0,
    .size = 4
};


ftl_partition_s data_partition = {
    .device = &device,
    .base_offset = 4,
    .size = 27
};

ftl_partition_s *partitions[] = {
    &index_partition,
    &data_partition
};

int write(const unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
    assert(offset == 0);
    assert(length == device.subpage_size);
    int ret = mci_write(buffer, page, 1);
    return ret;
}

int read(unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
    assert(offset == 0);
    assert(length == device.subpage_size);
    int ret = mci_read(buffer, page, 1);
    return ret;
}

int erase(uint32_t block) {
    unsigned int buff[2];
    buff[0] = block * device.pages_per_block;
    buff[1] = (block + 1) * device.pages_per_block - 1;
    int ret = mci_ioctl(CTRL_ERASE_SECTOR, &buff);
    return ret;
}

int main(void)
{
    int ret;

    device.partitions = partitions;
    ret = ftl_init(&device);
    assert(ret == 0);

    ret = ftl_read_raw(&index_partition, device._subpage_buffer, 0);
    assert(ret == 0);

    printf("Success\n");
    return 0;
}
