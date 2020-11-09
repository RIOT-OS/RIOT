/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riotboot_flashwrite riotboot flash writer
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       riotboot flash writing module
 *
 * This module provides an API to reliably write a firmware image to flash.
 *
 * The API is similar to stream hashing functions:
 *
 * 1. initialize state structure using riotboot_flashwrite_init()
 * 2. put some data using riotboot_flashwrite_putbytes()
 * 3. repeat 2. until all data has been received
 * 4. finish update using riotboot_flashwrite_finish()
 *
 * The module will *not* automatically reboot after an image has been
 * successfully written.
 *
 * Under the hood, the module tries to abstract page sizes for writing the image
 * to flash. Verification of the image is left to the caller.
 * If the data is not correctly written, riotboot_put_bytes() will
 * return -1.
 *
 * The module makes sure that at no point in time an invalid image is bootable.
 * The algorithm for that makes use of the bootloader verifying checksum and
 * works as follows:
 *
 * 1. erase first block (making its checksum invalid)
 * 2. write image starting at second block
 * 3. write first block
 *
 * When using raw mode is used with this module, the need to buffer a full
 * flashpage page is removed, instead it must buffer two times the
 * FLASHPAGE_WRITE_BLOCK_SIZE. One is used to buffer the current write block,
 * the other buffers the first chunk (offset zero, page zero). This first
 * chunk is written when finalizing the flash operation. The minimal size for
 * RIOTBOOT_FLASHPAGE_BUFFER_SIZE is 4, at least the riotboot magic number must
 * fit into this and FLASHPAGE_SIZE must be a multiple of
 * RIOTBOOT_FLASHPAGE_BUFFER_SIZE
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifndef RIOTBOOT_FLASHWRITE_H
#define RIOTBOOT_FLASHWRITE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "riotboot/slot.h"
#include "periph/flashpage.h"

/**
 * @brief Enable/disable raw writes to flash
 */
#ifndef CONFIG_RIOTBOOT_FLASHWRITE_RAW
#define CONFIG_RIOTBOOT_FLASHWRITE_RAW  1
#endif

/**
 * @brief Intermediate buffer size for firmware image data
 */
#if CONFIG_RIOTBOOT_FLASHWRITE_RAW

#if (FLASHPAGE_WRITE_BLOCK_SIZE < 4)
#define RIOTBOOT_FLASHPAGE_BUFFER_SIZE 4
#else
#define RIOTBOOT_FLASHPAGE_BUFFER_SIZE FLASHPAGE_WRITE_BLOCK_SIZE
#endif

#else /* CONFIG_RIOTBOOT_FLASHWRITE_RAW */

#define RIOTBOOT_FLASHPAGE_BUFFER_SIZE FLASHPAGE_SIZE

#endif /* !CONFIG_RIOTBOOT_FLASHWRITE_RAW */

/**
 * @brief Extra attributes required for the firmware intermediate buffer
 */
#define RIOTBOOT_FLASHPAGE_BUFFER_ATTRS \
    __attribute__((aligned(FLASHPAGE_WRITE_BLOCK_ALIGNMENT)))

/**
 * @brief   firmware update state structure
 *
 * @note    @ref FLASHPAGE_SIZE can be very large on some platforms, don't place
 *          this struct on the stack or increase the thread stack size
 *          accordingly.
 */
typedef struct {
    int target_slot;                        /**< update targets this slot     */
    size_t offset;                          /**< update is at this position   */
    unsigned flashpage;                     /**< update is at this flashpage  */

    /**
     * @brief flash writing buffer
     */
    uint8_t RIOTBOOT_FLASHPAGE_BUFFER_ATTRS
        flashpage_buf[RIOTBOOT_FLASHPAGE_BUFFER_SIZE];
#if CONFIG_RIOTBOOT_FLASHWRITE_RAW || DOXYGEN
    /**
     * @brief Buffer for the first chunk containing the checksum when using
     *        FLASHWRITE_RAW
     */
    uint8_t RIOTBOOT_FLASHPAGE_BUFFER_ATTRS
        firstblock_buf[RIOTBOOT_FLASHPAGE_BUFFER_SIZE];
#endif
} riotboot_flashwrite_t;

/**
 * @brief Amount of bytes to skip at initial write of first page
 */
#define RIOTBOOT_FLASHWRITE_SKIPLEN     sizeof(RIOTBOOT_MAGIC)

