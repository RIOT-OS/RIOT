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
 * @brief       Implementation of Flash Translation Layer (FTL)
 * @author      Lucas Jenß <lucas@x3ro.de>
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "storage/ftl.h"
#include "ecc/hamming256.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define MYDEBUG(...) DEBUG("%s: ", __func__); \
                     DEBUG(__VA_ARGS__)

#define HEXDUMP_BUFFER(buffer, size)    for(int i=0; i<size; i++) { \
                                    printf("%02x ", ((unsigned char*) buffer)[i]); \
                                    if((i+1)%32 == 0) { printf("\n"); } \
                                } \
                                printf("\nbuffer ^^^^^\n");

uint32_t uint32_log2(uint32_t inVal) {
    if(inVal == 0)
        return 0;
    uint32_t tempOut = 0;
    if(inVal >= (1 << 16)) { inVal >>= 16; tempOut += 16; }
    if(inVal >= (1 <<  8)) { inVal >>=  8; tempOut +=  8; }
    if(inVal >= (1 <<  4)) { inVal >>=  4; tempOut +=  4; }
    if(inVal >= (1 <<  2)) { inVal >>=  2; tempOut +=  2; }
    if(inVal >= (1 <<  1)) {               tempOut +=  1; }
    return tempOut;
}

uint64_t ftl_device_capacity(const ftl_device_s *device) {
    return device->total_pages * device->page_size;
}

uint32_t ftl_device_blocksize(const ftl_device_s *device) {
    return device->pages_per_block * device->page_size;
}

uint32_t ftl_subpages_in_partition(const ftl_partition_s *partition) {
    return partition->size * partition->device->pages_per_block *
           (partition->device->page_size / partition->device->subpage_size);
}

uint8_t ftl_ecc_size(const ftl_device_s *device) {
    // This would be the result for the "good" Hamming algorithm
    //return (uint8_t) uint32_log2(device->subpage_size * 8) * 2;

    // The current Hamming code implememntation unfortunately only supports
    // 256 byte blocks, each with a 22 bit code.
    uint8_t bitsize = (device->subpage_size / 256) * 22;

    uint8_t bytesize = bitsize / 8;
    if(bitsize % 8 > 0) {
        bytesize++;
    }

    return bytesize;
}

uint8_t ftl_subpage_mod(const ftl_device_s *device, uint32_t subpage) {
    return subpage % (device->page_size / device->subpage_size);
}

uint32_t ftl_first_subpage_of_block(const ftl_device_s *device, uint32_t block) {
    return block * device->pages_per_block * (device->page_size / device->subpage_size);
}

uint32_t ftl_subpage_to_page(const ftl_partition_s *partition, uint32_t subpage) {
    return (partition->base_offset * partition->device->pages_per_block) +
            subpage / (partition->device->page_size / partition->device->subpage_size);
}

uint16_t ftl_data_per_subpage(const ftl_device_s *device, bool ecc_enabled) {
    uint16_t size = device->subpage_size - sizeof(subpageheader_s);

    if(ecc_enabled) {
        size -= device->ecc_size;
    }

    return size;
}

bool ftl_is_initialized(const ftl_device_s *device) {
    return device->is_initialized;
}


int ftl_init(ftl_device_s *device) {
    // if(ftl_device_capacity(device) < FTL_INDEX_SIZE) {
    //     return -EFAULT;
    // }

    // uint32_t blocksize = ftl_device_blocksize(device);
    // device->index_partition.device = device;
    // device->index_partition.base_offset = 0;
    // device->index_partition.size = FTL_INDEX_SIZE / blocksize;
    // if((FTL_INDEX_SIZE % blocksize) > 0) {
    //     device->index_partition.size += 1;
    // }

    // The data partition starts directly after the index and spans the rest
    // of the device.
    // device->data_partition.device = device;
    // device->data_partition.base_offset = device->index_partition.size;
    // device->data_partition.size = (device->total_pages / device->pages_per_block) -
    //                                device->index_partition.size;

    //device->ecc_size = ftl_ecc_size(device);
    //device->_subpage_buffer = malloc(device->subpage_size);
    // if(device->_subpage_buffer == 0)  {
    //     return -ENOMEM;
    // }

    // device->_ecc_buffer = malloc(device->ecc_size);
    // if(device->_ecc_buffer == 0)  {
    //     return -ENOMEM;
    // }
    //
    //

    // TODO: return an error if the supplied paritions overlap.

    // TODO :D
    device->is_initialized = true;
    device->metadata_version = 0;
    return 0;
}

