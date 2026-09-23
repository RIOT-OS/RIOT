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
#include <stdint.h>
#include <stdio.h>

#ifdef RIOT_VERSION
#include "log.h"
#else
#define LOG_INFO(...) printf(__VA_ARGS__)
#define LOG_DEBUG(...) printf(__VA_ARGS__)
#endif

#include "riotboot/hdr.h"
#include "checksum/fletcher32.h"
#include "byteorder.h"

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#   error \
    "This code is implemented in a way that it will only work for little-endian systems!"
#endif

static uint32_t *_magic_number(riotboot_hdr_t *riotboot_hdr)
{
    return &riotboot_hdr->v1.magic_number;
}

static uint32_t *_version(riotboot_hdr_t *riotboot_hdr)
{
    switch (riotboot_hdr->v1.magic_number) {
    case RIOTBOOT_MAGIC_V1:
        return &riotboot_hdr->v1.version;
    case RIOTBOOT_MAGIC_V2:
        return &riotboot_hdr->v2.version;
    default:
        return NULL;
    }
}

static uint32_t *_start_addr(riotboot_hdr_t *riotboot_hdr)
{
    switch (riotboot_hdr->v1.magic_number) {
    case RIOTBOOT_MAGIC_V1:
        return &riotboot_hdr->v1.start_addr;
    case RIOTBOOT_MAGIC_V2:
        return &riotboot_hdr->v2.start_addr;
    default:
        return NULL;
    }
}

static uint32_t *_flags(riotboot_hdr_t *riotboot_hdr)
{
    if (riotboot_hdr->v1.magic_number == RIOTBOOT_MAGIC_V2) {
        return &riotboot_hdr->v2.flags;
    }
    return NULL;
}

static uint32_t *_checksum(riotboot_hdr_t *riotboot_hdr)
{
    switch (riotboot_hdr->v1.magic_number) {
    case RIOTBOOT_MAGIC_V1:
        return &riotboot_hdr->v1.chksum;
    case RIOTBOOT_MAGIC_V2:
        return &riotboot_hdr->v2.chksum;
    default:
        return NULL;
    }
}

uint32_t riotboot_hdr_get_magic_number(const riotboot_hdr_t *riotboot_hdr)
{
    return *_magic_number((riotboot_hdr_t *)riotboot_hdr);
}

uint32_t riotboot_hdr_get_version(const riotboot_hdr_t *riotboot_hdr)
{
    return *_version((riotboot_hdr_t *)riotboot_hdr);
}

uint32_t riotboot_hdr_get_start_addr(const riotboot_hdr_t *riotboot_hdr)
{
    return *_start_addr((riotboot_hdr_t *)riotboot_hdr);
}

uint32_t riotboot_hdr_get_flags(const riotboot_hdr_t *riotboot_hdr)
{
    return *_flags((riotboot_hdr_t *)riotboot_hdr);
}

uint32_t riotboot_hdr_get_checksum(const riotboot_hdr_t *riotboot_hdr)
{
    return *_checksum((riotboot_hdr_t *)riotboot_hdr);
}

void riotboot_hdr_print(const riotboot_hdr_t *riotboot_hdr)
{
    uint32_t magic = riotboot_hdr_get_magic_number(riotboot_hdr);
    printf("Image magic_number: 0x%08x\n", (unsigned)magic);
    printf("Image Version: 0x%08x\n", (unsigned)riotboot_hdr_get_version(riotboot_hdr));
    printf("Image start address: 0x%08x\n", (unsigned)riotboot_hdr_get_start_addr(riotboot_hdr));
    printf("Header chksum: 0x%08x\n", (unsigned)riotboot_hdr_get_checksum(riotboot_hdr));
    if (magic == RIOTBOOT_MAGIC_V2) {
        printf("Image flags: 0x%08x\n", (unsigned)riotboot_hdr_get_flags(riotboot_hdr));
    }
    printf("\n");
}

