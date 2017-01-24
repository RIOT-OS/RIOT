/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 */

#include "net/icmpv6.h"
#include "net/gnrc/ipv6.h"
#include "net/gnrc/icmpv6.h"
#include "net/gnrc.h"
#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/rpl/dodag.h"
#include "net/gnrc/rpl/p2p.h"
#include "net/gnrc/rpl/p2p_structs.h"
#include "net/gnrc/rpl/p2p_dodag.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

#define GNRC_RPL_P2P_RDO_LEN            (18)
#define GNRC_RPL_P2P_RDO_FLAGS_LIFETIME (6)
#define GNRC_RPL_P2P_RDO_FLAGS_HBH      (6)
#define GNRC_RPL_P2P_RDO_FLAGS_REPLY    (7)
#define GNRC_RPL_P2P_RDO_FLAGS_ADDRNUM  (4)
#define GNRC_RPL_P2P_RDO_FLAGS_COMPR    (0x0F)
#define GNRC_RPL_P2P_RDO_FLAGS_MAXRANK  (0x3F)
#define GNRC_RPL_P2P_RDO_FLAGS_NEXT_HOP (GNRC_RPL_P2P_RDO_FLAGS_MAXRANK)
#define GNRC_RPL_P2P_DRO_FLAGS_SEQ      (12)
#define GNRC_RPL_P2P_DRO_FLAGS_ACK      (14)
#define GNRC_RPL_P2P_DRO_FLAGS_STOP     (15)

gnrc_rpl_p2p_ext_t gnrc_rpl_p2p_exts[GNRC_RPL_P2P_EXTS_NUMOF];
const uint8_t gnrc_rpl_p2p_lifetime_lookup[4] = { 1, 4, 16, 64 };

void gnrc_rpl_p2p_update(void)
{
    gnrc_rpl_p2p_ext_t *p2p_ext;
    for (uint8_t i = 0; i < GNRC_RPL_P2P_EXTS_NUMOF; ++i) {
        p2p_ext = &gnrc_rpl_p2p_exts[i];
        if ((p2p_ext->state) && (p2p_ext->lifetime_sec > 0)) {
            p2p_ext->lifetime_sec -= GNRC_RPL_LIFETIME_UPDATE_STEP;
            if (p2p_ext->lifetime_sec <= 0) {
                gnrc_rpl_dodag_remove_all_parents(p2p_ext->dodag);
                p2p_ext->dodag->instance->cleanup = GNRC_RPL_CLEANUP_TIME;
                continue;
            }
            p2p_ext->dro_delay -= GNRC_RPL_LIFETIME_UPDATE_STEP;
            if (p2p_ext->reply && (p2p_ext->dro_delay < 0) && (p2p_ext->for_me)) {
                gnrc_rpl_p2p_send_DRO(NULL, p2p_ext);
            }
        }
    }
}

