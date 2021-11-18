/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2017 Inria
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riotboot_hdr
 * @{
 *
 * @file
 * @brief       RIOT header helpers and tools
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <stddef.h>
#include <stdio.h>

#ifdef RIOT_VERSION
#include "log.h"
#else
#define LOG_INFO(...) printf(__VA_ARGS__)
#endif

#include "riotboot/hdr.h"
#include "checksum/fletcher32.h"
#include "byteorder.h"

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#   error \
    "This code is implementented in a way that it will only work for little-endian systems!"
#endif

void riotboot_hdr_print(const riotboot_hdr_t *riotboot_hdr)
{
    printf("Image magic_number: 0x%08x\n",
           (unsigned)riotboot_hdr->magic_number);
    printf("Image Version: 0x%08x\n", (unsigned)riotboot_hdr->version);
    printf("Image start address: 0x%08x\n", (unsigned)riotboot_hdr->start_addr);
    printf("Header chksum: 0x%08x\n", (unsigned)riotboot_hdr->chksum);
    printf("\n");
}

int riotboot_hdr_validate(const riotboot_hdr_t *riotboot_hdr)
{
    if (riotboot_hdr->magic_number != RIOTBOOT_MAGIC) {
        LOG_INFO("%s: riotboot_hdr magic number invalid\n", __func__);
        return -1;
    }

    int res = riotboot_hdr_checksum(riotboot_hdr) ==
              riotboot_hdr->chksum ? 0 : -1;

    if (res) {
        LOG_INFO("%s: riotboot_hdr checksum invalid\n", __func__);
    }

    return res;
}

uint32_t riotboot_hdr_checksum(const riotboot_hdr_t *riotboot_hdr)
{
    return fletcher32((uint16_t *)riotboot_hdr, offsetof(riotboot_hdr_t,
                                                         chksum) /
                      sizeof(uint16_t));
}
