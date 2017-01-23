/*
 * Copyright (c) 2016, Mark Solters <msolters@gmail.com>
 *               2016, Francisco Acosta <francisco.acosta@inria.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @defgroup    sys_fw_slots Firmware slots management
 * @ingroup     sys
 *
 * @file
 * @brief       FW Image R/W and Verification
 *
 * @author      Mark Solters <msolters@gmail.com>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef FW_SLOTS_H
#define FW_SLOTS_H

#include "hashes/sha256.h"

/*
 *  FW_METADATA_LENGTH:
 *    This is just the size of the FW_metadata_t struct, which is 4-byte
 *    aligned. We use 104 bytes currently, so this struct will be 104 bytes.
 */
#define FW_METADATA_LENGTH  sizeof(FW_metadata_t)

/*
 *  SIGN_LEN:
 *    Provisional length for signed hash
 */
#define SIGN_LEN            (SHA256_DIGEST_LENGTH)

typedef struct FW_metadata {
    uint8_t hash[SHA256_DIGEST_LENGTH]; /* SHA256 Hash of firmware image */
    uint8_t shash[SIGN_LEN];            /* Signed SHA256 */
    uint32_t size;                      /* Size of firmware image */
    uint32_t uuid;                      /* Integer representing unique firmware ID */
    uint16_t version;                   /* Integer representing firmware version */
} FW_metadata_t;

/**
 * @brief  Print formatted FW image metadata to STDIO.
 *
 * @param[in] metadata          Metadata struct to fill with firmware metadata
 *
 */
void print_metadata(FW_metadata_t *metadata);

/**
 * @brief   Validate internal FW slot as a secure firmware
 *
 * @param[in] fw_slot          The FW slot to be validated.
 *
 * @return  0 on success or error code
 */
int validate_int_fw_slot(uint8_t fw_slot);

/**
 * @brief   Get the internal metadata belonging to an FW slot in internal
 *          flash.
 *
 * @param[in] fw_slot             The FW slot to be read for metadata.
 *
 * @param[in] *fw_slot_metadata   Pointer to the FW_metadata_t struct where
 *                                 the metadata is to be written.
 *
 * @return  0 on success or error code
 */
int get_int_metadata(uint8_t fw_slot_page, FW_metadata_t *fw_metadata);

/**
 * @brief   Get the metadata belonging to an FW slot in external flash.
 *
 * @param[in] fw_slot            The FW slot to be read for metadata.
 *
 * @param[in] *fw_slot_metadata  Pointer to the FW_metadata_t struct where
 *                                the metadata is to be written.
 *
 * @return  0 on success or error code
 */
int get_fw_slot_metadata(uint8_t fw_slot, FW_metadata_t *fw_slot_metadata);

/**
 * @brief   Get the metadata belonging to an FW slot in internal flash.
 *
 * @param[in] fw_slot             The FW slot to be read for metadata.
 *
 * @param[in] *fw_slot_metadata   Pointer to the FW_metadata_t struct where
 *                                 the metadata is to be written.
 *
 * @return  0 on success or error code
 */
int get_int_fw_slot_metadata(uint8_t fw_slot, FW_metadata_t *fw_slot_metadata);

/**
 * @brief   Get the address corresponding to a given slot
 *
 * @param[in] fw_slot             The FW slot to get the address.
 *
 *
 * @return  0 on success or error code
 */
uint32_t get_slot_fw_address(uint8_t fw_slot);

/**
 * @brief   Get the page corresponding to a given slot
 *
 * @param[in] fw_slot             The FW slot to get the page.
 *
 *
 * @return  0 on success or error code
 */
uint32_t get_slot_fw_page(uint8_t fw_slot);

/**
 * @brief   Write new metadata to a specific FW slot in internal flash.
 *
 * @param   fw_slot             The FW slot to be modified.
 *
 * @param   *fw_slot_metadata   Pointer to the new FW_metadata_t data.
 *
 * @return  0 on success or error code
 */
