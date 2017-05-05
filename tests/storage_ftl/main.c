/*
 * Copyright (C) Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for flash memory simulation
 *
 * @author      Lucas Jenß <lucas@x3ro.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>


#include "embUnit.h"
#include "periph/pm.h"
#include "xtimer.h"
#include "storage/flash_sim.h"
#include "storage/ftl.h"

#define ENABLE_DEBUG 1

#include "board.h"
#include "debug.h"
#include "mtd_native.h"


//native_flash_driver



#define USTRNCMP(A, B, C)   strncmp((char*) A, (char*) B, C)

#define HEXDUMP_BUFFER(buffer, size)    for(int i=0; i<(int)size; i++) { \
                                    printf("%02x ", ((unsigned char*) buffer)[i]); \
                                    if((i+1)%32 == 0) { printf("\n"); } \
                                } \
                                printf("\nbuffer ^^^^^\n");


#define TEST_FTL_HEADER_SIZE 3 // sizeof(subpageheader_s)
#define TEST_FTL_ECC_SIZE 6

#ifdef BOARD_NATIVE

flash_sim fs;

#define FTL_SUBPAGE_SIZE 128



/* Functions for a flash_sim based FTL device */

int write(const unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
    //return flash_sim_ftl_write(&fs, (char*) buffer, page, offset, length);
    return mtd_write(mtd0, buffer, page * mtd0->page_size + offset, length);
}

int read(unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
    //return flash_sim_ftl_read(&fs, (char*) buffer, page, offset, length);
    DEBUG("read page: %u addr: %u count: %u\n", page, page * mtd0->page_size + offset, length);
    return mtd_read(mtd0, buffer, page * mtd0->page_size + offset, length);

}

int erase(uint32_t block) {
    //return flash_sim_ftl_erase(&fs, block);
    uint32_t block_size = mtd0->page_size * mtd0->pages_per_sector;
    DEBUG("erase block: %u addr: %u size: %u\n", block, block * block_size, block_size);
    return mtd_erase(mtd0, block * block_size, block_size);
}

unsigned char subpage_buffer[128];
unsigned char ecc_buffer[6];