int riotboot_hdr_validate(const riotboot_hdr_t *riotboot_hdr)
{
    uint32_t magic = riotboot_hdr_get_magic_number(riotboot_hdr);
    if (magic != RIOTBOOT_MAGIC_V1 && magic != RIOTBOOT_MAGIC_V2) {
        LOG_DEBUG("%s: riotboot_hdr magic number invalid\n", __func__);
        return -1;
    }

    int res = riotboot_hdr_checksum(riotboot_hdr) ==
              riotboot_hdr_get_checksum(riotboot_hdr) ? 0 : -1;

    if (res) {
        LOG_DEBUG("%s: riotboot_hdr checksum invalid\n", __func__);
    }

    return res;
}

uint32_t riotboot_hdr_checksum(const riotboot_hdr_t *riotboot_hdr)
{
    return fletcher32((uint16_t *)riotboot_hdr,
                      ((uintptr_t)_checksum((riotboot_hdr_t *)riotboot_hdr) -
                              (uintptr_t)riotboot_hdr) /
                      sizeof(uint16_t));
}

bool riotboot_hdr_is_v2(const riotboot_hdr_t *riotboot_hdr)
{
    return (riotboot_hdr_get_magic_number(riotboot_hdr) == RIOTBOOT_MAGIC_V2);
}

unsigned riotboot_hdr_get_boot_count(const riotboot_hdr_t *riotboot_hdr)
{
    if (riotboot_hdr_is_v2(riotboot_hdr)) {
        unsigned count = (riotboot_hdr->v2.flags & RIOTBOOT_HDR_IMAGE_BOOT_COUNT_MASK) >>
                         RIOTBOOT_HDR_IMAGE_BOOT_COUNT_SHIFT;
        /* count trailing zeros */
        return !count ? 4 : __builtin_ctz(count);
    }
    return 0;
}

void riotboot_hdr_set_boot_count(riotboot_hdr_t *riotboot_hdr,
                                 unsigned boot_count)
{
    if (riotboot_hdr_is_v2(riotboot_hdr)) {
        riotboot_hdr->v2.flags &= ~RIOTBOOT_HDR_IMAGE_BOOT_COUNT_MASK;
        riotboot_hdr->v2.flags |= (((0x0f << boot_count) & 0x0f)
                                   << RIOTBOOT_HDR_IMAGE_BOOT_COUNT_SHIFT);
    }
}

int riotboot_hdr_set_img_state(riotboot_hdr_t *riotboot_hdr,
                               riotboot_hdr_img_state_t state)
{
    if (riotboot_hdr_is_v2(riotboot_hdr)) {
        int current = riotboot_hdr_get_img_state(riotboot_hdr);
        if (current == RIOTBOOT_HDR_IMG_STATE_DEACTIVATED &&
            state != RIOTBOOT_HDR_IMG_STATE_ACTIVATED) {
            return -1;
        }
        if (current == RIOTBOOT_HDR_IMG_STATE_ACTIVATED &&
            (state != RIOTBOOT_HDR_IMG_STATE_CONFIRMED &&
             state != RIOTBOOT_HDR_IMG_STATE_DISMISSED)) {
            return -2;
        }
        riotboot_hdr->v2.flags &= ~RIOTBOOT_HDR_IMAGE_STATE_MASK;
        riotboot_hdr->v2.flags |= state << RIOTBOOT_HDR_IMAGE_STATE_SHIFT;
    }
    return 0;
}

int riotboot_hdr_get_img_state(const riotboot_hdr_t *riotboot_hdr)
{
    if (riotboot_hdr_is_v2(riotboot_hdr)) {
        return (riotboot_hdr->v2.flags & RIOTBOOT_HDR_IMAGE_STATE_MASK)
               >> RIOTBOOT_HDR_IMAGE_STATE_SHIFT;
    }
    return RIOTBOOT_HDR_IMG_STATE_CONFIRMED;
}
