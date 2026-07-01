/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotconf_hdr
 * @{
 *
 * @file
 * @brief       RIOT configuration header
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "byteorder.h"
#include "checksum/fletcher32.h"
#include "log.h"
#include "riotconf/hdr.h"

void riotconf_hdr_checksum_init(riotconf_hdr_checksum_ctx_t *ctx, riotconf_hdr_t *hdr)
{
    hdr->magic = htonl(RIOTCONF_MAGIC);
    hdr->checksum = 0;
    hdr->state = 0;
    fletcher32_init(&ctx->ctx);
}

void riotconf_hdr_checksum_update(riotconf_hdr_checksum_ctx_t *ctx, void *data, size_t size)
{
    /* pad with zero for the last update */
    memset(((uint8_t *)data) + size, 0, size % sizeof(uint16_t));
    fletcher32_update(&ctx->ctx, data, size + (size % sizeof(uint16_t)));
}

void riotconf_hdr_checksum_final(riotconf_hdr_checksum_ctx_t *ctx, riotconf_hdr_t *hdr)
{
    hdr->checksum = fletcher32_finish(&ctx->ctx);
}

int riotconf_hdr_validate(riotconf_hdr_t *hdr)
{
    if (hdr->magic != RIOTCONF_MAGIC) {
        LOG_DEBUG("%s: riotconf_hdr magic number invalid\n", __func__);
        return -EINVAL;
    }
    return 0;
}

void riotconf_hdr_hton(riotconf_hdr_t *hdr)
{
    hdr->magic = htonl(hdr->magic);
    hdr->sequence = htonl(hdr->sequence);
    hdr->version = htonl(hdr->version);
    hdr->size = htonl(hdr->size);
    hdr->checksum = htonl(hdr->checksum);
    hdr->state = htonl(hdr->state);
}

void riotconf_hdr_ntoh(riotconf_hdr_t *hdr)
{
    hdr->magic = ntohl(hdr->magic);
    hdr->sequence = ntohl(hdr->sequence);
    hdr->version = ntohl(hdr->version);
    hdr->size = ntohl(hdr->size);
    hdr->checksum = ntohl(hdr->checksum);
    hdr->state = ntohl(hdr->state);
}
