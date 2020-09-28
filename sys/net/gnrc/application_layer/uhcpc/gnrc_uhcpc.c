/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <kernel_defines.h>

#include "net/gnrc/ipv6/nib.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netif.h"
#ifdef MODULE_GNRC_RPL
#include "net/gnrc/rpl.h"
#endif
#ifdef MODULE_GNRC_SIXLOWPAN_CTX
#include "net/gnrc/sixlowpan/ctx.h"
#endif
#include "net/ipv6/addr.h"
#include "net/netdev.h"
#include "net/netopt.h"

#include "net/uhcp.h"
#include "log.h"
#include "fmt.h"

static char addr_str[IPV6_ADDR_MAX_STR_LEN];
static kernel_pid_t gnrc_border_interface;
static kernel_pid_t gnrc_wireless_interface;

static void set_interface_roles(void)
{
    gnrc_netif_t *netif = NULL;

    while ((netif = gnrc_netif_iter(netif))) {
        kernel_pid_t dev = netif->pid;
        int is_wired = gnrc_netapi_get(dev, NETOPT_IS_WIRED, 0, NULL, 0);
        if ((!gnrc_border_interface) && (is_wired == 1)) {
            ipv6_addr_t addr, defroute = IPV6_ADDR_UNSPECIFIED;
            gnrc_border_interface = dev;

            ipv6_addr_from_str(&addr, "fe80::2");
            gnrc_netapi_set(dev, NETOPT_IPV6_ADDR, 64 << 8, &addr,
                            sizeof(addr));
            ipv6_addr_from_str(&addr, "fe80::1");
            gnrc_ipv6_nib_ft_add(&defroute, IPV6_ADDR_BIT_LEN, &addr, dev, 0);

            /* Disable router advertisements on upstream interface. With this,
             * the border router
             * 1. Does not confuse the upstream router to add the border router
             *    to its default router list and
             * 2. Solicits upstream Router Advertisements quicker to
             *    auto-configure its upstream global address.
             */
            gnrc_ipv6_nib_change_rtr_adv_iface(netif, false);
        }
        else if ((!gnrc_wireless_interface) && (is_wired != 1)) {
            gnrc_wireless_interface = dev;
        }

        if (gnrc_border_interface && gnrc_wireless_interface) {
            break;
        }
    }

    LOG_INFO("gnrc_uhcpc: Using %u as border interface and %u as wireless "
             "interface.\n", gnrc_border_interface, gnrc_wireless_interface);
}

static ipv6_addr_t _prefix;
static uint8_t _prefix_len;

#ifdef MODULE_GNRC_SIXLOWPAN_CTX
#define SIXLO_CTX_LTIME_MIN (60U)   /**< context lifetime in minutes */

static bool _ctx_match(const gnrc_sixlowpan_ctx_t *ctx,
                       const ipv6_addr_t *prefix, uint8_t prefix_len)
{
    return (ctx != NULL) &&
           (ctx->prefix_len == prefix_len) &&
           (ipv6_addr_match_prefix(&ctx->prefix, prefix) >= prefix_len);
}

static void _update_6ctx(const ipv6_addr_t *prefix, uint8_t prefix_len)
{
    gnrc_sixlowpan_ctx_t *ctx = gnrc_sixlowpan_ctx_lookup_addr(prefix);
    uint8_t cid = 0;

    if (!_ctx_match(ctx, prefix, prefix_len)) {
        /* While the context is a prefix match, the defined prefix within the
         * context does not match => use new context */
        ctx = NULL;
    }
    else {
        cid = ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK;
    }
    /* find first free context ID */
    if (ctx == NULL) {
        while (((ctx = gnrc_sixlowpan_ctx_lookup_id(cid)) != NULL) &&
               !_ctx_match(ctx, prefix, prefix_len)) {
            cid++;
        }
    }
    if (cid < GNRC_SIXLOWPAN_CTX_SIZE) {
        LOG_INFO("gnrc_uhcpc: uhcp_handle_prefix(): add compression context "
                 "%u for prefix %s/%u\n", cid,
                 ipv6_addr_to_str(addr_str, prefix, sizeof(addr_str)),
                 prefix_len);
        gnrc_sixlowpan_ctx_update(cid, (ipv6_addr_t *)prefix, prefix_len,
                                  SIXLO_CTX_LTIME_MIN, true);
    }
}
#endif

