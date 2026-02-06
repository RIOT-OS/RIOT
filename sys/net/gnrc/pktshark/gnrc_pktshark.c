/*
 * Copyright (C) 2025 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_pktshark
 * @{
 *
 * @file
 * @brief       Pretty-printer for packages sent/received via netapi
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>

#include <errno.h>
#include "auto_init_utils.h"
#include "auto_init_priorities.h"
#include "byteorder.h"
#include "fmt.h"
#include "thread.h"
#include "msg.h"
#include "net/gnrc.h"
#include "net/icmpv6.h"
#include "net/ipv6/addr.h"
#include "net/ipv6/hdr.h"
#include "net/sixlowpan/nd.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "net/sixlowpan.h"
#include "net/nanocoap.h"
#include "net/ipv4/hdr.h"
#include "od.h"
#include "shell.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Default message queue size for the pktshark thread (as exponent of
 *          2^n).
 *
 *          As the queue size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the queue.
 */
#ifndef CONFIG_GNRC_PKTSHARK_MSG_QUEUE_SIZE_EXP
#define CONFIG_GNRC_PKTSHARK_MSG_QUEUE_SIZE_EXP  3
#endif
/** @} */

/**
 * @brief   Message queue size for the pktshark thread
 */
#ifndef GNRC_PKTSHARK_MSG_QUEUE_SIZE
#define GNRC_PKTSHARK_MSG_QUEUE_SIZE  (1 << CONFIG_GNRC_PKTSHARK_MSG_QUEUE_SIZE_EXP)
#endif

/**
 * @brief   Priority of the pktshark thread
 */
#ifndef GNRC_PKTSHARK_PRIO
#define GNRC_PKTSHARK_PRIO               (THREAD_PRIORITY_MAIN - 1)
#endif

/**
 * @brief   Stack size used for the pktshark thread
 */
#ifndef GNRC_PKTSHARK_STACKSIZE
#define GNRC_PKTSHARK_STACKSIZE          THREAD_STACKSIZE_MAIN
#endif

/**
 * @brief   PID of the pktshark thread
 */
static kernel_pid_t gnrc_pktshark_pid = KERNEL_PID_UNDEF;

/**
 * @brief   Stack for the pktshark thread
 */
static char _stack[GNRC_PKTSHARK_STACKSIZE];
static msg_t _msg_queue[GNRC_PKTSHARK_MSG_QUEUE_SIZE];

/* Iterator for NDP options in a packet */
#define FOREACH_OPT(ndp_pkt, opt, icmpv6_len) \
    for (opt = (ndp_opt_t *)(ndp_pkt + 1); \
         icmpv6_len > 0; \
         icmpv6_len -= (opt->len << 3), \
         opt = (ndp_opt_t *)(((uint8_t *)opt) + (opt->len << 3)))

static void _dump_rtr_sol(const ndp_rtr_sol_t *rtr_sol, size_t payload_len)
{
    print_str("Router Sol");

    ndp_opt_t *opt;
    FOREACH_OPT(rtr_sol, opt, payload_len) {
        switch (opt->type) {
        case NDP_OPT_SL2A:
            print_str(" sl2a");
            break;
        default:
            print_str(" opt=");
            print_u32_dec(opt->type);
        }
    }
}