/**
 * @brief   Initialize firmware update (raw version)
 *
 * Allows setting the initial offset to @p offset, which would mean that the
 * first bytes passed in via @ref riotboot_flashwrite_putbytes() will be
 * written to (slot_start + offset).
 *
 * @note offset *should* be <= FLASHPAGE_SIZE, otherwise the results are
 *       undefined.
 *
 * @param[in,out]   state       ptr to preallocated state structure
 * @param[in]       target_slot slot to write update into
 * @param[in]       offset      Bytes offset to start write at
 *
 * @returns         0 on success, <0 otherwise
 */
int riotboot_flashwrite_init_raw(riotboot_flashwrite_t *state, int target_slot,
                                 size_t offset);

/**
 * @brief   Initialize firmware update (riotboot version)
 *
 * This function initializes a firmware update, skipping riotboot's magic
 * number ("RIOT") by calling @ref riotboot_flashwrite_init_raw() with an
 * offset of RIOTBOOT_FLASHWRITE_SKIPLEN (4). This ensures that riotboot will
 * ignore the slot until the magic number has been restored, e.g., through @ref
 * riotboot_flashwrite_finish().
 *
 * @param[in,out]   state       ptr to preallocated state structure
 * @param[in]       target_slot slot to write update into
 *
 * @returns         0 on success, <0 otherwise
 */
static inline int riotboot_flashwrite_init(riotboot_flashwrite_t *state,
                                           int target_slot)
{
    /* initialize state, but skip "RIOT" */
    return riotboot_flashwrite_init_raw(state, target_slot,
                                        RIOTBOOT_FLASHWRITE_SKIPLEN);
}

/**
 * @brief   Feed bytes into the firmware writer
 *
 * @note    If the update has been initialized via @ref
 *          riotboot_flashwrite_init(), make sure to skip the first
 *          RIOTBOOT_FLASHWRITE_SKIPLEN bytes.
 *
 * @param[in,out]   state   ptr to previously used update state
 * @param[in]       bytes   ptr to data
 * @param[in]       len     len of data
 * @param[in]       more    whether more data is coming
 *
 * @returns         0 on success, <0 otherwise
 */
int riotboot_flashwrite_putbytes(riotboot_flashwrite_t *state,
                                 const uint8_t *bytes, size_t len, bool more);
/**
 * @brief   Force flush the buffer onto the flash
 *
 * @param[in,out]   state   ptr to previously used update state
 *
 * @returns         0 on success, <0 otherwise
 */
int riotboot_flashwrite_flush(riotboot_flashwrite_t *state);

/**
 * @brief   Finish a firmware update (raw version)
 *
 * This function finishes a firmware update by re-writing the first header
 *
 * @param[in]   state       ptr to previously used state structure
 * @param[in]   bytes       data to re-write in the first page
 * @param[in]   len         size of data in bytes (must be <=FLASHPAGE_SIZE)
 *
 * @returns     0 on success, <0 otherwise
 */
int riotboot_flashwrite_finish_raw(riotboot_flashwrite_t *state,
                               const uint8_t *bytes, size_t len);

/**
 * @brief   Finish a firmware update (riotboot version)
 *
 * This function finishes a firmware update by re-writing the first header so
 * it includes riotboot's magic number ("RIOT").
 *
 * @param[in]   state       ptr to previously used state structure
 *
 * @returns     0 on success, <0 otherwise
 */
static inline int riotboot_flashwrite_finish(riotboot_flashwrite_t *state)
{
    return riotboot_flashwrite_finish_raw(state, (const uint8_t *)"RIOT",
                                          RIOTBOOT_FLASHWRITE_SKIPLEN);
}

/**
 * @brief       Get a slot's size
 *
 * @param[in]   state   ptr to state struct
 *
 * @returns     the size of the slot that @p state is configured to update to
 */
size_t riotboot_flashwrite_slotsize(const riotboot_flashwrite_t *state);

/**
 * @brief       Verify the digest of an image
 *
 * @param[in]   sha256_digest   content of the image digest
 * @param[in]   img_size        the size of the image
 * @param[in]   target_slot     the image slot number
 *
 * @returns     -1 when image is too small
 * @returns     0 if the digest is valid
 * @returns     1 if the digest is invalid
 */
int riotboot_flashwrite_verify_sha256(const uint8_t *sha256_digest,
                                      size_t img_size, int target_slot);

#ifdef __cplusplus
}
#endif

#endif /* RIOTBOOT_FLASHWRITE_H */