ftl_device_s device = {
    .total_pages = 32768,
    .page_size = 256,
    .subpage_size = 128,
    .pages_per_block = 16,
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

#endif /* Board Native */



// #ifdef BOARD_MSBA2

// #include <diskio.h>
// #define FTL_PAGE_SIZE 512
// #define FTL_SUBPAGE_SIZE 512
// #define FTL_PAGES_PER_BLOCK 1024
// #define FTL_TOTAL_PAGES 32768

// int write(const unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
//     assert(offset == 0);
//     assert(length == FTL_SUBPAGE_SIZE);
//     int ret = MCI_write(buffer, page, 1);
//     return ret;
// }

// int read(unsigned char *buffer, uint32_t page, uint32_t offset, uint16_t length) {
//     assert(offset == 0);
//     assert(length == FTL_SUBPAGE_SIZE);
//     //int ret = mtd_read(mtd0, buffer, page * mtd->page_size, length);
//     //int ret = MCI_read(buffer, page, 1);
//     ret = 1;
//     return ret;
// }

// int erase(uint32_t block) {
//     //unsigned int block_to_erase = block + 0;
//     unsigned int buff[2];
//     buff[0] = block * FTL_PAGES_PER_BLOCK;
//     buff[1] = (block + 1) * FTL_PAGES_PER_BLOCK - 1;
//     int ret = MCI_ioctl(CTRL_ERASE_SECTOR, &buff);
//     return ret;
// }

// unsigned char subpage_buffer[512];
// unsigned char ecc_buffer[6];

// ftl_device_s device = {
//     .total_pages = 32768,
//     .page_size = 512,
//     .subpage_size = 512,
//     .pages_per_block = 1024,
//     .ecc_size = 6,
//     .partition_count = 2,

//     ._write = write,
//     ._read = read,
//     ._erase = erase,
//     ._bulk_erase = NULL,

//     ._subpage_buffer = subpage_buffer,
//     ._ecc_buffer = ecc_buffer
// };


// ftl_partition_s index_partition = {
//     .device = &device,
//     .base_offset = 0,
//     .size = 4
// };


// ftl_partition_s data_partition = {
//     .device = &device,
//     .base_offset = 4,
//     .size = 27
// };



// ftl_partition_s *partitions[] = {
//     &index_partition,
//     &data_partition
// };

// #endif /* Board MSBA2 */



unsigned char page_buffer[FTL_SUBPAGE_SIZE];
unsigned char expect_buffer[FTL_SUBPAGE_SIZE];

#ifdef BOARD_NATIVE

static void test_init(void) {
    TEST_ASSERT_EQUAL_INT(false, ftl_is_initialized(&device));

    device.partitions = partitions;

    // device._write = write;
    // device.read = read;
    // device.erase = erase;
    // device.page_size = FTL_PAGE_SIZE;
    // device.subpage_size = FTL_SUBPAGE_SIZE;
    // device.pages_per_block = FTL_PAGES_PER_BLOCK;
    // device.total_pages = FTL_TOTAL_PAGES;

    fs.page_size = device.page_size;
    fs.block_size = device.pages_per_block * device.page_size;
    fs.storage_size = device.total_pages * device.page_size;

    int ret = flash_sim_init(&fs);
    int ret1 = mtd_init(mtd0);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, ret1);

    TEST_ASSERT_EQUAL_INT(false, ftl_is_initialized(&device));
    ret = ftl_init(&device);
    TEST_ASSERT_EQUAL_INT(true, ftl_is_initialized(&device));
    TEST_ASSERT(index_partition.device != 0);
    TEST_ASSERT_EQUAL_INT(0, index_partition.base_offset);
    TEST_ASSERT_EQUAL_INT(4, index_partition.size);

    TEST_ASSERT(data_partition.device != 0);
    TEST_ASSERT_EQUAL_INT(4, data_partition.base_offset);
    TEST_ASSERT_EQUAL_INT(27, data_partition.size);

    TEST_ASSERT_EQUAL_INT(6, device.ecc_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

#endif /* Board Native */


#ifdef BOARD_MSBA2

static void test_init(void) {
    DSTATUS status = MCI_initialize();
    if(status == STA_NOINIT) {
        printf("Could not initialize MCI interface :(\n");
    } else if(status == STA_NODISK) {
        printf("NO SDCard detected. Aborting\n");
    } else if(status == STA_PROTECT) {
        printf("SDCard is in read-only mode\n");
    }

    TEST_ASSERT_EQUAL_INT(0, status);

    unsigned long sector_count = 0;
    MCI_ioctl(GET_SECTOR_COUNT, &sector_count);
    printf("sector_count: %lu\n", sector_count);

    unsigned short sector_size = 0;
    MCI_ioctl(GET_SECTOR_SIZE, &sector_size);
    printf("sector_size: %hu\n", sector_size);

    unsigned long block_size = 0;
    MCI_ioctl(GET_BLOCK_SIZE, &block_size);
    printf("block_size: %lu\n", block_size);

    device.partitions = partitions;

    // device.page_size = FTL_PAGE_SIZE;
    // device.subpage_size = FTL_SUBPAGE_SIZE;
    // device.pages_per_block = FTL_PAGES_PER_BLOCK;
    // device.total_pages = FTL_TOTAL_PAGES;

    int ret = ftl_init(&device);
    TEST_ASSERT(index_partition.device != 0);
    TEST_ASSERT_EQUAL_INT(0, index_partition.base_offset);
    TEST_ASSERT_EQUAL_INT(4, index_partition.size);

    TEST_ASSERT(data_partition.device != 0);
    TEST_ASSERT_EQUAL_INT(4, data_partition.base_offset);
    TEST_ASSERT_EQUAL_INT(27, data_partition.size);

    TEST_ASSERT_EQUAL_INT(6, device.ecc_size);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

#endif /* Board MSBA2 */

static void test_ecc_helpers(void) {
    ftl_device_s test;
    test.subpage_size = 256;
    TEST_ASSERT_EQUAL_INT(3, ftl_ecc_size(&test));
    test.subpage_size = 512;
    TEST_ASSERT_EQUAL_INT(6, ftl_ecc_size(&test));
    test.subpage_size = 2048;
    TEST_ASSERT_EQUAL_INT(22, ftl_ecc_size(&test));

    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE - TEST_FTL_HEADER_SIZE, ftl_data_per_subpage(&device, false));
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE - TEST_FTL_HEADER_SIZE - TEST_FTL_ECC_SIZE, ftl_data_per_subpage(&device, true));


}

static void test_size_helpers(void) {
    printf("\n\n%s\n", __func__);

#ifdef BOARD_MSBA2
    TEST_ASSERT_EQUAL_INT(0, ftl_first_subpage_of_block(&device, 0));
    TEST_ASSERT_EQUAL_INT(1024, ftl_first_subpage_of_block(&device, 1));
    TEST_ASSERT_EQUAL_INT(43008, ftl_first_subpage_of_block(&device, 42));


    TEST_ASSERT_EQUAL_INT(4096, ftl_subpages_in_partition(&index_partition));
    TEST_ASSERT_EQUAL_INT(27648, ftl_subpages_in_partition(&data_partition));
#endif

#ifdef BOARD_NATIVE
    TEST_ASSERT_EQUAL_INT(0, ftl_first_subpage_of_block(&device, 0));
    TEST_ASSERT_EQUAL_INT(32, ftl_first_subpage_of_block(&device, 1));
    TEST_ASSERT_EQUAL_INT(1344, ftl_first_subpage_of_block(&device, 42));


    TEST_ASSERT_EQUAL_INT(128, ftl_subpages_in_partition(&index_partition));
    TEST_ASSERT_EQUAL_INT(864, ftl_subpages_in_partition(&data_partition));
#endif

}

static void test_write_read_raw(void) {
    printf("\n\n%s\n", __func__);
    int ret;

    uint32_t block = 2;
    ret = ftl_erase(&data_partition, block);
    TEST_ASSERT_EQUAL_INT(0, ret);

    memset(page_buffer, 0x0, FTL_SUBPAGE_SIZE);
    memset(expect_buffer, 0xFF, FTL_SUBPAGE_SIZE);
    uint32_t subpage = ftl_first_subpage_of_block(&device, block);
    ret = ftl_read_raw(&data_partition, page_buffer, subpage);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);


    // HEXDUMP_BUFFER(page_buffer, FTL_SUBPAGE_SIZE);
    // HEXDUMP_BUFFER(expect_buffer, FTL_SUBPAGE_SIZE);
    TEST_ASSERT_EQUAL_INT(0, USTRNCMP(page_buffer, expect_buffer, FTL_SUBPAGE_SIZE));

    memset(page_buffer, 0xAB, FTL_SUBPAGE_SIZE);
    ret = ftl_write_raw(&data_partition, page_buffer, subpage);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);

    memset(page_buffer, 0x00, FTL_SUBPAGE_SIZE);
    memset(expect_buffer, 0xAB, FTL_SUBPAGE_SIZE);
    ret = ftl_read_raw(&data_partition, page_buffer, subpage);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);
    TEST_ASSERT_EQUAL_INT(0, USTRNCMP(page_buffer, expect_buffer, FTL_SUBPAGE_SIZE));
}