static void _dump_rtr_adv(const ndp_rtr_adv_t *rtr_adv, size_t payload_len)
{
    print_str("Router Adv");

    ndp_opt_t *opt;
    FOREACH_OPT(rtr_adv, opt, payload_len) {
        switch (opt->type) {
        case NDP_OPT_SL2A:
            print_str(" sl2a");
            break;
        case NDP_OPT_MTU:
            print_str(" mtu=");
            const ndp_opt_mtu_t *mtuo = (void *)opt;
            print_u32_dec(byteorder_ntohl(mtuo->mtu));
            break;
        case NDP_OPT_PI:
            print_str("\n\t  pfx[");
            const ndp_opt_pi_t *pio = (void *)opt;
            if (pio->flags & NDP_OPT_PI_FLAGS_A) {
                print_str("A");
            }
            if (pio->flags & NDP_OPT_PI_FLAGS_L) {
                print_str("L");
            }
            print_str("]=");
            ipv6_prefix_print(&pio->prefix, pio->prefix_len);
            print_str(" ltime={");
            print_u32_dec(byteorder_ntohl(pio->valid_ltime));
            print_str(", ");
            print_u32_dec(byteorder_ntohl(pio->pref_ltime));
            print_str("}");
            break;
        case NDP_OPT_RI:
            print_str("\n\t  route=");
            const ndp_opt_ri_t *rio = (void *)opt;
            ipv6_prefix_print(&rio->prefix, rio->prefix_len);
            print_str(" ltime=");
            print_u32_dec(byteorder_ntohl(rio->route_ltime));
            break;
        case NDP_OPT_6CTX:
            print_str("\n\t  6ctx[");
            const sixlowpan_nd_opt_6ctx_t *sixco = (void *)opt;
            print_u32_dec(sixlowpan_nd_opt_6ctx_get_cid(sixco));
            print_str("]=");
            ipv6_prefix_print((ipv6_addr_t *)(sixco + 1), sixco->ctx_len);
            print_str(" ltime=");
            print_u32_dec(60 * byteorder_ntohs(sixco->ltime));
            break;
        case NDP_OPT_ABR:
            print_str(" abr");
            break;
        case NDP_OPT_RDNSS:
            print_str("\n\t  dns=");
            const ndp_opt_rdnss_impl_t *rdnsso = (void *)opt;
            unsigned addrs_num = (rdnsso->len - 1) / 2;
            for (unsigned i = 0; i < addrs_num; i++) {
                ipv6_addr_print(&rdnsso->addrs[i]);
                if (i + 1 < addrs_num) {
                    print_str(", ");
                }
            }
            print_str(" ltime=");
            print_u32_dec(byteorder_ntohl(rdnsso->ltime));
            break;
        default:
            print_str(" opt=");
            print_u32_dec(opt->type);
        }
    }

    if (rtr_adv->ltime.u16) {
        print_str("\n\t  default, ltime=");
        print_u32_dec(byteorder_ntohs(rtr_adv->ltime));
    }
}

static void _dump_nbr_sol(const ndp_nbr_sol_t *nbr_sol, size_t payload_len)
{
    print_str("Neighbor Sol (");
    ipv6_addr_print(&nbr_sol->tgt);
    print_str(")");

    ndp_opt_t *opt;
    FOREACH_OPT(nbr_sol, opt, payload_len) {
        switch (opt->type) {
        case NDP_OPT_SL2A:
            print_str(" sl2a");
            break;
        case NDP_OPT_AR:
            print_str(" ar");
            break;
        default:
            print_str(" opt=");
            print_u32_dec(opt->type);
        }
    }
}

static void _dump_nbr_adv(const ndp_nbr_adv_t *nbr_adv, size_t payload_len)
{
    print_str("Neighbor Adv (");
    if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_R) {
        print_str("R");
    }
    if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_S) {
        print_str("S");
    }
    if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_O) {
        print_str("O");
    }
    if (nbr_adv->flags & NDP_NBR_ADV_FLAGS_MASK) {
        print_str(" ");
    }
    ipv6_addr_print(&nbr_adv->tgt);
    print_str(")");

    ndp_opt_t *opt;
    FOREACH_OPT(nbr_adv, opt, payload_len) {
        switch (opt->type) {
        case NDP_OPT_TL2A:
            print_str(" tl2a");
            break;
        case NDP_OPT_AR:
            print_str(" ar");
            break;
        default:
            print_str(" opt=");
            print_u32_dec(opt->type);
        }
    }
}

