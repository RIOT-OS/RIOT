/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup olsr2
 * @{
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 * @}
 */

#include <stdlib.h>

#ifdef RIOT
#include "vtimer.h"
#include "rtc.h"
#else
#include <time.h>
#include <unistd.h>
#endif

#include "constants.h"
#include "util.h"
#include "node.h"
#include "olsr_debug.h"

const char* netaddr_to_str_s(struct netaddr_str* dst, const struct netaddr* src)
{
    return src ? netaddr_to_string(dst, src) : NULL;
}

struct netaddr* netaddr_dup(struct netaddr* addr)
{
    struct netaddr_rc* addr_new = calloc(1, sizeof(struct netaddr_rc));

    if (addr_new == NULL)
        return NULL;

    addr_new->_refs = 1;
    return memcpy(addr_new, addr, sizeof(struct netaddr));
}

struct netaddr* netaddr_use(struct netaddr* addr)
{
    ((struct netaddr_rc*) addr)->_refs++;
    return addr;
}

struct netaddr* netaddr_reuse(struct netaddr* addr)
{
    if (netaddr_cmp(addr, get_local_addr()) == 0)
        return netaddr_use(get_local_addr());

    struct olsr_node* n = get_node(addr);
    if (!n) {
        DEBUG("Address %s not found, this shouldn't happen", netaddr_to_str_s(&nbuf[0], addr));
        return netaddr_dup(addr);
    }
    return netaddr_use(n->addr);
}

struct netaddr* netaddr_free(struct netaddr* addr)
{
    struct netaddr_rc* addr_rc = (struct netaddr_rc*) addr;

    if (addr)
        DEBUG("netaddr_free(%s) - %d refs", netaddr_to_str_s(&nbuf[0], addr), addr_rc->_refs);

    if (addr != NULL && --addr_rc->_refs == 0)
        free(addr_rc);

    return NULL;
}

void netaddr_switch(struct netaddr** old_addr, struct netaddr* new_addr)
{
    netaddr_free(*old_addr);
    *old_addr = netaddr_reuse(new_addr);
}

time_t time_now(void)
{
#ifdef RIOT
    struct timeval _tv;
    return rtc_time(&_tv);
#else
    return time(0);
#endif
}

void sleep_s(int secs)
{
#ifdef RIOT
    vtimer_usleep(secs * SECOND);
#else
    // process wakes up when a package arrives
    // go back to sleep to prevent flooding
    int remaining_sleep = secs;
    while ((remaining_sleep = sleep(remaining_sleep)));
#endif
}