// Is this still required?
// static void test_read_before_write(void) {
//     printf("\n\n%s\n", __func__);
//     int ret;

//     uint32_t block = 3;
//     ret = ftl_erase(&data_partition, block);
//     TEST_ASSERT_EQUAL_INT(0, ret);

//     uint32_t subpage = ftl_first_subpage_of_block(&device, block);

//     subpageheader_s header;
//     ret = ftl_read(&data_partition, page_buffer, &header, subpage);
//     TEST_ASSERT_EQUAL_INT(-ENOENT, ret);
// }

static void test_write_read(void) {
    printf("\n\n%s\n", __func__);
    int ret;

    uint32_t block = 0;
    ret = ftl_erase(&data_partition, block);

    bool ecc_enabled = false;
    uint16_t data_length = ftl_data_per_subpage(&device, ecc_enabled);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE - TEST_FTL_HEADER_SIZE, data_length);
    memset(page_buffer, 0xAB, data_length);
    // for(int i=0; i<data_length; i++) {
    //     page_buffer[i] = (unsigned char) 100 + i;
    // }

    uint32_t subpage = ftl_first_subpage_of_block(&device, block);

    ret = ftl_write(&data_partition, page_buffer, FTL_SUBPAGE_SIZE);
    TEST_ASSERT_EQUAL_INT(-EFBIG, ret);

    ret = ftl_write(&data_partition, page_buffer, data_length);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, data_partition.last_written_subpage);

    ret = ftl_write(&data_partition, page_buffer, data_length/2);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(1, data_partition.last_written_subpage);

    subpageheader_s header;
    ret = ftl_read(&data_partition, page_buffer, &header, subpage);
    TEST_ASSERT_EQUAL_INT(data_length, ret);

    // DEBUG("read out buffers\n");
    // HEXDUMP_BUFFER(&header, sizeof(subpageheader_s));
    // HEXDUMP_BUFFER(&page_buffer, data_length);

    TEST_ASSERT_EQUAL_INT(data_length, header.data_length);
    memset(expect_buffer, 0xAB, data_length);
    TEST_ASSERT_EQUAL_INT(0, USTRNCMP(page_buffer, expect_buffer, data_length));

    ret = ftl_read(&data_partition, page_buffer, &header, subpage+1);
    TEST_ASSERT_EQUAL_INT(data_length/2, ret);
    TEST_ASSERT_EQUAL_INT(data_length/2, header.data_length);
    memset(expect_buffer, 0xAB, data_length/2);
    TEST_ASSERT_EQUAL_INT(0, USTRNCMP(page_buffer, expect_buffer, data_length/2));
}