static void _dump_icmpv6_echo(const icmpv6_echo_t *echo, size_t payload_len)
{
    print_str(" seq=");
    print_u32_dec(byteorder_ntohs(echo->seq));
    print_str(" (");
    print_u32_dec(payload_len);
    print_str(" bytes)\n");
}

static void _dump_icmpv6(const icmpv6_hdr_t *hdr, size_t payload_len)
{
    print_str("\n\tICMPv6 ");

    if (!IS_USED(MODULE_GNRC_PKTSHARK_ICMPV6)) {
        goto _default;
    }

    switch (hdr->type) {
    case ICMPV6_DST_UNR:
        print_str("Destination unreachable");
        break;
    case ICMPV6_PKT_TOO_BIG:
        print_str("Packet too big");
        break;
    case ICMPV6_TIME_EXC:
        print_str("Timeout");
        break;
    case ICMPV6_ECHO_REQ:
        print_str("PING");
        _dump_icmpv6_echo((icmpv6_echo_t *)hdr, payload_len - sizeof(icmpv6_echo_t));
        break;
    case ICMPV6_ECHO_REP:
        print_str("PONG");
        _dump_icmpv6_echo((icmpv6_echo_t *)hdr, payload_len - sizeof(icmpv6_echo_t));
        break;
    case ICMPV6_RTR_SOL:
        _dump_rtr_sol((ndp_rtr_sol_t *)hdr, payload_len - sizeof(ndp_rtr_sol_t));
        break;
    case ICMPV6_RTR_ADV:
        _dump_rtr_adv((ndp_rtr_adv_t *)hdr, payload_len - sizeof(ndp_rtr_adv_t));
        break;
    case ICMPV6_NBR_SOL:
        _dump_nbr_sol((ndp_nbr_sol_t *)hdr, payload_len - sizeof(ndp_nbr_sol_t));
        break;
    case ICMPV6_NBR_ADV:
        _dump_nbr_adv((ndp_nbr_adv_t *)hdr, payload_len - sizeof(ndp_nbr_adv_t));
        break;
    case 143:
        print_str("Multicast Listener Report v2");
        break;
    default:
    _default:
        print_str("msg type ");
        print_u32_dec(hdr->type);
        print_str(" (");
        print_u32_dec(payload_len);
        print_str(" bytes)\n");
    }
    print_str("\n");
}

static void _print_coap_block(coap_pkt_t *pkt, uint16_t option)
{
    coap_block1_t block;
    coap_get_block(pkt, &block, option);

    if (option == COAP_OPT_BLOCK1) {
        print_str(" B1[");
    } else {
        print_str(" B2[");
    }
    print_u32_dec(block.blknum);
    print_str(".");
    print_u32_dec(coap_szx2size(block.szx));

    if (block.more) {
        print_str(" M");
    }

    print_str("]");
}

static void _print_coap_format(unsigned format)
{
    print_str(" ");
    switch (format) {
    case COAP_FORMAT_TEXT:
        print_str("text/plain");
        break;
    case COAP_FORMAT_LINK:
        print_str("application/link-format");
        break;
    case COAP_FORMAT_JSON:
        print_str("application/json");
        break;
    case COAP_FORMAT_CBOR:
        print_str("application/cbor");
        break;
    case COAP_FORMAT_OCTET:
        print_str("application/octet-stream");
        break;
    default:
        print_str("fmt=");
        print_u32_dec(format);
        break;
    }
}

