/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       Simple firmware format update module
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>

#ifdef RIOT_VERSION
#include "log.h"
#include "ota_pubkey.h"
#else
#include <stdio.h>
#define LOG_INFO(...) printf(__VA_ARGS__)
#endif

#include "firmware.h"
#include "firmware/simple.h"

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

void firmware_simple_print(firmware_simple_t *simple)
{
    printf("Firmware magic_number: 0x%08x\n", (unsigned)simple->metadata.magic_number);
    printf("Firmware Version: %#x\n", (unsigned)simple->metadata.version);
    printf("Firmware start address: 0x%08x\n", (unsigned)simple->metadata.start_addr);
    /* Only output full info if the metadata_type matches */
    if (simple->metadata.metadata_type == FIRMWARE_METADATA_RIOTBOOT)
    {
        printf("Firmware APPID: %#x\n", (unsigned)simple->appid);
        printf("Firmware Size: %" PRIu32 "\n", simple->size);
        printf("Firmware HASH: ");
        for (unsigned long i = 0; i < sizeof(simple->hash); i++) {
            printf("%02x ", simple->hash[i]);
        }
        printf("\n");
        printf("Firmware chksum: 0x%08x\n", (unsigned)simple->metadata.chksum);
        printf("Firmware signature: ");
        for (unsigned long i = 0; i < sizeof(simple->sig); i++) {
            printf("%02x ", simple->sig[i]);
        }
        printf("\n");
    }
    else {
        printf("Firmware of unknown type\n");
    }
}

int firmware_simple_validate_signature(firmware_simple_t *simple, const unsigned char *pk)
{
    if (firmware_validate_metadata_checksum(&simple->metadata)) {
        return -1;
    }

    unsigned char sm[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    memcpy(sm, ((unsigned char *)simple) + FIRMWARE_SIGN_BYTES, crypto_sign_BYTES);
    memcpy(sm + crypto_sign_BYTES, simple, FIRMWARE_SIGN_BYTES);

    unsigned char m[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    unsigned long long mlen;
    int res = crypto_sign_open(m, &mlen, sm, FIRMWARE_SIGN_BYTES + crypto_sign_BYTES, pk);
    if (res) {
        LOG_INFO("%s: RIOTboot metadata signature invalid\n", __func__);
    }
    return res;
}

int firmware_simple_sign(firmware_simple_t *simple, unsigned char *sk)
{
    unsigned char sm[FIRMWARE_SIGN_BYTES + crypto_sign_BYTES];
    unsigned long long smlen;

    crypto_sign(sm, &smlen, (unsigned char *)simple, FIRMWARE_SIGN_BYTES, sk);
    memcpy(simple->sig, sm, crypto_sign_BYTES);
    return 0;
}

#ifdef RIOT_VERSION
static ssize_t _simple_recv_metadata(firmware_simple_update_t *state, const uint8_t *buf, size_t len)
{
    size_t to_copy = min(len, FIRMWARE_METADATA_SIZE - state->writer.offset);
    /* copy metadata from packet to the firmware update struct */
    memcpy(state->m.metadata_buf + state->writer.offset,
           buf,
           to_copy);
    if ((state->writer.offset + to_copy) >= FIRMWARE_METADATA_SIZE - 1) {
        /* Full metadata received */
        firmware_simple_t *metadata = &state->m.metadata;
        LOG_INFO("ota: verifying metadata ...\n");
        if (metadata->metadata.start_addr != firmware_get_image_startaddr(state->writer.target_slot)) {
            LOG_WARNING("ota: start address doesn't match selected slot. Aborting.\n");
            LOG_WARNING("ota: (image start=%p slot start=%p)\n", (void *)metadata->metadata.start_addr, \
                        (void *)firmware_get_image_startaddr(state->writer.target_slot));
            return -1;
        }

        /* check metadata magic nr, checksum and signature */
        if (firmware_simple_validate_signature(metadata, ota_public_key)) {
            LOG_WARNING("ota: verification failed!\n");
            return -1;
        }
        else {
            state->state = FIRMWARE_UPDATE_VERIFIED;
            LOG_INFO("ota: verification successful\n");
        }
        state->writer.flashpage++;
    }
    return to_copy;
}

int firmware_simple_init(firmware_simple_update_t *state)
{
    state->state = FIRMWARE_UPDATE_INITIALIZED;
    return firmware_flashwrite_init(&state->writer, firmware_target_slot(), 0);
}

int firmware_simple_putbytes(firmware_simple_update_t *state, const uint8_t *bytes, size_t len)
{
    /* metadata copy */
    if (state->state != FIRMWARE_UPDATE_VERIFIED) {
        ssize_t res = _simple_recv_metadata(state, bytes, len);
        if (res < 0 ) {
            return res;
        }
        state->writer.offset += res;
        bytes += res;
        len -= res;
    }
    /* firmware copy */
    if (state->state == FIRMWARE_UPDATE_VERIFIED && len) {
        /* Calculate if there are more bytes that should be written */
        size_t metadata_size = state->m.metadata.metadata.start_addr -
                (uint32_t)firmware_get_metadata(state->writer.target_slot);
        bool more = (state->m.metadata.size + metadata_size) -
                 (state->writer.offset + len);
        return firmware_flashwrite_putbytes(&state->writer, bytes, len, more);
    }
    return 0;
}

int firmware_simple_finish(firmware_simple_update_t *state)
{
    firmware_simple_t *metadata = &state->m.metadata;

    LOG_DEBUG("ota: verifying hash...\n");
    sha256_init(&state->sha);
    sha256_update(&state->sha, (void*)metadata->metadata.start_addr,
            metadata->size);

    sha256_final(&state->sha, state->writer.flashpage_buf);

    if (memcmp(state->writer.flashpage_buf, metadata->hash, SHA256_DIGEST_LENGTH)) {
        LOG_WARNING("ota: image hash incorrect!\n");
        state->state = FIRMWARE_UPDATE_IDLE;
        return -1;
    }

    LOG_DEBUG("ota: hash verified, re-flashing first block...\n");
    if (firmware_flashwrite_finish(&state->writer,
                (firmware_metadata_t*)metadata, sizeof(firmware_simple_t)) != 0) {
        return -1;
    }

    LOG_INFO("ota: firmware flashing completed successfully\n");
    return 0;

}
#endif