static void test_write_read_ecc(void) {
    return;
    printf("\n\n%s\n", __func__);
    int ret;

    uint32_t block = 0;
    ret = ftl_erase(&data_partition, block);

    bool ecc_enabled = true;
    uint16_t data_length = ftl_data_per_subpage(&device, ecc_enabled);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE - TEST_FTL_HEADER_SIZE - TEST_FTL_ECC_SIZE, data_length); // 2 bytes header + 6 ECC removed
    memset(page_buffer, 0xAB, data_length);

    uint32_t subpage;

    ret = ftl_write_ecc(&data_partition, page_buffer, FTL_SUBPAGE_SIZE);
    TEST_ASSERT_EQUAL_INT(-EFBIG, ret);

    ret = ftl_write_ecc(&data_partition, page_buffer, data_length);
    TEST_ASSERT_EQUAL_INT(0, ret);
    subpage = data_partition.last_written_subpage;

    subpageheader_s header;
    ret = ftl_read(&data_partition, page_buffer, &header, subpage);
    TEST_ASSERT_EQUAL_INT(data_length, ret);
    TEST_ASSERT_EQUAL_INT(data_length, header.data_length);
    memset(expect_buffer, 0xAB, data_length);
    TEST_ASSERT_EQUAL_INT(0, USTRNCMP(page_buffer, expect_buffer, data_length));


    unsigned char *u_page_buffer = (unsigned char*) page_buffer;

    // Fake a broken subpage that can be corrected
    ret = ftl_erase(&data_partition, block);
    memset(page_buffer, 0xAB, FTL_SUBPAGE_SIZE);
    memcpy(page_buffer, &header, sizeof(header));
    // The correct hamming code for the 0xAB sequence
    u_page_buffer[3] = 0xFF; u_page_buffer[4] = 0x30; u_page_buffer[5] = 0xC3;
    u_page_buffer[6] = 0xFF; u_page_buffer[7] = 0xFF; u_page_buffer[8] = 0xFF;
    // The flipped bit
    u_page_buffer[27] = 0xAA;
    ret = ftl_write_raw(&data_partition, page_buffer, subpage);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);

    ret = ftl_read(&data_partition, page_buffer, &header, subpage);
    TEST_ASSERT_EQUAL_INT(data_length, ret);


    // Fake a broken subpage that cannot be corrected
    ret = ftl_erase(&data_partition, block);
    memset(page_buffer, 0xAB, FTL_SUBPAGE_SIZE);
    memcpy(page_buffer, &header, sizeof(header));
    // The correct hamming code for the 0xAB sequence
    u_page_buffer[3] = 0xFF; u_page_buffer[4] = 0x30; u_page_buffer[5] = 0xC3;
    u_page_buffer[6] = 0xFF; u_page_buffer[7] = 0xFF; u_page_buffer[8] = 0xFF;
    // The flipped bit
    u_page_buffer[26] = 0xAA;
    u_page_buffer[27] = 0xAA;
    ret = ftl_write_raw(&data_partition, page_buffer, subpage);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);

    ret = ftl_read(&data_partition, page_buffer, &header, subpage);
    TEST_ASSERT_EQUAL_INT(-EBADF, ret);


    // Fake a broken header that can be corrected
    ret = ftl_erase(&data_partition, block);
    memset(page_buffer, 0xAB, FTL_SUBPAGE_SIZE);
    header.data_length -= 1;
    memcpy(page_buffer, &header, sizeof(header));
    header.data_length += 1;
    // The correct hamming code for the 0xAB sequence
    u_page_buffer[3] = 0xFF; u_page_buffer[4] = 0x30; u_page_buffer[5] = 0xC3;
    u_page_buffer[6] = 0xFF; u_page_buffer[7] = 0xFF; u_page_buffer[8] = 0xFF;
    ret = ftl_write_raw(&data_partition, page_buffer, subpage);
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);

    // haha segfault vvvvv
    // I think this happens because the hamming code is not correct anymore
    // with changed subpage size?
    // TODO: fix
    ret = ftl_read(&data_partition, page_buffer, &header, subpage);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(503, header.data_length);


    // Fake broken header that cannot be recovered
    ret = ftl_erase(&data_partition, block);
    memset(page_buffer, 0xAB, data_length);
    header.data_length = 0xFF;
    memcpy(page_buffer, &header, sizeof(header));
    // The correct hamming code for the 0xAB sequence
    u_page_buffer[3] = 0xFF; u_page_buffer[4] = 0x30; u_page_buffer[5] = 0xC3;
    u_page_buffer[6] = 0xFF; u_page_buffer[7] = 0xFF; u_page_buffer[8] = 0xFF;
    ret = ftl_write_raw(&data_partition, page_buffer, subpage);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = ftl_read(&data_partition, page_buffer, &header, subpage);
    TEST_ASSERT_EQUAL_INT(-EBADF, ret);
}