int ftl_erase(const ftl_partition_s *partition, uint32_t block) {
    uint32_t absolute_block = partition->base_offset + block;
    uint32_t block_capacity = partition->device->total_pages / partition->device->pages_per_block;
    if(block >= partition->size || absolute_block >= block_capacity) {
        return -EFAULT;
    }

    MYDEBUG("Erasing block=%lu\n", (unsigned long) absolute_block);

    return partition->device->_erase(absolute_block);
}

int ftl_format(const ftl_partition_s *partition) {
    uint32_t blocks = partition->size;
    int ret = 0;
    for(uint32_t i=0; i<blocks; i++) {
        ret = ftl_erase(partition, i);
        if(ret != 0) {
            return ret;
        }
    }
    return ret;
}

int ftl_read_raw(const ftl_partition_s *partition,
                     unsigned char *buffer,
                     uint32_t subpage) {

    if(subpage >= ftl_subpages_in_partition(partition)) {
        return -EFAULT;
    }

    uint32_t page = ftl_subpage_to_page(partition, subpage);
    MYDEBUG("Reading from page %lu, offset=%lu, size=%lu\n", (unsigned long) page,
        (unsigned long) ftl_subpage_mod(partition->device, subpage) * partition->device->subpage_size,
        (unsigned long) partition->device->subpage_size);

    return partition->device->_read(buffer,
                                   page,
                                   ftl_subpage_mod(partition->device, subpage) * partition->device->subpage_size,
                                   partition->device->subpage_size);
}

int ftl_write_raw(const ftl_partition_s *partition,
                     const unsigned char *buffer,
                     uint32_t subpage) {

    // printf("subpage: %lu, subpages in partition: %lu\n", (unsigned long) subpage,
    //     (unsigned long) ftl_subpages_in_partition(partition));

    if(subpage >= ftl_subpages_in_partition(partition)) {
        return -EFAULT;
    }

    uint32_t page = ftl_subpage_to_page(partition, subpage);
    MYDEBUG("Writing to page %lu, offset=%lu, size=%lu\n", (unsigned long) page,
        (unsigned long) ftl_subpage_mod(partition->device, subpage) * partition->device->subpage_size,
        (unsigned long) partition->device->subpage_size);

     return partition->device->_write(
        buffer,
        page,
        ftl_subpage_mod(partition->device, subpage) * partition->device->subpage_size,
        partition->device->subpage_size);
}


int ftl_write(ftl_partition_s *partition,
                      const unsigned char *buffer,
                      uint16_t data_length) {

    subpageheader_s header;
    if(data_length > ftl_data_per_subpage(partition->device, false)) {
        return -EFBIG;
    }

    MYDEBUG("Writing to subpage %d\n", (int) partition->next_subpage);

    header.data_length = data_length;
    header.ecc_enabled = 0;
    header.reserved = 0;



    memcpy(partition->device->_subpage_buffer, &header, sizeof(header));
    memcpy(partition->device->_subpage_buffer + sizeof(header), buffer, data_length);

    // MYDEBUG("data_length: %d\n", (int) header.data_length);
    // HEXDUMP_BUFFER(partition->device->_subpage_buffer, partition->device->subpage_size);


    int ret = ftl_write_raw(partition, partition->device->_subpage_buffer, partition->next_subpage);
    if(ret < 0) {
        return ret;
    }

    partition->last_written_subpage = partition->next_subpage;
    partition->next_subpage++;
    return 0;
}

int _prepare_subpage_buffer_with_ecc(ftl_partition_s *partition, uint16_t data_length) {
    subpageheader_s header;
    header.data_length = data_length;
    header.ecc_enabled = 1;
    header.reserved = 0;

    memcpy(partition->device->_subpage_buffer, &header, sizeof(header));

    hamming_compute256x((uint8_t*)partition->device->_subpage_buffer,
                        partition->device->subpage_size,
                        (uint8_t*)partition->device->_ecc_buffer);

    memcpy(partition->device->_subpage_buffer + sizeof(header),
           partition->device->_ecc_buffer,
           partition->device->ecc_size);

    return 0;
}

