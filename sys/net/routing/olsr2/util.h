/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup olsr2
 * @{
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 * @}
 */

#ifndef OLSR2_UTIL_H_
#define OLSR2_UTIL_H_

#include "common/netaddr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct netaddr_rc {
    struct netaddr super;
    uint8_t _refs;
};

const char *netaddr_to_str_s(struct netaddr_str *dst, const struct netaddr *src);

struct netaddr *netaddr_dup(struct netaddr *addr);
struct netaddr *netaddr_use(struct netaddr *addr);
struct netaddr *netaddr_reuse(struct netaddr *addr);
struct netaddr *netaddr_free(struct netaddr *addr);
void netaddr_switch(struct netaddr **old_addr, struct netaddr *new_addr);

time_t time_now(void);
void sleep_s(int secs);

#ifdef __cplusplus
}
#endif

#endif