static bool _dump_coap(const void *buf, size_t len)
{
    if (!IS_USED(MODULE_GNRC_PKTSHARK_COAP)) {
        return false;
    }

    coap_pkt_t pkt;
    if (coap_parse(&pkt, (void *)buf, len) < 0) {
        return false;
    }

    static const char *coap_method_str[] = {
        [COAP_CODE_EMPTY]  = "EMPTY",
        [COAP_METHOD_GET]  = "GET",
        [COAP_METHOD_POST] = "POST",
        [COAP_METHOD_PUT]  = "PUT",
        [COAP_METHOD_DELETE] = "DELETE",
    };

    static const char *coap_type_str[] = {
        [COAP_TYPE_CON] = "CON",
        [COAP_TYPE_NON] = "NON",
        [COAP_TYPE_ACK] = "ACK",
        [COAP_TYPE_RST] = "RST",
    };

    print_str("\tCoAP ");
    print_str(coap_type_str[coap_get_type(&pkt)]);
    print_str(" ");
    if (coap_get_code_class(&pkt) == 0) {
        print_str(coap_method_str[coap_get_code_raw(&pkt)]);
    } else {
        print_u32_dec(coap_get_code_decimal(&pkt));
    }

    print_str(" id=");
    print_u32_dec(coap_get_id(&pkt));

    if (coap_get_token_len(&pkt)) {
        print_str(" tkn=");
        print_bytes_hex(coap_get_token(&pkt), coap_get_token_len(&pkt));
    }

    /* parse CoAP options */
    coap_optpos_t opt = {
        .offset = coap_get_total_hdr_len(&pkt)
    };
    uint8_t *value;
    unsigned old_opt = 0;
    int opt_len;
    bool print_fmt = false;
    while ((opt_len = coap_opt_get_next(&pkt, &opt, &value, false)) >= 0) {
        const bool first_of_a_kind = opt.opt_num != old_opt;
        switch (opt.opt_num) {
        case COAP_OPT_URI_HOST:
            print_str(" host=");
            print((char *)value, opt_len);
            break;
        case COAP_OPT_URI_PATH:
            if (first_of_a_kind) {
                print_str(" ");
            }
            print_str("/");
            print((char *)value, opt_len);
            break;
        case COAP_OPT_URI_QUERY:
            print_str(first_of_a_kind ? "?" : "&");
            print((char *)value, opt_len);
            break;
        case COAP_OPT_CONTENT_FORMAT:
            print_fmt = true;
            break;
        case COAP_OPT_BLOCK1:
        case COAP_OPT_BLOCK2:
            _print_coap_block(&pkt, opt.opt_num);
            break;
        case COAP_OPT_NO_RESPONSE:
            print_str(" no-resp");
            break;
        case COAP_OPT_OBSERVE:
            print_str(" obs=");
            print_bytes_hex(value, opt_len);
            break;
        case COAP_OPT_ETAG:
            print_str(" etag=");
            print_bytes_hex(value, opt_len);
            break;
        case COAP_OPT_REQUEST_TAG:
            print_str(" rtag=");
            print_bytes_hex(value, opt_len);
            break;
        default:
            print_str(" opt=");
            print_u32_dec(opt.opt_num);
            break;
        }
        old_opt = opt.opt_num;
    }

    if (print_fmt) {
        _print_coap_format(coap_get_content_type(&pkt));
    }

    print_str(" (");
    print_u32_dec(pkt.payload_len);
    print_str(" bytes)\n");

    return true;
}

static bool _try_parse_udp(uint16_t port, const void *payload, size_t payload_len)
{
    switch (port) {
    case COAP_PORT:
        if (_dump_coap(payload, payload_len)) {
            return true;
        }
        break;
    }

    return false;
}

static void _dump_udp(const udp_hdr_t *hdr, size_t payload_len, gnrc_pktsnip_t *next)
{
    const void *payload;

    if (next) {
        payload     = next->data;
        payload_len = next->size;
    } else {
        payload = hdr + 1;
    }

    print_str("  UDP [");
    print_u32_dec(byteorder_ntohs(hdr->src_port));
    print_str(" -> ");
    print_u32_dec(byteorder_ntohs(hdr->dst_port));
    print_str("]\n");

    /* we can be either client or server */
    if (_try_parse_udp(byteorder_ntohs(hdr->src_port), payload, payload_len) ||
        _try_parse_udp(byteorder_ntohs(hdr->dst_port), payload, payload_len)) {
        return;
    }

    print_str("\t(");
    print_u32_dec(payload_len);
    print_str(" bytes)\n");
}