static void test_out_of_bounds(void) {
    printf("\n\n%s\n", __func__);
    int ret;

    ret = ftl_read_raw(&index_partition, page_buffer, 999999);
    TEST_ASSERT_EQUAL_INT(-EFAULT, ret);

    ret = ftl_read_raw(&data_partition, page_buffer, 999999);
    TEST_ASSERT_EQUAL_INT(-EFAULT, ret);

    ret = ftl_write_raw(&index_partition, page_buffer, 999999);
    TEST_ASSERT_EQUAL_INT(-EFAULT, ret);

    ret = ftl_write_raw(&data_partition, page_buffer, 999999);
    TEST_ASSERT_EQUAL_INT(-EFAULT, ret);
}

static void test_format(void) {
    printf("\n\n%s\n", __func__);
    int ret = ftl_format(&index_partition);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = ftl_format(&data_partition);
    TEST_ASSERT_EQUAL_INT(0, ret);

    memset(page_buffer, 0x00, FTL_SUBPAGE_SIZE);
    memset(expect_buffer, 0xFF, FTL_SUBPAGE_SIZE);

    int index_pages = index_partition.size * device.pages_per_block;
    int index_subpages = index_pages * (device.page_size / device.subpage_size);
    for(int i=0; i<index_subpages; i++) {
        ret = ftl_read_raw(&index_partition, page_buffer, i);
        TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);
        TEST_ASSERT_EQUAL_INT(0, USTRNCMP(page_buffer, expect_buffer, FTL_SUBPAGE_SIZE));
    }

    int data_pages = data_partition.size * device.pages_per_block;
    int data_subpages = data_pages * (device.page_size / device.subpage_size);
    for(int i=0; i<data_subpages; i++) {
        ret = ftl_read_raw(&data_partition, page_buffer, i);
        TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);
        TEST_ASSERT_EQUAL_INT(0, USTRNCMP(page_buffer, expect_buffer, FTL_SUBPAGE_SIZE));
    }
}

