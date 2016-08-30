/*
 * Copyright (C) 2015 Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_storage_ftl Flash Translation Layer
 * @ingroup     sys_storage
 *
 * @brief       Flash Translation Layer (FTL) intended to provide a uniform interface
 *              to flash-based data storage devices.
 *

TOOD: Move this. The RIOT FTL divides
each page on the flash device into one or more so-called subpages to reduce the size of
the required read/write buffers. Each subpage is optionally attached with an error
correction code.



Initialization
==============

In order to make use of the FTL, it needs to be initialized with the parameters
particular to the flash device you wish to use, i.e.:

- Its page size
- The amount of pages in each erase block
- The total amount of pages on the device (or as many as you wish to address through the FTL)

In addition, a subpage size fitting the storage device needs to be chosen, knowing that
the FTL will allocate a read/write buffer of the same size. There are some constraints
to keep in mind when choosing the subpage size:

- Size of the ECC scales with the subpage size (see #ftl_ecc_size).
- The target flash storage must support at least (page size)/(subpage size) sequential
  writes to the same page (without having to erase the resident block).
- Since each subpage is preceded by a dedicated fixed-size header, the metadata to data ratio will
  gets worse with decreasing subpage size.

Lastly, the FTL needs to be given three callbacks for reading/writing raw data and for
erasing a single block. For the required signature please see the write/read/erase members
of #ftl_device_s.


Example
-------

Let's assume a device with a page size of 2048 bytes and a blocksize of 1024 pages,
supporting 4 sequential writes to the same page. The total capacity of the device is 8GiB
resulting in 4,194,304 pages and 4096 blocks.

For this device we would pick a subpage size of 2048 / 4 = 512 bytes. The initialization
would then look as follows:

    ftl_device_s device;

    device.write = ...
    device.read = ...
    device.erase = ...

    device.page_size = 2048;
    device.subpage_size = 512;
    device.pages_per_block = 1024;
    device.total_pages = 4194304;

    int ret = ftl_init(&device);
    if(ret != 0) {
        fprintf(stderr, "Error initializing the device\n");
    }



Usage
=====

After the device is initialized, we can proceed with reading/writing some data. As an
example we will first format the index partition of the device and proceed to write
some data:

    int ret = ftl_format(&device.index_partition);
    if(ret != 0) { Error }

    char buffer[512]; // Allocate a buffer which can hold a page
    memset(subpage, 0xAB, 512); // Generate some data to write

    int target_subpage = 1234;
    ret = ftl_write(&device.index_partition, buffer, target_subpage);
    if(ret != 0) { Error }

It is very important to note that the above will __NOT__ write 512 bytes to the subpage,
since each of them is equipped with at least a subpage header (3 bytes) and an optional
ECC. As a result, the above will only write 509 bytes of data. This can be verified
as follows:

    subpageheader_s header;
    ret = ftl_read(&device.index_partition, buffer, &header, target_subpage);
    if(ret != 0) { Error }

    assert(header.data_length == 509);

Writing data with an ECC works similarly:

    ret = ftl_write_ecc(&device.index_partition, buffer, target_subpage);
    if(ret != 0) { Error }

    ret = ftl_read(&device.index_partition, buffer, &header, target_subpage);
    if(ret != 0) { Error }

    assert(header.data_length == 503);

To know in advance how much data can be written per subpage, one can use the
#ftl_data_per_subpage function:

    int data_length = ftl_data_per_subpage(&device, false);
    int data_length_with_ecc = ftl_data_per_subpage(&device, true);

    assert(data_length == 509);
    assert(data_length_with_ecc == 503);

ECC
===

TODO: write about ecc

 *
 *
 * @{
 * @author      Lucas Jenß <lucas@x3ro.de>
 */