int ftl_write_ecc(ftl_partition_s *partition,
                      const unsigned char *buffer,
                      uint16_t data_length) {

    MYDEBUG("l: %d, other: %d\n", data_length, partition->device->subpage_size - ftl_data_per_subpage(partition->device, true));
    if(data_length >
       ftl_data_per_subpage(partition->device, true)) {
        return -EFBIG;
    }

    MYDEBUG("Writing to subpage %d w/ ECC\n", (int) partition->next_subpage);


    // Page buffer needs to be wiped because of the ECC calculation
    memset(partition->device->_subpage_buffer, 0x00, partition->device->subpage_size);

    //memcpy(partition->device->_subpage_buffer, &header, sizeof(header));

    memcpy(partition->device->_subpage_buffer + sizeof(subpageheader_s) + partition->device->ecc_size,
           buffer, data_length);

    _prepare_subpage_buffer_with_ecc(partition, data_length);

    // hamming_compute256x((uint8_t*)partition->device->_subpage_buffer,
    //                     partition->device->subpage_size,
    //                     (uint8_t*)partition->device->_ecc_buffer);

    // memcpy(partition->device->_subpage_buffer + sizeof(header),
    //        partition->device->_ecc_buffer,
    //        partition->device->ecc_size);

    int ret = ftl_write_raw(partition, partition->device->_subpage_buffer, partition->next_subpage);
    if(ret < 0) {
        printf("write raw failed\n");
        return ret;
    }

    partition->last_written_subpage = partition->next_subpage;
    partition->next_subpage++;
    return 0;
}

int ftl_read(const ftl_partition_s *partition,
                     unsigned char *buffer,
                     subpageheader_s *header,
                     uint32_t subpage) {

    int ret = ftl_read_raw(partition, partition->device->_subpage_buffer, subpage);
    if(ret < 0) {
        return ret;
    }

    memcpy(header, partition->device->_subpage_buffer, sizeof(subpageheader_s));
    MYDEBUG("WTF %d\n", __LINE__);
    //HEXDUMP_BUFFER(&partition->device->_subpage_buffer, (int)sizeof(partition->device->_subpage_buffer));

    bool uninitialized = true;
    for(unsigned int i=0; i < sizeof(subpageheader_s); i++) {
        if(((unsigned char*) header)[i] != 0xff) {
            uninitialized = false;
            break;
        }
    }
    MYDEBUG("WTF %d\n", __LINE__);

    if(uninitialized) {
        return -ENOENT;
    }

    if(header->data_length > partition->device->subpage_size) {
        return -EBADF;
    }
    MYDEBUG("WTF %d\n", __LINE__);

    unsigned char *data_buffer = partition->device->_subpage_buffer;
    unsigned char *ecc_buffer = partition->device->_ecc_buffer;
    MYDEBUG("data length: %d\n", header->data_length);
    if(header->ecc_enabled) {
        uint8_t ecc_size = partition->device->ecc_size;
        memcpy(ecc_buffer, data_buffer + sizeof(subpageheader_s), ecc_size);
        memset(data_buffer + sizeof(subpageheader_s), 0x00, ecc_size);
        uint8_t result = hamming_verify256x((uint8_t*) data_buffer,
                                            partition->device->subpage_size,
                                            (uint8_t*) ecc_buffer);

        MYDEBUG("WTF %d\n", __LINE__);
        if(result != Hamming_ERROR_NONE && result != Hamming_ERROR_SINGLEBIT) {
            return -EBADF;
        } else if(result == Hamming_ERROR_SINGLEBIT) {
            // We need to update the header in case that the flipped bit was in there
            memcpy(header, partition->device->_subpage_buffer, sizeof(subpageheader_s));
        }

        data_buffer += ecc_size;
    }

    MYDEBUG("WTF %d\n", __LINE__);

    data_buffer += sizeof(subpageheader_s);
    MYDEBUG("WTF %d\n", __LINE__);
    if(buffer != 0) {
        MYDEBUG("data length: %d\n", header->data_length);
        memcpy(buffer, data_buffer, header->data_length);
        MYDEBUG("WTF %d\n", __LINE__);
    }
    MYDEBUG("WTF %d\n", __LINE__);

    return header->data_length;
}



/* ================
 * Metadata storage
 * ================ */