gnrc_rpl_instance_t *gnrc_rpl_p2p_root_init(uint8_t instance_id, ipv6_addr_t *dodag_id,
                                            ipv6_addr_t *target, bool gen_inst_id)
{
    if (gen_inst_id) {
        instance_id = gnrc_rpl_gen_instance_id(true);
    }

    gnrc_rpl_dodag_t *dodag = NULL;
    gnrc_rpl_instance_t *instance = gnrc_rpl_root_instance_init(instance_id, dodag_id,
                                                                GNRC_RPL_P2P_MOP);

    if (!instance) {
        return NULL;
    }

    dodag = &instance->dodag;

    instance->max_rank_inc = 0;
    dodag->dtsn = 0;
    dodag->prf = 0;
    dodag->dio_interval_doubl = GNRC_RPL_DEFAULT_DIO_INTERVAL_DOUBLINGS;
    dodag->dio_min = GNRC_RPL_P2P_DEFAULT_DIO_INTERVAL_MIN;
    dodag->dio_redun = GNRC_RPL_P2P_DEFAULT_DIO_REDUNDANCY_CONSTANT;
    dodag->default_lifetime = GNRC_RPL_P2P_DEFAULT_LIFETIME;
    dodag->lifetime_unit = GNRC_RPL_P2P_LIFETIME_UNIT;
    dodag->version = 0;
    dodag->grounded = 1;
    dodag->node_status = GNRC_RPL_ROOT_NODE;
    dodag->my_rank = GNRC_RPL_ROOT_RANK;
    dodag->dio_opts |= GNRC_RPL_REQ_DIO_OPT_DODAG_CONF;
    dodag->dio_opts &= ~GNRC_RPL_REQ_DIO_OPT_PREFIX_INFO;

    gnrc_rpl_p2p_ext_t *p2p_ext = gnrc_rpl_p2p_ext_get(dodag);
    p2p_ext->target = *target;
    p2p_ext->compr = GNRC_RPL_P2P_COMPR;
    p2p_ext->routes_numof = 0;
    p2p_ext->hop_by_hop = true;
    p2p_ext->reply = true;
    p2p_ext->lifetime_enc = GNRC_RPL_P2P_LIFETIME;
    p2p_ext->lifetime_sec = gnrc_rpl_p2p_lifetime_lookup[p2p_ext->lifetime_enc];
    p2p_ext->maxrank = GNRC_RPL_P2P_MAX_RANK;
    p2p_ext->dro_delay = -1;

    trickle_start(gnrc_rpl_pid, &dodag->trickle, GNRC_RPL_MSG_TYPE_TRICKLE_INTERVAL,
                  GNRC_RPL_MSG_TYPE_TRICKLE_CALLBACK, (1 << dodag->dio_min),
                  dodag->dio_interval_doubl, dodag->dio_redun);

    return instance;
}

gnrc_pktsnip_t *gnrc_rpl_p2p_rdo_build(gnrc_pktsnip_t *pkt, gnrc_rpl_p2p_ext_t *p2p_ext)
{
    gnrc_rpl_p2p_opt_rdo_t *rdo;
    gnrc_pktsnip_t *opt_snip;
    size_t addr_len = (sizeof(ipv6_addr_t) - p2p_ext->compr);
    for (uint8_t i = p2p_ext->addr_numof; i > 0; i--) {
        if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, addr_len, GNRC_NETTYPE_UNDEF)) == NULL) {
            DEBUG("RPL: BUILD RDO - no space left in packet buffer\n");
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        memcpy(opt_snip->data, &p2p_ext->addr_vec[i-1], addr_len);
        pkt = opt_snip;
    }

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_p2p_opt_rdo_t),
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL: BUILD RDO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    rdo = opt_snip->data;
    rdo->type = GNRC_RPL_P2P_OPT_RDO;
    rdo->length = GNRC_RPL_P2P_RDO_LEN +
                  p2p_ext->addr_numof * (sizeof(ipv6_addr_t) - p2p_ext->compr);
    rdo->compr_flags = (p2p_ext->reply << GNRC_RPL_P2P_RDO_FLAGS_REPLY) |
                        (p2p_ext->hop_by_hop << GNRC_RPL_P2P_RDO_FLAGS_HBH)
                        | ((p2p_ext->routes_numof & 0x3) << GNRC_RPL_P2P_RDO_FLAGS_ADDRNUM)
                        | (p2p_ext->compr & GNRC_RPL_P2P_RDO_FLAGS_COMPR);
    rdo->lmn = ((p2p_ext->lifetime_enc & 0x3) << GNRC_RPL_P2P_RDO_FLAGS_LIFETIME) |
                (p2p_ext->maxrank & GNRC_RPL_P2P_RDO_FLAGS_MAXRANK);
    rdo->target = p2p_ext->target;
    return opt_snip;
}

