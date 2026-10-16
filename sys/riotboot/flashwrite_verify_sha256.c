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

#include "riotboot/hdr.h"
#include <stdint.h>
#include <string.h>

#include "architecture.h"
#include "hashes/sha256.h"
#include "log.h"
#include "riotboot/slot.h"

int riotboot_flashwrite_verify_sha256(const uint8_t *sha256_digest,
                                      size_t img_len, int target_slot)
{
    char digest[SHA256_DIGEST_LENGTH];

    sha256_context_t sha256;

    if (img_len < 4) {
        LOG_INFO("riotboot: verify_sha256(): image too small\n");
        return -1;
    }

    uint8_t *img_start = (uint8_t *)riotboot_slot_get_hdr(target_slot);

    LOG_INFO("riotboot: verifying digest at %p (img at: %p size: %" PRIuSIZE ")\n",
             sha256_digest, img_start, img_len);

    sha256_init(&sha256);

    /* add RIOTBOOT_MAGIC since it isn't written into flash until
     * riotboot_flashwrite_finish()" */
    uint32_t magic = RIOTBOOT_MAGIC;
    sha256_update(&sha256, &magic, 4);

    /* account for injected RIOTBOOT_MAGIC by skipping RIOTBOOT_MAGIC_LEN */
    sha256_update(&sha256, img_start + 4, img_len - 4);

    sha256_final(&sha256, digest);

    return memcmp(sha256_digest, digest, SHA256_DIGEST_LENGTH) != 0;
}