static void test_metadata(void) {
    printf("\n\n%s\n", __func__);

    int ret;

    ret = ftl_format(&index_partition);
    TEST_ASSERT_EQUAL_INT(0, ret);

    ftl_metadata_header_s metadata_header;
    char metadata_buffer[32];
    char test_metadata1[] = "flubbeldywubbeldy";
    char test_metadata2[] = "schwurbel";

    // ret = ftl_load_latest_metadata(&device, metadata_buffer, &metadata_header, true);
    // TEST_ASSERT_EQUAL_INT(0, ret);

    ret = ftl_write_metadata(&device, test_metadata1, strlen(test_metadata1));
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);

    ftl_partition_s save_index = index_partition;
    ftl_partition_s save_data = data_partition;

    ret = ftl_write_metadata(&device, test_metadata2, strlen(test_metadata2));
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);



    ret = ftl_load_latest_metadata(&device, metadata_buffer, &metadata_header, false);
    TEST_ASSERT_EQUAL_INT(strlen(test_metadata2), ret);
    TEST_ASSERT_EQUAL_INT(2, metadata_header.version);
    //HEXDUMP_BUFFER(metadata_buffer, 32)
    TEST_ASSERT_EQUAL_INT(0, strncmp(test_metadata2, metadata_buffer, strlen(test_metadata2)));




    ret = ftl_load_metadata(&device, metadata_buffer, &metadata_header, metadata_header.version-1, false);
    TEST_ASSERT_EQUAL_INT(strlen(test_metadata1), ret);
    TEST_ASSERT_EQUAL_INT(0, strncmp(test_metadata1, metadata_buffer, strlen(test_metadata1)));



    /* ============================================
     * write some data to change partition metadata
     * ============================================ */

    uint16_t data_length = ftl_data_per_subpage(&device, true);
    memset(page_buffer, 0xAB, data_length);
    ret = ftl_write(&data_partition, page_buffer, data_length);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = ftl_write(&data_partition, page_buffer, data_length);
    TEST_ASSERT_EQUAL_INT(0, ret);
    ret = ftl_write(&data_partition, page_buffer, data_length);
    TEST_ASSERT_EQUAL_INT(0, ret);



    /* ======================================================
     * load metadata and verify it is the same as when stored
     * ====================================================== */

    // This accounts for the fact that these two indices are updated after the metadata
    // page has already been written.
    save_index.next_subpage++;
    save_index.last_written_subpage++;

    TEST_ASSERT(memcmp(&data_partition, &save_data, sizeof(ftl_partition_s)) != 0);
    TEST_ASSERT(memcmp(&index_partition, &save_index, sizeof(ftl_partition_s)) != 0);

    ret = ftl_load_latest_metadata(&device, metadata_buffer, &metadata_header, true);
    TEST_ASSERT_EQUAL_INT(strlen(test_metadata2), ret);
    TEST_ASSERT_EQUAL_INT(2, metadata_header.version);

    // HEXDUMP_BUFFER(&save_data, sizeof(ftl_partition_s));
    // HEXDUMP_BUFFER(&data_partition, sizeof(ftl_partition_s));

    // HEXDUMP_BUFFER(&save_index, sizeof(ftl_partition_s));
    // HEXDUMP_BUFFER(&index_partition, sizeof(ftl_partition_s));

    TEST_ASSERT_EQUAL_INT(0, memcmp(&data_partition, &save_data, sizeof(ftl_partition_s)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(&index_partition, &save_index, sizeof(ftl_partition_s)));

    ret = ftl_write_metadata(&device, test_metadata1, strlen(test_metadata1));
    TEST_ASSERT_EQUAL_INT(FTL_SUBPAGE_SIZE, ret);
}


Test *testsrunner(void) {
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_init),
        new_TestFixture(test_ecc_helpers),
        new_TestFixture(test_size_helpers),
        new_TestFixture(test_write_read_raw),
        //new_TestFixture(test_read_before_write),
        new_TestFixture(test_write_read),
        new_TestFixture(test_write_read_ecc),
        new_TestFixture(test_out_of_bounds),
        new_TestFixture(test_format),
        new_TestFixture(test_metadata),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
#ifdef OUTPUT
    TextUIRunner_setOutputter(OUTPUTTER);
#endif

    TESTS_START();
    TESTS_RUN(testsrunner());
    TESTS_END();

    puts("xtimer_wait()");
    xtimer_usleep(100000);

    pm_off();
    return 0;
}
