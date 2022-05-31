/*
 * Copyright (C) 2019 Inria
 *               2019 Freie Universität Berlin
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_flashwrite
 * @{
 *
 * @file
 * @brief       Firmware update sha256 verification helper functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "hashes/sha256.h"
#include "riotboot/hdr.h"
#include "log.h"

int riotboot_slot_verify_sha256(const riotboot_hdr_t *hdr)
{
    sha256_context_t sha256;
    uint8_t digest[SHA256_DIGEST_LENGTH];
    uint8_t *img_start = (uint8_t *)hdr->start_addr;

    if (hdr->img_size + hdr->start_addr > CPU_FLASH_BASE + ROM_LEN) {
        return -1;
    }

    sha256_init(&sha256);
    sha256_update(&sha256, img_start, hdr->img_size);
    sha256_final(&sha256, digest);

    if (memcmp(hdr->digest, digest, SHA256_DIGEST_LENGTH) != 0) {
        LOG_INFO("riotboot: invalid digest");
        return -1;
    }
    return 0;
}