void gnrc_rpl_p2p_rdo_parse(gnrc_rpl_p2p_opt_rdo_t *rdo, gnrc_rpl_p2p_ext_t *p2p_ext)
{
    DEBUG("RPL: Route Discovery DIO option parsed\n");

    uint8_t addr_num = (rdo->length - GNRC_RPL_P2P_RDO_LEN)
                        / (sizeof(ipv6_addr_t) - p2p_ext->compr);
    if (addr_num >= GNRC_RPL_P2P_ADDR_VEC_NUMOF) {
        DEBUG("RPL: cannot parse RDO - too many hops\n");
        return;
    }

    p2p_ext->for_me = (gnrc_ipv6_netif_find_by_addr(NULL, &rdo->target) != KERNEL_PID_UNDEF);

    p2p_ext->reply = (rdo->compr_flags & (1 << GNRC_RPL_P2P_RDO_FLAGS_REPLY))
                      >> GNRC_RPL_P2P_RDO_FLAGS_REPLY;
    p2p_ext->hop_by_hop = (rdo->compr_flags & (1 << GNRC_RPL_P2P_RDO_FLAGS_HBH))
                           >> GNRC_RPL_P2P_RDO_FLAGS_HBH;
    p2p_ext->routes_numof = (rdo->compr_flags & (0x3 << GNRC_RPL_P2P_RDO_FLAGS_ADDRNUM))
                             >> GNRC_RPL_P2P_RDO_FLAGS_ADDRNUM;
    p2p_ext->compr = rdo->compr_flags & GNRC_RPL_P2P_RDO_FLAGS_COMPR;
    p2p_ext->lifetime_enc = (rdo->lmn & (0x3 << GNRC_RPL_P2P_RDO_FLAGS_LIFETIME))
                             >> GNRC_RPL_P2P_RDO_FLAGS_LIFETIME;
    if (p2p_ext->lifetime_sec == INT8_MIN) {
        p2p_ext->lifetime_sec = gnrc_rpl_p2p_lifetime_lookup[p2p_ext->lifetime_enc];
    }
    p2p_ext->maxrank = rdo->lmn & GNRC_RPL_P2P_RDO_FLAGS_MAXRANK;
    p2p_ext->target = rdo->target;

    memset(&p2p_ext->addr_vec, 0, sizeof(ipv6_addr_t) * GNRC_RPL_P2P_ADDR_VEC_NUMOF);
    p2p_ext->addr_numof = 0;
    uint8_t *tmp = (uint8_t *) (rdo + 1);
    uint8_t *addr = NULL;
    uint8_t addr_len = sizeof(ipv6_addr_t) - p2p_ext->compr;
    uint8_t i = 0;
    for (i = 0; i < addr_num; i++) {
        addr = ((uint8_t *) &p2p_ext->addr_vec[i]) + p2p_ext->compr;
        memcpy(addr, tmp, addr_len);
        tmp += addr_len;
        p2p_ext->addr_numof++;
    }

    if (!p2p_ext->for_me) {
        ipv6_addr_t *me = NULL;
        if(gnrc_ipv6_netif_find_by_prefix(&me, &p2p_ext->dodag->dodag_id) == KERNEL_PID_UNDEF) {
            DEBUG("RPL: no address configured\n");
            return;
        }
        addr = ((uint8_t *) &p2p_ext->addr_vec[i]) + p2p_ext->compr;
        memcpy(addr, ((uint8_t *) me) + p2p_ext->compr, addr_len);
        p2p_ext->addr_numof++;
    }
    else if (p2p_ext->reply) {
        p2p_ext->stop = true;
        p2p_ext->dro_ack = true;
        p2p_ext->dro_delay = GNRC_RPL_P2P_DRO_DELAY;
    }
}