#ifndef _FS_FTL_H
#define _FS_FTL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name Data Types
 * @{
 */

// This is needed here since `ftl_partition_s` and `ftl_device_s` reference each other
struct ftl_device_s;

/**
 * @brief A partition on a device managed by the FTL.
 */
typedef struct ftl_partition_s {
    struct ftl_device_s *device;    //!< The device on which the partition is located
    uint32_t base_offset;           //!< Zero-indexed absolute offset of the partition __in blocks__
    uint32_t size;                  //!< Size of the partition __in blocks__

    /**
     * The next page to be written by the FTL, or in other words,
     * the first known free page
     */
    uint32_t next_subpage;

    /**
     * Page that was last written in this partition. Must not necessarily be next_page-1,
     * because the previous block may have been faulty
     */
    uint32_t last_written_subpage;

    uint32_t erased_until;          //!< Pointer to the last erased block
    uint32_t free_until;            //!< Pointer to the last known free block
} ftl_partition_s;

/**
 * @brief A device managed by the FTL.
 */
typedef struct ftl_device_s {
    uint32_t metadata_version;

    uint32_t total_pages;     //!< Total amount of pages configured for the device
    uint16_t page_size;       //!< Page size configured for the device
    uint16_t subpage_size;    //!< Size of a single subpage in bytes
    uint16_t pages_per_block; //!< Amount of pages inside an erase segment (block)
    uint8_t ecc_size;         //!< Size of the ECC determined for device's subpage size
    bool is_initialized;      //!< Whether #ftl_init was successfully called

    uint8_t partition_count;      //!< Number of partitions allocated for this device
    ftl_partition_s **partitions; //!< Array of partitions allocated for this device

    unsigned char *_subpage_buffer; //!< Buffer for subpage read/write operations.
    unsigned char *_ecc_buffer;     //!< Buffer for ECC calculation

    /**
     * Callback which must write a data buffer of the given length to a certain offset
     * inside a page.
     */
    int (*_write)(const unsigned char *buffer,
                              uint32_t page,
                              uint32_t offset,
                              uint16_t length);

    /**
     * Callback which must read a data segment of the given length from a certain offset
     * inside a page and writes it to the given data buffer.
     */
    int (*_read)(unsigned char *buffer,
                       uint32_t page,
                       uint32_t offset,
                       uint16_t length);

    /**
     * Callback which must erase the given block.
     */
    int (*_erase)(uint32_t block);


    /**
     * Callback which must erase "length" blocks starting at "start_block".
     */
    int (*_bulk_erase)(uint32_t start_block, uint32_t length);
} ftl_device_s;

/**
 * @brief Header which precedes every subpage not written in raw mode.
 *
 * The header may be followed by an ECC of the size defined in ::ftl_device_s's ecc_size.
 * TODO adjust according to thesis
 */
typedef struct __attribute__((__packed__)) subpageheader_s {
    unsigned int data_length:16;    //!< Length of the data written to this subpage
    unsigned int ecc_enabled:1;     //!< If the header is directly followed by an ECC
    unsigned int reserved:7;        //!< Reserved for future use
} subpageheader_s;

/**
 * The ftl_metadata_header is always written at the start of a metadata subpage,
 * followed by the the following (in this order)
 *
 *    - `$partition_count * sizeof(ftl_partition_s)` bytes of partition information
 *    - `$foreign_metadata_length` bytes of metadata belonging to the OSL, or another
 *      system built on top of the FTL.
 */
typedef struct __attribute__((__packed__)) ftl_metadata_header_s {
    /**
     * Sequential version number to decide whether any given metadata subpage is more
     * recent than another
     */
    uint32_t version;

    /**
     * Foreign metadata (bytes) stored by the storage layer operating above the FTL
     */
    uint16_t foreign_metadata_length;

    uint8_t partition_count; //!< Number of known partitions in this metadata chunk
} ftl_metadata_header_s;

/** @} */



/**
 * @name Functions operating on the device
 * @{
 */

/**
 * Initializes instance of the FTL based on the passed device configuration
 * @param  device configuration
 *
 * @return        0 on success
 * @return        -EFAULT if the device is too small for the configured index partition
 * @return        -ENOMEM if any buffer could not be allocated
 *
 */
int ftl_init(ftl_device_s *device);

/** @} */



/**
 * @name Functions operating on partitions
 * @{
 */

/**
 * Reads a single subpage
 * @param  partition to be read from
 * @param  buffer    to be written to
 * @param  header    Buffer to which the subpage header data will be written
 * @param  subpage   Index of the subpage to read
 *
 * @return           0 on success
 * @return           -EBADF if the page was corrupt beyond repair using ECC
 * @return           -ENOENT if trying to read from a page which hasn't been written to
 * @return           Any error code that #ftl_read_raw may return
 */
int ftl_read(const ftl_partition_s *partition,
             unsigned char *buffer,
             subpageheader_s *header,
             uint32_t subpage);

/**
 * @brief Writes a single subpage, including its header, without error correction
 *
 * @note This function will __NOT__ write "subpage size" bytes of data, since each
 *       each subpage includes a dedicated header. Please use #ftl_data_per_subpage
 *       to determine how much data can be written per subpage.
 *
 * @param  partition   to be written to
 * @param  buffer      to be written from
 * @param  subpage     Index of the subpage to write to
 * @param  data_length Amount of bytes to write to the subpage
 * @return             0 on success
 * @return             -EFBIG if the given data does not fit into a subpage
 */
int ftl_write(ftl_partition_s *partition,
              const unsigned char *buffer,
              uint16_t data_length);

/**
 * @brief Same as #ftl_write, but adds an error correction code (ECC) to the written subpage
 *
 * @note This function will __NOT__ write "subpage size" bytes of data, since each
 *       each subpage includes a dedicated header. In addition, this function also
 *       appends an error correction code (ECC). Please use #ftl_data_per_subpage
 *       (with the `ecc_enabled` flag set) to determine how much data can be written
 *       per subpage.
 *
 * @see #ftl_write
 */
int ftl_write_ecc(ftl_partition_s *partition,
                  const unsigned char *buffer,
                  uint16_t data_length);


/**
 * @brief Erases all blocks in a given partition
 * @param[in]  partition
 * @return     0 or the result of any failed erase operation (#ftl_erase)
 */
int ftl_format(const ftl_partition_s *partition);

/**
 * @brief Erases a single block of the given partition
 * @param  partition
 * @param  block     Block to be erased
 *
 * @return    0 on success
 * @return    -EFAULT if an invalid subpage is given
 */
int ftl_erase(const ftl_partition_s *partition, uint32_t block);

/**
 * @brief Writes the buffer to the given subpage __without a subpage header__ (for automatic
 * header handling use #ftl_write or #ftl_write_ecc.
 *
 * @param[in]  partition    which is to be written
 * @param[in]  buffer       which should be written. It must be at least the size
 *                          of a subpage.
 * @param[in]  subpage      that should be written
 *
 * @return 0 or an error code
 */
int ftl_write_raw(const ftl_partition_s *partition,
                  const unsigned char *buffer,
                  uint32_t subpage);

/**
 * @brief Reads a single subpage from the given partition into the buffer, __including
 * the subpage header__ (for automatic header handling use #ftl_read)
 *
 * @param[in]  partition from which to read
 * @param[out] buffer    to which subpage should be written. It must be
 *                       at least the size of a subpage.
 * @param[in]  subpage   to read
 *
 * @return    0 on success
 * @return    -EFAULT if an invalid subpage is given
 */
int ftl_read_raw(const ftl_partition_s *partition,
                 unsigned char *buffer,
                 uint32_t subpage);

/** @} */


/**
 * @name Functions for metadata handling
 * @{
 */


int ftl_load_metadata_page_with_version(ftl_device_s *device,
                                        uint32_t version,
                                        uint32_t *source_page);

int ftl_write_metadata(ftl_device_s *device,
                       const void *metadata,
                       uint16_t length);

int32_t ftl_load_latest_metadata(ftl_device_s *device,
                                 void *buffer,
                                 ftl_metadata_header_s *header,
                                 bool set_ftl_state);

int32_t ftl_load_metadata(ftl_device_s *device,
                          void *buffer,
                          ftl_metadata_header_s *header,
                          uint32_t version,
                          bool set_ftl_state);

/** @} */

/**
 * @name Utility functions without side-effects
 * @{
 */

/**
 * @brief Computes the required ECC size for the given device configuration
 * @param  device
 * @return        ECC size
 */
uint8_t ftl_ecc_size(const ftl_device_s *device);

/**
 * @brief Computes the first subpage of a given block on the device (absolute, not per-partition)
 * @param  device
 * @param  block
 * @return        The absolute index of the first subpage in the given block
 */
uint32_t ftl_first_subpage_of_block(const ftl_device_s *device, uint32_t block);

/**
 * @brief Computes the amount of bytes that can be stored per subpage for the given device
 * @param  device
 * @param  ecc_enabled Whether ECC is enabled or not for this page
 * @return             Number of bytes that fit into the subpage
 */
uint16_t ftl_data_per_subpage(const ftl_device_s *device, bool ecc_enabled);

/**
 * @brief Computes the number of subpages held by the given partition
 * @param   partition
 * @return  Number of subpages
 */
uint32_t ftl_subpages_in_partition(const ftl_partition_s *partition);

bool ftl_is_initialized(const ftl_device_s *device);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/** @} */