static void _dump_tcp(const tcp_hdr_t *hdr, size_t payload_len)
{
    print_str("  TCP [");
    print_u32_dec(byteorder_ntohs(hdr->src_port));
    print_str(" -> ");
    print_u32_dec(byteorder_ntohs(hdr->dst_port));
    print_str("]\n");

    /* TODO: parse TCP protocols */

    printf("\t(");
    print_u32_dec(payload_len);
    print_str(" bytes)\n");
}

static bool _is_extopt(uint8_t next_header)
{
    switch (next_header) {
    case PROTNUM_IPV6_EXT_HOPOPT:
        print_str(" hopopt");
        return true;
    case PROTNUM_IPV6_EXT_RH:
        print_str(" rh");
        return true;
    case PROTNUM_IPV6_EXT_FRAG:
        print_str(" frag");
        return true;
    case PROTNUM_IPV6_EXT_ESP:
        print_str(" esp");
        return true;
    case PROTNUM_IPV6_EXT_AH:
        print_str(" ah");
        return true;
    case PROTNUM_IPV6_EXT_DST:
        print_str(" dst");
        return true;
    case PROTNUM_IPV6_EXT_MOB:
        print_str(" mob");
        return true;
    }

    return false;
}

static void _dump_ipv4(const ipv4_hdr_t *hdr, size_t payload_len, bool rx)
{
    const ipv4_addr_t *me = rx ? &hdr->dst : &hdr->src;
    const ipv4_addr_t *they = rx ? &hdr->src : &hdr->dst;
    const void *payload = hdr + 1;

    ipv4_addr_print(me);
    print_str(rx ? " <= " : " => ");
    ipv4_addr_print(they);

    switch (hdr->protocol) {
    case PROTNUM_UDP:
        _dump_udp(payload, payload_len - sizeof(udp_hdr_t), NULL);
        break;
    case PROTNUM_TCP:
        _dump_tcp(payload, payload_len - sizeof(tcp_hdr_t));
        break;
    default:
        print_str("\tunknown protocol ");
        print_u32_dec(hdr->protocol);
        printf(" (");
        print_u32_dec(payload_len);
        print_str(" bytes)\n");
    }
}

static void _dump_ipv6(const ipv6_hdr_t *hdr, size_t payload_len, bool rx)
{
    const ipv6_addr_t *me = rx ? &hdr->dst : &hdr->src;
    const ipv6_addr_t *they = rx ? &hdr->src : &hdr->dst;

    ipv6_addr_print(me);
    print_str(rx ? " <= " : " => ");
    ipv6_addr_print(they);

    if (payload_len) {
        const void *payload = hdr + 1;
        uint8_t next_header = hdr->nh;

        /* skip IPv6 extension headers */
        while (_is_extopt(next_header)) {
            const struct {
                uint8_t nh;
                uint8_t opt_len;
            } *extopt = payload;

            next_header = extopt->nh;
            payload = (uint8_t *)payload + extopt->opt_len + 8;
            payload_len -= extopt->opt_len + 8;
        }

        switch (next_header) {
        case PROTNUM_ICMPV6:
            _dump_icmpv6(payload, payload_len);
            break;
        case PROTNUM_UDP:
            _dump_udp(payload, payload_len - sizeof(udp_hdr_t), NULL);
            break;
        case PROTNUM_TCP:
            _dump_tcp(payload, payload_len - sizeof(tcp_hdr_t));
            break;
        case PROTNUM_IPV4:
            if (IS_USED(MODULE_GNRC_PKTSHARK_4IN6)) {
                /* 4in6 */
                print_str("\n\t");
                _dump_ipv4(payload, payload_len - sizeof(ipv4_hdr_t), rx);
                break;
            }
            /* fall-through */
        default:
            print_str("\tunknown next header type ");
            print_u32_dec(next_header);
            print_str("\n");
        }
    }
}