int ftl_write_metadata(ftl_device_s *device, const void *metadata, uint16_t length) {
    if(sizeof(ftl_metadata_header_s) +
       device->partition_count * sizeof(ftl_partition_s) +
       length > ftl_data_per_subpage(device, true)) {

        return -EFBIG;
    }

    ftl_metadata_header_s header = {
        .version = device->metadata_version+1,
        .foreign_metadata_length = length,
        .partition_count = device->partition_count
    };

    //printf("version: %x length %x count %x\n", header.version, header.foreign_metadata_length, header.partition_count);

    int offset = sizeof(subpageheader_s) + device->ecc_size;
    memset(device->_subpage_buffer, 0, device->subpage_size);

    memcpy(device->_subpage_buffer + offset, &header, sizeof(ftl_metadata_header_s));
    offset += sizeof(ftl_metadata_header_s);

    for(unsigned int i=0; i<device->partition_count; i++) {
        memcpy(device->_subpage_buffer + offset, device->partitions[i], sizeof(ftl_partition_s));
        offset += sizeof(ftl_partition_s);
    }

    memcpy(device->_subpage_buffer + offset, metadata, length);
    offset += length;

    ftl_partition_s *metadata_partition = device->partitions[0];
    _prepare_subpage_buffer_with_ecc(metadata_partition, offset+1);

    //HEXDUMP_BUFFER(device->_subpage_buffer, device->subpage_size);

    int ret = ftl_write_raw(metadata_partition, device->_subpage_buffer, metadata_partition->next_subpage);
    metadata_partition->next_subpage++;
    device->metadata_version++;
    return ret;
}

// TODO: use binary search based mechanism
int ftl_load_metadata_page_with_version(ftl_device_s *device, uint32_t version, uint32_t *source_page) {
    ftl_partition_s *metadata_partition = device->partitions[0];

    ftl_metadata_header_s metadata_header;
    subpageheader_s subpage_header;
    int ret;
    for(uint32_t i=0; i<ftl_subpages_in_partition(metadata_partition); i++) {
        ret = ftl_read(metadata_partition, NULL, &subpage_header, i);

        //printf("reading page %d", i);
        //HEXDUMP_BUFFER(device->_subpage_buffer, device->subpage_size);

        // first page was not initialized, assuming empty partition
        if(ret == -ENOENT && i==0) {
            return -1;
        }

        //printf("xxx subpage %d error %d\n", i, ret);
        if(ret < 0) {
            // In case of error reading, return the latest valid version
            ret = ftl_read(metadata_partition, NULL, &subpage_header, i-1);
            //printf("subpage %d error %d\n", i-1, ret);
            assert(ret >= 0);
            *source_page = i-1;
            return 2;
        }

        memcpy(&metadata_header, device->_subpage_buffer + sizeof(subpageheader_s) + device->ecc_size, sizeof(metadata_header));
        if(metadata_header.version == version) {
            *source_page = i;
            return 1;
        }
    }

    return -1;
}

int32_t ftl_load_latest_metadata(ftl_device_s *device, void *buffer, ftl_metadata_header_s *header, bool set_ftl_state) {
    return ftl_load_metadata(device, buffer, header, (2<<31)-1, set_ftl_state);
}

int32_t ftl_load_metadata(ftl_device_s *device, void *buffer, ftl_metadata_header_s *header, uint32_t version, bool set_ftl_state) {
    uint32_t source_subpage;
    int ret = ftl_load_metadata_page_with_version(device, version, &source_subpage);
    //printf("ret %d version %d\n", ret, header->version);
    if(ret < 0) {
        return ret;
    }

    //HEXDUMP_BUFFER(device->_subpage_buffer, device->subpage_size);

    int offset = sizeof(subpageheader_s) + device->ecc_size;
    memcpy(header, device->_subpage_buffer + offset, sizeof(ftl_metadata_header_s));
    offset += sizeof(ftl_metadata_header_s);

    if(set_ftl_state) {
        //printf("loading %d partitions\n", header->partition_count);
        for(unsigned int i=0; i<header->partition_count; i++) {
            memcpy(device->partitions[i], device->_subpage_buffer + offset, sizeof(ftl_partition_s));
            device->partitions[i]->device = device;
            offset += sizeof(ftl_partition_s);
        }
        assert(device->_subpage_buffer != 0);

        // We need to account for the fact that the metadata partition's next_subpage was
        // incremented after
        device->partitions[0]->next_subpage = source_subpage+1;
        device->partitions[0]->last_written_subpage = source_subpage;
    } else {
        offset += sizeof(ftl_partition_s) * header->partition_count;
    }

    memcpy(buffer, device->_subpage_buffer + offset, header->foreign_metadata_length);
    return header->foreign_metadata_length;
}


