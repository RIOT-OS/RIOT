/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_firmware Firmware Updates
 * @ingroup     sys
 * @{
 *
 * # riotboot minimal firmware update infrastructure
 *
 * ## Overview
 *
 * riotboot is the name of a minimal bootloader application and infrastructure.
 * It consists of
 *
 * - the application "riotboot" in "dist/riotboot" which serves as
 *   minimal bootloader
 *
 * - the modules "firmware" and "firmware_update" that are used
 *   in an image to handle verification and flash-writing
 *
 * - the module "ota_coap" which serves as transport for the updates
 *
 * - a tool in dist/tools/firmware which handles key generation
 *   and firmware image signing.
 *
 * - a couple of make targets to glue everything together
 *
 * ## Concept
 *
 * riotboot expects the flash to be split in three parts: one for the
 * bootloader itself, and two slots for images.
 * Of the latter, one slot holds the running (or latest) image, while the other is
 * used to write a newer image.
 *
 * The bootloader will, on reset, verify the checksum of the image slots, then
 * choose the one with the highest version number that has a valid checksum.  If
 * all slots have the same version number, the first one will be booted.  If none
 * of the slots has a valid checksum, no image will be booted and the bootloader
 * will enter "while(1);".
 *
 * Any image that the bootloader can load is supposed to be signed by the firmware
 * tool, which will take an image, sign it using tweetnacl prefixes the image with
 * corresponding metadata.
 *
 * The metadata contains
 *
 * - "RIOT" as magic number
 * - an application version
 * - an application ID
 * - a checksum
 * - a signature
 *
 * The bootloader only cares about checksum and application version. It expects a
 * running image to verify the metadata.
 *
 * All image transportation and verification is supposed to be done by a running
 * image.
 *
 * The user must update using a binary that has been linked & signed for the
 * desired target slot.
 * The slot that contains the running image cannot be overwritten.
 *
 * The module "firmware_update" provides a simple API that can be fed image data
 * in arbitrary block sizes.  The API is similar to stream hashing. There's
 * "firmware_update_init()", "firmware_update_put_bytes()" and
 * "firmware_update_finish()".  The module transparently handles image
 * verification and flash writing.
 *
 * A tool in dist/tools/firmware can create the necessary keys using tweetnacl,
 * and also sign/verify firmware images.
 *
 * A module in sys/firmware contains common code for firmware metadata handling.
 *
 * ## Threat vector
 *
 * If properly used, riotboot allows secure updates of running firmware.
 * Only update requests that have a valid signature will cause a write to flash.
 * Only updates with a higher version number will be considered, protecting from
 * replay or downgrade attacks, given a node is running the latest version.
 * Updates cannot put the node in undefined state if an upgrade is aborted at any
 * time or otherwise invalid.
 * Only correctly written and verified updates are considered by the bootloader.
 *
 * ## Usage
 *
 * riotboot needs a public key to be compiled in the image for verification purposes.
 * Create a keypair using dist/tools/firmware.
 *
 * riotboot needs some board specific support, mainly slot size configuration, in
 * order to work for a specific board.
 *
 * If that support is in place, it is easiest to use the riotboot specific make
 * targets in order to get the support on the device.
 *
 * See README.md in examples/ota for examples.
 *
 * @file
 * @brief       Firmware Updates
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#ifndef FIRMWARE_H
#define FIRMWARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "hashes/sha256.h"
#include "tweetnacl.h"

/**
 *  @brief Provisional length for signed hash
 */
#define FIRMWARE_SIG_LEN            (crypto_sign_BYTES)

/**
 *  @brief Length of secret Ed25519 key
 */
#define FIRMWARE_SECKEY_LEN         (crypto_sign_SECRETKEYBYTES)

/**
 *  @brief Length of public Ed25519 key
 */
#define FIRMWARE_PUBKEY_LEN         (crypto_sign_PUBLICKEYBYTES)

/**
 *  @brief Number of metadata bytes that get checksummed
 */
#define FIRMWARE_CHECKSUM_LEN       (20)

/**
 *  @brief Length of bytes to be signed
 */
#define FIRMWARE_SIGN_BYTES         (FIRMWARE_CHECKSUM_LEN + 4 + SHA256_DIGEST_LENGTH)

/**
 *  @brief Length of metadata prefixed to firmware binaries
 */
#ifndef FIRMWARE_METADATA_SIZE
#define FIRMWARE_METADATA_SIZE      (256)
#endif

/**
 *  @brief Length of padding to align metadata size
 */
#define FIRMWARE_PADDING            (FIRMWARE_METADATA_SIZE-(FIRMWARE_SIGN_BYTES + FIRMWARE_SIG_LEN))

/**
 *  @brief Real length of metadata without padding
 */
#define FIRMWARE_METADATA_REALLEN   (sizeof(firmware_metadata_t) - FIRMWARE_PADDING)

/**
 * @brief Structure to store firmware metadata
 * @{
 */
typedef struct {
    uint32_t magic_number;              /**< metadata magic_number (always "RIOT")  */
    uint32_t appid;                     /**< Integer representing the application ID*/
    uint32_t version;                   /**< Integer representing firmware version  */
    uint32_t size;                      /**< Size of firmware image (w/o metadata)  */
    uint32_t start_addr;                /**< Start address in flash                 */
    uint32_t chksum;                    /**< checksum of metadata                   */
    uint8_t hash[SHA256_DIGEST_LENGTH]; /**< SHA256 Hash of firmware image          */
    uint8_t sig[FIRMWARE_SIG_LEN];      /**< Firmware signature                     */
    uint8_t pad[FIRMWARE_PADDING];      /**< padding to total of
                                             FIRMWARE_METADATA_SIZE bytes           */
} firmware_metadata_t;
/** @} */

/**
 * @brief  Print formatted FW image metadata to STDIO
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_metadata_print(firmware_metadata_t *metadata);

/**
 * @brief  Validate FW image metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
int firmware_validate_metadata_checksum(firmware_metadata_t *metadata);

/**
 * @brief  Calculate metadata checksum
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
uint32_t firmware_metadata_checksum(firmware_metadata_t *metadata);

/**
 * @brief  Sign metadata
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] sk        NaCL secret signing key to use
 *
 */
int firmware_sign_metadata(firmware_metadata_t *metadata, unsigned char *sk);

/**
 * @brief  Validate FW metadata signature
 *
 * @param[in] metadata  ptr to firmware metadata
 * @param[in] pk        NaCL public signing key to use
 *
 */
int firmware_validate_metadata_signature(firmware_metadata_t *metadata, const unsigned char *pk);

/**
 * @brief  Jump to image
 *
 * @param[in] metadata  ptr to firmware metadata
 *
 */
void firmware_jump_to_image(firmware_metadata_t *metadata);

/**
 * @brief  Get currently running image slot
 *
 * returns nr of currently active slot
 */
int firmware_current_slot(void);

/**
 * @brief  Get next (to be empty) image slot
 *
 * returns free target slot
 */
int firmware_target_slot(void);

/**
 * @brief  Get metadata of firmware slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * returns metadata of image slot nr @p slot
 */
firmware_metadata_t *firmware_get_metadata(unsigned slot);

/**
 * @brief  Get jump-to address of firmware slot
 *
 * @param[in]   slot    slot nr to work on
 *
 * @returns address of first byte of @p slot
 */
unsigned firmware_get_image_startaddr(unsigned slot);

/**
 * @brief  Boot into image in slot @p slot
 *
 * @param[in]   slot    slot nr to jump to
 */
void firmware_jump_to_slot(unsigned slot);

/**
 * @brief  Dump firmware slot addresses
 */
void firmware_dump_slot_addrs(void);

/**
 * @brief   Number of configured firmware slots (incl. bootloader slot)
 */
extern const unsigned firmware_num_slots;

#ifdef __cplusplus
}
#endif

#endif /* FIRMWARE_H */
