/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_uuid
 * @{
 * @file
 * @brief   Function implementations to create RFC 4122 UUID objects
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#include <string.h>
#include "byteorder.h"
#include "hashes/md5.h"
#include "hashes/sha1.h"
#include "random.h"
#include "uuid.h"
#include "fmt.h"

const uuid_t uuid_namespace_dns = { /* 6ba7b810-9dad-11d1-80b4-00c04fd430c8 */
    .time_low.u8 =      { 0x6b, 0xa7, 0xb8, 0x10 },
    .time_mid.u8 =      { 0x9d, 0xad },
    .time_hi.u8 =       { 0x11, 0xd1 },
    .clk_seq_hi_res =   0x80,
    .clk_seq_low =      0xb4,
    .node =             { 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }
};

const uuid_t uuid_namespace_url = { /* 6ba7b811-9dad-11d1-80b4-00c04fd430c8 */
    .time_low.u8 =      { 0x6b, 0xa7, 0xb8, 0x11 },
    .time_mid.u8 =      { 0x9d, 0xad },
    .time_hi.u8 =       { 0x11, 0xd1 },
    .clk_seq_hi_res =   0x80,
    .clk_seq_low =      0xb4,
    .node =             { 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }
};

/* Name string is an ISO OID */
const uuid_t uuid_namespace_iso = { /* 6ba7b812-9dad-11d1-80b4-00c04fd430c8 */
    .time_low.u8 =      { 0x6b, 0xa7, 0xb8, 0x12 },
    .time_mid.u8 =      { 0x9d, 0xad },
    .time_hi.u8 =       { 0x11, 0xd1 },
    .clk_seq_hi_res =   0x80,
    .clk_seq_low =      0xb4,
    .node =             { 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }
};

const uuid_t uuid_namespace_x500 = { /* 6ba7b814-9dad-11d1-80b4-00c04fd430c8 */
    .time_low.u8 =      { 0x6b, 0xa7, 0xb8, 0x14 },
    .time_mid.u8 =      { 0x9d, 0xad },
    .time_hi.u8 =       { 0x11, 0xd1 },
    .clk_seq_hi_res =   0x80,
    .clk_seq_low =      0xb4,
    .node =             { 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8 }
};

static inline void _set_version(uuid_t *uuid, unsigned version)
{
    uint16_t time_hi = byteorder_ntohs(uuid->time_hi) & 0x0fff;

    time_hi |= (version << 12);
    uuid->time_hi = byteorder_htons(time_hi);
}

static inline void _set_reserved(uuid_t *uuid)
{
    uuid->clk_seq_hi_res = (uuid->clk_seq_hi_res & 0x3f) | 0x80;
}

void uuid_v3(uuid_t *uuid, const uuid_t *ns, const uint8_t *name, size_t len)
{
    /* Digest calculation */
    md5_ctx_t ctx;

    md5_init(&ctx);
    md5_update(&ctx, ns, sizeof(uuid_t));
    md5_update(&ctx, name, len);
    md5_final(&ctx, uuid);

    _set_version(uuid, UUID_V3);
    _set_reserved(uuid);
}

void uuid_v4(uuid_t *uuid)
{
    random_bytes((uint8_t *)uuid, sizeof(uuid_t));
    _set_version(uuid, UUID_V4);
    _set_reserved(uuid);
}

void uuid_v5(uuid_t *uuid, const uuid_t *ns, const uint8_t *name, size_t len)
{
    uint8_t digest[20];
    sha1_context ctx;

    sha1_init(&ctx);
    sha1_update(&ctx, ns, sizeof(uuid_t));
    sha1_update(&ctx, name, len);
    sha1_final(&ctx, digest);

    memcpy(uuid, digest, sizeof(uuid_t));

    _set_version(uuid, UUID_V5);
    _set_reserved(uuid);
}

void uuid_to_string(const uuid_t *uuid, char *str)
{
    char *p = str;
    p += fmt_u32_hex(p, byteorder_ntohl(uuid->time_low));
    p += fmt_char(p, '-');
    p += fmt_u16_hex(p, byteorder_ntohs(uuid->time_mid));
    p += fmt_char(p, '-');
    p += fmt_u16_hex(p, byteorder_ntohs(uuid->time_hi));
    p += fmt_char(p, '-');
    p += fmt_byte_hex(p, uuid->clk_seq_hi_res);
    p += fmt_byte_hex(p, uuid->clk_seq_low);
    p += fmt_char(p, '-');
    p += fmt_bytes_hex(p, uuid->node, UUID_NODE_LEN);
    *p = '\0';
    fmt_to_lower(str, str);
}

int uuid_from_string(uuid_t *uuid, const char *str)
{
    uint32_t tmp;
    if (fmt_strlen(str) < UUID_STR_LEN) {
        return -1;
    }
    tmp = scn_u32_hex(str, 8);
    uuid->time_low = byteorder_htonl(tmp);
    str += 8;
    if (*str++ != '-') {
        return -2;
    }
    tmp = scn_u32_hex(str, 4);
    uuid->time_mid = byteorder_htons(tmp);
    str += 4;
    if (*str++ != '-') {
        return -2;
    }
    tmp = scn_u32_hex(str, 4);
    uuid->time_hi = byteorder_htons(tmp);
    str += 4;
    if (*str++ != '-') {
        return -2;
    }
    uuid->clk_seq_hi_res = scn_u32_hex(str, 2);
    str += 2;
    uuid->clk_seq_low = scn_u32_hex(str, 2);
    str += 2;
    if (*str++ != '-') {
        return -2;
    }
    for (unsigned i = 0; i < UUID_NODE_LEN; i++) {
        uuid->node[i] = scn_u32_hex(str, 2);
        str += 2;
    }

    return 0;
}
