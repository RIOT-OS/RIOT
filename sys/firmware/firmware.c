/*
 * Copyright (C) 2017 Kaspar Schleiser
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_firmware
 * @{
 *
 * @file
 * @brief       Firmware module
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>

#ifdef RIOT_VERSION
#include "cpu.h"
#include "log.h"
#include "crypto/ed25519_pub.h"
#include "periph/flashpage.h"
#else
#include <stdio.h>
#define LOG_INFO(...) printf(__VA_ARGS__)
#endif

#include "firmware.h"
#include "checksum/fletcher32.h"
#include "tweetnacl.h"

void firmware_metadata_print(firmware_metadata_t *metadata)
{
    printf("Firmware magic_number: 0x%08x\n", (unsigned)metadata->magic_number);
    printf("Firmware Size: %lu\n", metadata->size);
    printf("Firmware APPID: %#lx\n", metadata->appid);
    printf("Firmware Version: %#lx\n", metadata->version);
    printf("Firmware start address: 0x%08x\n", (unsigned)metadata->start_addr);
    printf("Firmware HASH: ");
    for (unsigned long i = 0; i < sizeof(metadata->hash); i++) {
        printf("%02x ", metadata->hash[i]);
    }
    printf("\n");
    printf("Firmware chksum: 0x%08x\n", (unsigned)metadata->chksum);
    printf("Firmware signature: ");
    for (unsigned long i = 0; i < sizeof(metadata->sig); i++) {
        printf("%02x ", metadata->sig[i]);
    }
    printf("\n");
}

int firmware_validate_metadata_checksum(firmware_metadata_t *metadata)
{
    if (memcmp(metadata, "RIOT", 4)) {
        printf("%s: metadata magic number invalid\n", __func__);
        return -1;
    }

    int res = firmware_metadata_checksum(metadata) == metadata->chksum ? 0 : -1;
    if (res) {
        printf("%s: metadata checksum invalid\n", __func__);
    }
    return res;
}

int firmware_validate_metadata_signature(firmware_metadata_t *metadata, unsigned char *pk)
{
    if (firmware_validate_metadata_checksum(metadata)) {
        return -1;
    }

    unsigned char sm[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    memcpy(sm, ((unsigned char *)metadata) + FIRMWARE_SIGN_BYTES, crypto_sign_BYTES);
    memcpy(sm + crypto_sign_BYTES, metadata, FIRMWARE_SIGN_BYTES);

    unsigned char m[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    unsigned long long mlen;
    int res = crypto_sign_open(m, &mlen, sm, FIRMWARE_SIGN_BYTES + crypto_sign_BYTES, pk);
    if (res) {
        printf("%s: metadata signature invalid\n", __func__);
    }
    return res;
}

uint32_t firmware_metadata_checksum(firmware_metadata_t *metadata)
{
    return fletcher32((uint16_t*)metadata, FIRMWARE_CHECKSUM_LEN / 2);
}

int firmware_sign_metadata(firmware_metadata_t *metadata, unsigned char *sk)
{
    unsigned char sm[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    unsigned long long smlen;
    crypto_sign(sm, &smlen, (unsigned char*)metadata, FIRMWARE_SIGN_BYTES, sk);
    memcpy(metadata->sig, sm, crypto_sign_BYTES);
    return 0;
}

#ifdef RIOT_VERSION
static const unsigned _firmware_slot_start[] = {
    CPU_FLASH_BASE,
    CPU_FLASH_BASE + FIRMWARE_SLOT0_SIZE,
    CPU_FLASH_BASE + FIRMWARE_SLOT0_SIZE + FIRMWARE_SLOT1_SIZE
};

void firmware_jump_to_image(firmware_metadata_t *metadata)
{
    uint32_t addr = (unsigned)metadata + (unsigned)sizeof(firmware_metadata_t);
    printf("[bootloader] jumping to %#lx\n", addr);
    cpu_jump_to_image(addr);
}

int firmware_current_slot(void)
{
    unsigned base_addr = cpu_get_image_baseaddr() - sizeof(firmware_metadata_t);

    for (unsigned i = 1; i < FIRMWARE_NUM_SLOTS; i++) {
        if (base_addr == _firmware_slot_start[i]) {
            return i;
        }
    }

    return -1;
}

firmware_metadata_t *firmware_get_metadata(int slot) {
    assert(slot < FIRMWARE_NUM_SLOTS);
    return (firmware_metadata_t *)_firmware_slot_start[slot];
}

unsigned firmware_get_image_startaddr(unsigned slot)
{
    assert(slot < FIRMWARE_NUM_SLOTS);
    return _firmware_slot_start[slot] + sizeof(firmware_metadata_t);
}

uint32_t firmware_get_slot_page(uint8_t slot)
{
    return cpu_get_slot_page(slot);
}

void firmware_jump_to_slot(unsigned slot)
{
    firmware_jump_to_image(firmware_get_metadata(slot));
}

void firmware_dump_slot_addrs(void)
{
    for (unsigned i = 1; i < FIRMWARE_NUM_SLOTS; i++) {
        printf("slot %u: metadata: 0x%08x image: 0x%08x\n", i, \
                (unsigned)_firmware_slot_start[i], firmware_get_image_startaddr(i));
    }
}

uint32_t firmware_get_slot_address(uint8_t slot)
{
    return cpu_get_slot_address(slot);
}

int firmware_validate_int_slot(uint8_t slot)
{
    firmware_metadata_t metadata;
    int result;

    assert(slot < FIRMWARE_NUM_SLOTS);

    memcpy(&metadata, firmware_get_metadata(slot), sizeof(firmware_metadata_t));
    result = firmware_validate_metadata_signature(&metadata, ed25519_pub);

    if (result == -1) {
        printf("[firmware] Signature check failed!\n");
        return result;
    }

    return result;
}

int firmware_find_newest_int_image(void)
{
    /* At first, we only assume knowledge of version v0 */
    uint32_t slot1_version = 0;
    uint32_t slot2_version = 0;
    firmware_metadata_t metadata[2];

    memcpy(&metadata[0], firmware_get_metadata(1), sizeof(firmware_metadata_t));
    memcpy(&metadata[1], firmware_get_metadata(2), sizeof(firmware_metadata_t));

    if (firmware_validate_metadata_checksum(&metadata[0]) != -1) {
        firmware_metadata_print(&metadata[0]);
        slot1_version = metadata[0].version;
    }
    else {
        printf("[firmware] ERROR cannot get slot 1 metadata.\n");
    }

    if (firmware_validate_metadata_checksum(&metadata[1]) != -1) {
        firmware_metadata_print(&metadata[1]);
        slot2_version = metadata[1].version;
    }
    else {
        printf("[firmware] ERROR cannot get slot 2 metadata.\n");
    }

    if (slot1_version > slot2_version) {
        return 1;
    }
    else if (slot1_version < slot2_version) {
        return 2;
    }
    else {
        return -1;
    }

    return -1;
}

int firmware_erase_int_image(uint8_t slot)
{
    /* Get the page where the fw_slot is located */
    uint32_t slot_page;
    uint32_t slot_pages_nbr = SLOT_SIZE / FLASHPAGE_SIZE;

    printf("[firmware] Erasing %ld pages for slot %d\n", slot_pages_nbr, slot);

    assert(slot < FIRMWARE_NUM_SLOTS);

    slot_page = firmware_get_slot_page(slot);

    printf("[firmware] Starting at page: %ld\n", slot_page);

    /* Erase each page in the FW internal slot! */
    for (int page = slot_page; page < slot_page + slot_pages_nbr; page++) {
        printf("[firmware] Erasing page %d\n", page);
        flashpage_write(page, NULL);
    }

    //DEBUG("[firmware] Erase successful\n");

    return 0;
}

#endif /* RIOT_VERSION */