static void _dump_snip(gnrc_pktsnip_t *pkt, bool rx)
{
    if (pkt->type <= 0) {
        return;
    }

    switch (pkt->type) {
    case GNRC_NETTYPE_IPV6:
        _dump_ipv6(pkt->data, pkt->size - sizeof(ipv6_hdr_t), rx);
        break;
#ifdef MODULE_GNRC_NETTYPE_ICMPV6
    case GNRC_NETTYPE_ICMPV6:
        _dump_icmpv6(pkt->data, pkt->size);
        break;
#endif
#ifdef MODULE_GNRC_NETTYPE_UDP
    case GNRC_NETTYPE_UDP:
        _dump_udp(pkt->data, pkt->size - sizeof(udp_hdr_t), pkt->next);
        break;
#endif
#ifdef MODULE_GNRC_NETTYPE_TCP
    case GNRC_NETTYPE_TCP:
        _dump_tcp(pkt->data, pkt->size - sizeof(tcp_hdr_t));
        break;
#endif
    default:
        print_str("\tunknown type ");
        print_u32_dec(pkt->type);
        print_str("\n");
        break;
    }
}

static void _dump(gnrc_pktsnip_t *pkt, bool rx)
{
    gnrc_pktsnip_t *snip = pkt;
    gnrc_pktsnip_t *netif = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_NETIF);

    print_str("\n");
    if (netif) {
        gnrc_netif_hdr_t *hdr = netif->data;
        print_u32_dec(hdr->if_pid);
        print_str(") ");
    }

    while (snip != NULL) {
        _dump_snip(snip, rx);
        snip = snip->next;
    }

    gnrc_pktbuf_release(pkt);
}

static void *_eventloop(void *arg)
{
    (void)arg;
    msg_t msg, reply;

    /* setup the message queue */
    msg_init_queue(_msg_queue, GNRC_PKTSHARK_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                _dump(msg.content.ptr, true);
                break;
            case GNRC_NETAPI_MSG_TYPE_SND:
                _dump(msg.content.ptr, false);
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            default:
                puts("gnrc_pktshark: received something unexpected");
                break;
        }
    }

    /* never reached */
    return NULL;
}

static void _set_capture(bool enable)
{
    static gnrc_netreg_entry_t dump;

    assume(gnrc_pktshark_pid != KERNEL_PID_UNDEF);
    if (enable && !dump.target.pid) {
        dump = (gnrc_netreg_entry_t) {
            .target = {
                .pid = gnrc_pktshark_pid,
            },
            .demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL,
        };
        gnrc_netreg_register(GNRC_NETTYPE_IPV6, &dump);
    } else if (!enable && dump.target.pid) {
        gnrc_netreg_unregister(GNRC_NETTYPE_IPV6, &dump);
        dump.target.pid = 0;
    }
}

static void gnrc_pktshark_init(void)
{
    gnrc_pktshark_pid = thread_create(_stack, sizeof(_stack), GNRC_PKTSHARK_PRIO, 0,
                                      _eventloop, NULL, "pktshark");
    if (IS_USED(MODULE_AUTO_INIT_GNRC_PKTSHARK)) {
        _set_capture(true);
    }
}
AUTO_INIT(gnrc_pktshark_init, AUTO_INIT_PRIO_MOD_GNRC_PKTSHARK);

#ifdef MODULE_SHELL_CMD_GNRC_PKTSHARK
static int cmd_pktshark(int argc, char **argv)
{
    if (argc < 2) {
        goto usage;
    }

    if (!strcmp("off", argv[1])) {
        _set_capture(0);
        return 0;
    }
    if (!strcmp("on", argv[1])) {
        _set_capture(1);
        return 0;
    }

usage:
    printf("usage: %s <on|off>\n", argv[0]);
    return -1;
}
SHELL_COMMAND(pktshark, "pretty-print network traffic", cmd_pktshark);
#endif
