/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

/* Required for strtok_r in string.h, when building with -std=c99 */
#define _DEFAULT_SOURCE 1
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel_defines.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/sixlowpan/ctx.h"
#include "net/sixlowpan/nd.h"
#include "timex.h"

#if IS_USED(MODULE_ZTIMER_MSEC)
#include "ztimer.h"
static ztimer_t del_timer[GNRC_SIXLOWPAN_CTX_SIZE];
#else
#include "xtimer.h"
static xtimer_t del_timer[GNRC_SIXLOWPAN_CTX_SIZE];
#endif

void _del_cb(void *ptr)
{
    gnrc_sixlowpan_ctx_t *ctx = ptr;
    uint8_t cid = ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK;
    ctx->prefix_len = 0;
    del_timer[cid].callback = NULL;
}

static void _usage(char *cmd_str)
{
    printf("usage: %s [{add <0-15> <prefix>/<prefix_len> <ltime in min>|del <ctx>}]\n", cmd_str);
    puts("       `del` will only invalidate the context for compression. It can be");
    puts("       reassigned after 5 min.");
}

int _gnrc_6ctx_list(void)
{
    puts("cid|prefix                                     |C|ltime");
    puts("-----------------------------------------------------------");
    for (uint8_t cid = 0; cid < GNRC_SIXLOWPAN_CTX_SIZE; cid++) {
        gnrc_sixlowpan_ctx_t *ctx = gnrc_sixlowpan_ctx_lookup_id(cid);
        if (ctx != NULL) {
            char addr_str[IPV6_ADDR_MAX_STR_LEN];
            printf(" %2u|%39s/%-3u|%x|%5umin\n", cid,
                   ipv6_addr_to_str(addr_str, &ctx->prefix, sizeof(addr_str)),
                   ctx->prefix_len, (uint8_t) ((ctx->flags_id & 0xf0) >> 4),
                   ctx->ltime);
        }
    }
    return 0;
}

int _gnrc_6ctx_add(char *cmd_str, char *ctx_str, char *prefix_str, char *ltime_str)
{
    ipv6_addr_t prefix;
    char *addr_str, *prefix_len_str, *save_ptr;
    unsigned prefix_len;
    unsigned ltime;
    unsigned ctx = atoi(ctx_str);
    if (ctx >= GNRC_SIXLOWPAN_CTX_SIZE) {
        _usage(cmd_str);
        return 1;
    }
    if (!IS_USED(MODULE_GNRC_IPV6_NIB_6LBR)) {
        puts("WARNING: context dissemination by non-6LBR not supported");
    }
    addr_str = strtok_r(prefix_str, "/", &save_ptr);
    if (addr_str == NULL) {
        _usage(cmd_str);
        return 1;
    }
    prefix_len_str = strtok_r(NULL, "/", &save_ptr);
    if (prefix_len_str == NULL) {
        _usage(cmd_str);
        return 1;
    }
    prefix_len = atoi(prefix_len_str);
    if ((prefix_len - 1U) > 128U) {
        puts("ERROR: prefix_len < 1 || prefix_len > 128");
        return 1;
    }
    ipv6_addr_from_str(&prefix, addr_str);
    if (ipv6_addr_is_unspecified(&prefix)) {
        puts("ERROR: prefix may not be ::");
        return 1;
    }
    ltime = atoi(ltime_str);
    if (gnrc_sixlowpan_ctx_update((uint8_t)ctx, &prefix, (uint8_t)prefix_len, ltime,
                                  true) == NULL) {
        puts("ERROR: can not add context");
        return 1;
    }
    return 0;
}

int _gnrc_6ctx_del(char *cmd_str, char *ctx_str)
{
    gnrc_sixlowpan_ctx_t *ctx;
    unsigned cid = atoi(ctx_str);
    if (cid >= GNRC_SIXLOWPAN_CTX_SIZE) {
        _usage(cmd_str);
        return 1;
    }
    if (!IS_USED(MODULE_GNRC_IPV6_NIB_6LBR)) {
        puts("WARNING: context dissemination by non-6LBR not supported");
    }
    if (del_timer[cid].callback == NULL) {
        ctx = gnrc_sixlowpan_ctx_lookup_id(cid);
        if (ctx != NULL) {
            ctx->flags_id &= ~GNRC_SIXLOWPAN_CTX_FLAGS_COMP;
            ctx->ltime = 0;
            del_timer[cid].callback = _del_cb;
            del_timer[cid].arg = ctx;
#if IS_USED(MODULE_ZTIMER_MSEC)
            ztimer_set(ZTIMER_MSEC, &del_timer[cid],
                       SIXLOWPAN_ND_MIN_CTX_CHANGE_SEC_DELAY * MS_PER_SEC);
#else
            xtimer_set(&del_timer[cid],
                       SIXLOWPAN_ND_MIN_CTX_CHANGE_SEC_DELAY * US_PER_SEC);
#endif
        }
    }
    else {
        printf("Context %u already marked for removal\n", cid);
        return 1;
    }
    return 0;
}

int _gnrc_6ctx(int argc, char **argv)
{
    if (argc < 2) {
        _gnrc_6ctx_list();
    }
    else if (strcmp("add", argv[1]) == 0) {
        if (argc < 5) {
            _usage(argv[0]);
            return 1;
        }
        return _gnrc_6ctx_add(argv[0], argv[2], argv[3], argv[4]);
    }
    else if (strcmp("del", argv[1]) == 0) {
        return _gnrc_6ctx_del(argv[0], argv[2]);
    }
    else {
        _usage(argv[0]);
        return 1;
    }
    return 0;
}

/** @} */