static gnrc_pktsnip_t *_build_initial_DRO(gnrc_rpl_p2p_ext_t *p2p_ext)
{
    gnrc_pktsnip_t *pkt = NULL, *opt_snip = NULL;
    gnrc_rpl_p2p_dro_t *dro = NULL;
    gnrc_rpl_p2p_opt_rdo_t *rdo = NULL;
    size_t addr_len = (sizeof(ipv6_addr_t) - p2p_ext->compr);
    uint8_t addr_size = p2p_ext->addr_numof * addr_len;

    for (uint8_t i = p2p_ext->addr_numof; i > 0; i--) {
        if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, addr_len, GNRC_NETTYPE_UNDEF)) == NULL) {
            DEBUG("RPL-P2P: cannot add addresses to RDO - no space left in packet buffer\n");
            gnrc_pktbuf_release(pkt);
            return NULL;
        }
        memcpy(opt_snip->data, &p2p_ext->addr_vec[i-1], addr_len);
        pkt = opt_snip;
    }

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_p2p_opt_rdo_t),
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL-P2P: cannot allocate DRO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }

    rdo = opt_snip->data;
    rdo->type = GNRC_RPL_P2P_OPT_RDO;
    rdo->length = GNRC_RPL_P2P_RDO_LEN + addr_size;
    rdo->compr_flags = (p2p_ext->hop_by_hop << GNRC_RPL_P2P_RDO_FLAGS_HBH) |
                       (p2p_ext->compr & GNRC_RPL_P2P_RDO_FLAGS_COMPR);
    /* rdo->length does not include the first two bytes, thus we have to add them manually */
    rdo->lmn = (((rdo->length + 2 - sizeof(*rdo)) / addr_len) & GNRC_RPL_P2P_RDO_FLAGS_NEXT_HOP);
    rdo->target = p2p_ext->target;
    pkt = opt_snip;

    if ((opt_snip = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_rpl_p2p_dro_t),
                                    GNRC_NETTYPE_UNDEF)) == NULL) {
        DEBUG("RPL-P2P: cannot allocate RDO - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }
    dro = opt_snip->data;
    dro->instance_id = p2p_ext->dodag->instance->id;
    dro->version_number = 0;
    dro->flags_rev = byteorder_htons((((p2p_ext->stop << 1) | (p2p_ext->dro_ack << 0))
                                      << GNRC_RPL_P2P_DRO_FLAGS_ACK) |
                                     ((p2p_ext->dro_seq & 0x3) << GNRC_RPL_P2P_DRO_FLAGS_SEQ));
    dro->dodag_id = p2p_ext->dodag->dodag_id;
    pkt = opt_snip;

    if ((opt_snip = gnrc_icmpv6_build(pkt, ICMPV6_RPL_CTRL, GNRC_RPL_P2P_ICMPV6_CODE_DRO,
                                      sizeof(icmpv6_hdr_t))) == NULL) {
        DEBUG("RPL-P2P: cannot allocate ICMPv6 - no space left in packet buffer\n");
        gnrc_pktbuf_release(pkt);
        return NULL;
    }

    pkt = opt_snip;

    return pkt;
}

void gnrc_rpl_p2p_send_DRO(gnrc_pktsnip_t *pkt, gnrc_rpl_p2p_ext_t *p2p_ext)
{
    assert(p2p_ext != NULL);

    if (pkt == NULL) {
        if ((pkt = _build_initial_DRO(p2p_ext)) == NULL) {
            DEBUG("RPL-P2P: BUILD INITIAL DRO - no space left in packet buffer\n");
            return;
        }
    }

    gnrc_rpl_send(pkt, p2p_ext->dodag->iface, NULL, NULL, &p2p_ext->dodag->dodag_id);

    return;
}

void gnrc_rpl_p2p_recv_DRO(gnrc_pktsnip_t *pkt, ipv6_addr_t *src)
{
    gnrc_pktsnip_t *icmpv6_snip = gnrc_pktbuf_mark(pkt, sizeof(icmpv6_hdr_t), GNRC_NETTYPE_ICMPV6);
    gnrc_pktsnip_t *dro_snip = gnrc_pktbuf_mark(pkt, sizeof(gnrc_rpl_p2p_dro_t),
                                                GNRC_NETTYPE_UNDEF);
    gnrc_pktsnip_t *rdo_snip = gnrc_pktbuf_mark(pkt, sizeof(gnrc_rpl_p2p_opt_rdo_t),
                                                GNRC_NETTYPE_UNDEF);
    gnrc_pktsnip_t *addr_snip = pkt;
    gnrc_rpl_instance_t *inst;
    gnrc_rpl_dodag_t *dodag;
    gnrc_rpl_p2p_ext_t *p2p_ext;
    gnrc_rpl_p2p_dro_t *dro;
    gnrc_rpl_p2p_opt_rdo_t *rdo;
    uint8_t *addr_vec;
    uint16_t flags;
    size_t addr_len;

    if (!rdo_snip || !dro_snip) {
        DEBUG("RPL-P2P: Error - No DRO or RDO received\n");
        gnrc_pktbuf_release(pkt);
        return;
    }

    dro = dro_snip->data;

    if ((inst = gnrc_rpl_instance_get(dro->instance_id)) == NULL) {
        DEBUG("RPL-P2P: Error - Instance (%d) does not exist\n", dro->instance_id);
        return;
    }

    dodag = &inst->dodag;

    if ((p2p_ext = gnrc_rpl_p2p_ext_get(dodag)) == NULL) {
        DEBUG("RPL-P2P: Error - No P2P-RPL DODAG extension found\n");
        return;
    }

    if (p2p_ext->for_me) {
        DEBUG("RPL-P2P: Ignore DRO\n");
        return;
    }

    flags = byteorder_ntohs(dro->flags_rev);
    p2p_ext->stop = (flags & (1 << GNRC_RPL_P2P_DRO_FLAGS_STOP)) >> GNRC_RPL_P2P_DRO_FLAGS_STOP;
    p2p_ext->dro_ack = (flags & (1 << GNRC_RPL_P2P_DRO_FLAGS_ACK)) >> GNRC_RPL_P2P_DRO_FLAGS_ACK;
    p2p_ext->dro_seq = (flags & (0x3 << GNRC_RPL_P2P_DRO_FLAGS_SEQ)) >> GNRC_RPL_P2P_DRO_FLAGS_SEQ;

    addr_len = sizeof(ipv6_addr_t) - p2p_ext->compr;
    ipv6_addr_t addr = p2p_ext->dodag->dodag_id;
    ipv6_addr_t *me = NULL;
    addr_vec = addr_snip->data;

    rdo = rdo_snip->data;

    if (rdo->lmn > 0) {
        rdo->lmn--;
        memcpy(&addr.u8[p2p_ext->compr], &addr_vec[addr_len * rdo->lmn], addr_len);
    }

    if (gnrc_ipv6_netif_find_by_addr(&me, &addr) == dodag->iface) {
        fib_add_entry(&gnrc_ipv6_fib_table, dodag->iface, p2p_ext->target.u8,
                      sizeof(ipv6_addr_t), 0x0, src->u8,
                      sizeof(ipv6_addr_t), FIB_FLAG_RPL_ROUTE,
                      p2p_ext->dodag->default_lifetime *
                      p2p_ext->dodag->lifetime_unit * MS_PER_SEC);

        if (p2p_ext->dodag->node_status != GNRC_RPL_ROOT_NODE) {
            if ((rdo_snip = gnrc_pktbuf_start_write(rdo_snip)) == NULL) {
                DEBUG("RPL-P2P: Error - Cannot allocate new RDO\n");
                return;
            }

            addr_snip->next = NULL;
            rdo_snip->next = addr_snip;
            dro_snip->next = rdo_snip;

            icmpv6_snip = gnrc_icmpv6_build(dro_snip, ICMPV6_RPL_CTRL, GNRC_RPL_P2P_ICMPV6_CODE_DRO,
                                            sizeof(icmpv6_hdr_t));
            if (icmpv6_snip == NULL) {
                DEBUG("RPL-P2P: cannot allocate ICMPv6 - no space left in packet buffer\n");
                gnrc_pktbuf_release(pkt);
                return;
            }

            gnrc_rpl_send(icmpv6_snip, p2p_ext->dodag->iface, NULL, NULL, &p2p_ext->dodag->dodag_id);
            return;
        }
    }

    gnrc_pktbuf_release(pkt);

    return;
}

/**
 * @}
 */