int overwrite_int_fw_slot_metadata(uint8_t fw_slot, FW_metadata_t *fw_slot_metadata);

/**
 * @brief   Write new metadata to a specific FW slot in external flash.
 *
 * @param   fw_slot             The FW slot to be modified.
 *
 * @param   *fw_slot_metadata   Pointer to the new FW_metadata_t data.
 *
 * @return  0 on success or error code
 */
int overwrite_fw_slot_metadata(uint8_t fw_slot, FW_metadata_t *fw_slot_metadata);

/**
 * @brief   Copy the current firmware into FW slot 0 as the "Golden Image"
 *
 * @return  0 for success or error code
 */
int backup_golden_image(void);

/**
 * @brief   Given an FW slot, verify the firmware content against the metadata.
 *          If everything is fine, update the metadata to indicate this FW slot
 *          is valid.
 *
 * @param[in]  fw_slot  - FW slot index to verify. (1-3)
 *
 * @return  0 for success or error code
 */
int verify_int_fw_slot(uint8_t fw_slot);

/**
 * @brief   Returns true only if the metadata provided indicates the FW slot
 *          is populated and valid.
 *
 * @param[in] *metadata          FW metadata to be validated
 *
 * @return  True if the FW slot is populated and valid. Otherwise, false.
 */
int validate_fw_metadata(FW_metadata_t *metadata);

/**
 * @brief   Find a FW slot containing firmware matching the supplied
 *          firmware version number. Will only find the first matching
 *          slot.
 *
 * @param[in] version            FW slot version.
 *
 * @return  The FW slot index of the matching FW slot. Return -1 in the event
 *          of no match.
 */
int find_matching_int_fw_slot(uint16_t version);

/**
 * @brief   Find the first empty FW download slot.  Failing this, find the slot
 *          with the most out-of-date firmware version.
 *
 * @return  The FW slot index of the empty/oldest FW slot.  This will never be
 *          0 because the Golden Image should never be erased.
 */
int find_empty_int_fw_slot(void);

/**
 * @brief   Find the FW slot containing the most out-of-date firmware version.
 *          FW slots are in external flash.
 *
 * @return  The FW slot index of the oldest firmware version.
 */
int find_oldest_int_fw_image(void);

/**
 * @brief   Find the FW slot containing the most recent firmware version.
 *          FW slots are in external flash.
 *
 * @return  The FW slot index of the newest firmware version.
 */
int find_newest_int_fw_image(void);

/**
 * @brief   Clear an FW slot in external flash.
 *
 * @param[in] fw_slot   The FW slot index of the firmware image to be copied.
 *
 * @return  0 or error code
 */
int erase_int_fw_image(uint8_t fw_slot);

/**
 * @brief   Overwrite firmware located in internal flash with the firmware
 *          stored in an external flash FW slot.
 *
 * @param[in] fw_slot   The FW slot index of the firmware image to be copied.
 *                       0 = "Golden Image" backup, aka factory restore
 *                       1, 2, 3 = FW Download slots
 *
 * @return  0 or error code
 */
int update_firmware(uint8_t fw_slot);

/**
 * @brief   Store firmware data in external flash at the specified
 *          address.
 *
 * @param[in] ext_address   External flash address to begin writing data.
 *
 * @param[in] data          Pointer to the data buffer to be written.
 *                          Note: page_data can be larger than 4096 bytes, but
 *                          only the first 4096 bytes will be written!
 *
 * @return  0 or error code
 */
int store_firmware_data(uint32_t ext_address, uint8_t *data, size_t data_length);

/**
 * @brief   Begin executing another firmware binary located in internal flash.
 *
 * @param[in] destination_address Internal flash address of the vector table
 *                                for the firmware binary that is to be booted
 *                                into. Since this FW lib prepends metadata
 *                                to each binary, the true VTOR start address
 *                                will be FW_METADATA_SPACE bytes past this
 *                                address.
 *
 */
void jump_to_image(uint32_t destination_address);

#endif /* FW_SLOTS_H */