void uhcp_handle_prefix(uint8_t *prefix, uint8_t prefix_len, uint16_t lifetime,
                        uint8_t *src, uhcp_iface_t iface)
{
    (void)lifetime;
    (void)src;

    eui64_t iid;
    if (!gnrc_wireless_interface) {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): received prefix, but "
                    "don't know any wireless interface\n");
        return;
    }

    if ((kernel_pid_t)iface != gnrc_border_interface) {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): received prefix from "
                    "unexpected interface\n");
        return;
    }

    if (gnrc_netapi_get(gnrc_wireless_interface, NETOPT_IPV6_IID, 0, &iid,
                        sizeof(eui64_t)) >= 0) {
        ipv6_addr_set_aiid((ipv6_addr_t*)prefix, iid.uint8);
    }
    else {
        LOG_WARNING("gnrc_uhcpc: uhcp_handle_prefix(): cannot get IID of "
                    "wireless interface\n");
        return;
    }

    if ((_prefix_len == prefix_len) &&
        (ipv6_addr_match_prefix(&_prefix,
                                (ipv6_addr_t *)prefix) >= prefix_len)) {
        LOG_INFO("gnrc_uhcpc: uhcp_handle_prefix(): got same prefix "
                 "again\n");
#ifdef MODULE_GNRC_SIXLOWPAN_CTX
        if (gnrc_netif_is_6ln(gnrc_netif_get_by_pid(gnrc_wireless_interface))) {
            /* always update 6LoWPAN compression context so it does not time
             * out, we can't just remove it anyway according to the RFC */
            _update_6ctx((ipv6_addr_t *)prefix, prefix_len);
        }
#endif
        return;
    }
    else if (!ipv6_addr_is_unspecified(&_prefix)) {
        gnrc_netapi_set(gnrc_wireless_interface, NETOPT_IPV6_ADDR_REMOVE, 0,
                        &_prefix, sizeof(_prefix));
#if defined(MODULE_GNRC_IPV6_NIB) && IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR) && \
    IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
        gnrc_ipv6_nib_abr_del(&_prefix);
#endif
        LOG_INFO("gnrc_uhcpc: uhcp_handle_prefix(): removed old prefix %s/%u\n",
                 ipv6_addr_to_str(addr_str, &_prefix, sizeof(addr_str)),
                 prefix_len);
    }
    memcpy(&_prefix, prefix, sizeof(_prefix));
    _prefix_len = prefix_len;
    gnrc_netapi_set(gnrc_wireless_interface, NETOPT_IPV6_ADDR,
                    (prefix_len << 8), prefix, sizeof(ipv6_addr_t));
    /* only configure 6Lo-ND features when wireless interface uses
     * 6Lo-ND */
    if (gnrc_netif_is_6ln(gnrc_netif_get_by_pid(gnrc_wireless_interface))) {
#ifdef MODULE_GNRC_SIXLOWPAN_CTX
        /* add compression before ABR to add it automatically to its context
         * list */
        _update_6ctx((ipv6_addr_t *)prefix, prefix_len);
#endif
#if defined(MODULE_GNRC_IPV6_NIB) && IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_6LBR) && \
        IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_MULTIHOP_P6C)
        gnrc_ipv6_nib_abr_add((ipv6_addr_t *)prefix);
#endif
    }
#ifdef MODULE_GNRC_RPL
    gnrc_rpl_init(gnrc_wireless_interface);
    gnrc_rpl_instance_t *inst = gnrc_rpl_instance_get(
                                    CONFIG_GNRC_RPL_DEFAULT_INSTANCE);
    if (inst) {
        gnrc_rpl_instance_remove(inst);
    }
    gnrc_rpl_root_init(CONFIG_GNRC_RPL_DEFAULT_INSTANCE, (ipv6_addr_t*)prefix, false,
                       false);
#endif
    LOG_INFO("gnrc_uhcpc: uhcp_handle_prefix(): configured new prefix %s/%u\n",
             ipv6_addr_to_str(addr_str, (ipv6_addr_t *)prefix,
             sizeof(addr_str)), prefix_len);
}

extern void uhcp_client(uhcp_iface_t iface);

static char _uhcp_client_stack[THREAD_STACKSIZE_DEFAULT +
                               THREAD_EXTRA_STACKSIZE_PRINTF];
static msg_t _uhcp_msg_queue[4];

static void* uhcp_client_thread(void *arg)
{
    (void)arg;

    msg_init_queue(_uhcp_msg_queue, ARRAY_SIZE(_uhcp_msg_queue));
    uhcp_client(gnrc_border_interface);
    return NULL;
}

void auto_init_gnrc_uhcpc(void)
{
    set_interface_roles();

    /* only start client if more than one interface is given */
    if (! (gnrc_border_interface && gnrc_wireless_interface)) {
        LOG_WARNING("gnrc_uhcpc: only one interface found, skipping setup.\n");
        return;
    }

    /* initiate uhcp client */
    thread_create(_uhcp_client_stack, sizeof(_uhcp_client_stack),
            THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
            uhcp_client_thread, NULL, "uhcp");
}
