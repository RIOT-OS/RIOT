/*
 * Copyright (C) 2016 José Ignacio Alamos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_gnrc_ppp
 * @file
 * @brief       Implementation of PPP's LCP protocol
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include "net/gnrc/ppp/lcp.h"
#include "net/gnrc/ppp/ppp.h"
#include <inttypes.h>
#include "net/gnrc/ppp/opt.h"
#include "net/gnrc/pkt.h"
#include "net/ppp/hdr.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"
#include <errno.h>
#include "net/gnrc/ppp/fsm.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

#define OPT_SIZE_MRU (2)        /**< size of MRU option */
#define OPT_SIZE_AUTH_PAP (2)   /**< size of PAP option */
#define OPT_SIZE_ACCM (4)       /**< size of ACCM option */

static lcp_t static_lcp;
static fsm_conf_t *lcp_get_conf_by_code(ppp_fsm_t *cp, uint8_t code)
{
    switch (code) {
        case LCP_OPT_MRU:
            return &cp->conf[LCP_MRU];
        case LCP_OPT_ACCM:
            return &cp->conf[LCP_ACCM];
        case LCP_OPT_AUTH:
            return &cp->conf[LCP_AUTH];
        default:
            return NULL;
    }
}

uint8_t lcp_mru_is_valid(ppp_option_t *opt)
{
    uint8_t *payload;

    ppp_opt_get_payload(opt, (void **) &payload);
    uint16_t u16 = ((*payload) << 8) + *(payload + 1);
    if (u16 > LCP_MAX_MRU) {
        return false;
    }
    return true;
}

uint8_t lcp_mru_build_nak_opts(uint8_t *buf)
{
    uint8_t len = OPT_HDR_SIZE + OPT_SIZE_MRU;
    ppp_option_t *opt = (ppp_option_t *) buf;
    network_uint16_t mru = byteorder_htons(LCP_DEFAULT_MRU);

    if (opt) {
        ppp_opt_set_type(opt, LCP_OPT_MRU);
        ppp_opt_set_length(opt, len);
        ppp_opt_set_payload(opt, &mru, sizeof(network_uint16_t));
    }
    return len;
}


void lcp_mru_set(ppp_fsm_t *lcp, ppp_option_t *opt, uint8_t peer)
{
    uint8_t *payload;

    ppp_opt_get_payload(opt, (void **) &payload);
    if (peer) {
        ((lcp_t *) lcp)->peer_mru = byteorder_ntohs(*((network_uint16_t *) payload));
    }
    else {
        ((lcp_t *) lcp)->mru = byteorder_ntohs(*((network_uint16_t *) payload));
    }
}


uint8_t lcp_accm_is_valid(ppp_option_t *opt)
{
    /*Flags are always valid*/
    return true;
}

uint8_t lcp_accm_build_nak_opts(uint8_t *buf)
{
    /* Never called */
    return true;
}

void lcp_accm_set(ppp_fsm_t *lcp, ppp_option_t *opt, uint8_t peer)
{
    gnrc_pppdev_t *dev = ((ppp_protocol_t *) lcp)->pppdev;
    uint8_t *payload;

    ppp_opt_get_payload(opt, (void **) &payload);
    if (peer) {
        dev->netdev->driver->set(dev->netdev, NETOPT_PPP_ACCM_RX, (void *) payload, sizeof(uint32_t));
    }
    else {
        dev->netdev->driver->set(dev->netdev, NETOPT_PPP_ACCM_TX, (void *) payload, sizeof(uint32_t));
    }
}

uint8_t lcp_auth_is_valid(ppp_option_t *opt)
{
    network_uint16_t *u16;

    ppp_opt_get_payload(opt, (void **) &u16);
    uint16_t val = byteorder_ntohs(*u16);

    /*Only accept PAP*/
    if (val == PPPTYPE_PAP) {
        return true;
    }

    return false;
}

uint8_t lcp_auth_build_nak_opts(uint8_t *buf)
{
    uint8_t len = OPT_HDR_SIZE + OPT_SIZE_AUTH_PAP;
    ppp_option_t *opt = (ppp_option_t *) buf;
    network_uint16_t protnum = byteorder_htons(PPPTYPE_PAP);

    if (opt) {
        ppp_opt_set_type(opt, LCP_OPT_AUTH);
        ppp_opt_set_length(opt, len);
        ppp_opt_set_payload(opt, &protnum, sizeof(network_uint16_t));
    }
    return len;
}

void lcp_auth_set(ppp_fsm_t *lcp, ppp_option_t *opt, uint8_t peer)
{
    lcp_t *l = (lcp_t *) lcp;

    if (peer) {
        l->local_auth = AUTH_PAP;
    }
    else {
        l->remote_auth = AUTH_PAP;
    }
}

static void lcp_config_init(ppp_fsm_t *lcp)
{
    lcp->conf = LCP_NUMOPTS ? ((lcp_t *) lcp)->lcp_opts : NULL;

    lcp->conf[LCP_MRU].type = LCP_OPT_MRU;
    lcp->conf[LCP_MRU].default_value = byteorder_htonl(LCP_DEFAULT_MRU);
    lcp->conf[LCP_MRU].size = OPT_SIZE_MRU;
    lcp->conf[LCP_MRU].flags = 0;
    lcp->conf[LCP_MRU].next = &lcp->conf[LCP_ACCM];
    lcp->conf[LCP_MRU].is_valid = &lcp_mru_is_valid;
    lcp->conf[LCP_MRU].build_nak_opts = &lcp_mru_build_nak_opts;
    lcp->conf[LCP_MRU].set = &lcp_mru_set;

    lcp->conf[LCP_ACCM].type = LCP_OPT_ACCM;
    lcp->conf[LCP_ACCM].default_value = byteorder_htonl(LCP_DEFAULT_ACCM);
    lcp->conf[LCP_ACCM].size = OPT_SIZE_ACCM;
    lcp->conf[LCP_ACCM].flags = 0;
    lcp->conf[LCP_ACCM].next = &lcp->conf[LCP_AUTH];
    lcp->conf[LCP_ACCM].is_valid = &lcp_accm_is_valid;
    lcp->conf[LCP_ACCM].build_nak_opts = &lcp_accm_build_nak_opts;
    lcp->conf[LCP_ACCM].set = &lcp_accm_set;

    lcp->conf[LCP_AUTH].type = LCP_OPT_AUTH;
    lcp->conf[LCP_AUTH].default_value = byteorder_htonl(LCP_DEFAULT_AUTH);
    lcp->conf[LCP_AUTH].size = OPT_SIZE_AUTH_PAP;
    lcp->conf[LCP_AUTH].flags = 0;
    lcp->conf[LCP_AUTH].next = NULL;
    lcp->conf[LCP_AUTH].is_valid = &lcp_auth_is_valid;
    lcp->conf[LCP_AUTH].build_nak_opts = &lcp_auth_build_nak_opts;
    lcp->conf[LCP_AUTH].set = &lcp_auth_set;
}


int lcp_handler(struct ppp_protocol_t *protocol, uint8_t ppp_event, void *args)
{
    ppp_fsm_t *lcp = (ppp_fsm_t *) protocol;

    if (ppp_event == PPP_MONITOR) {
        /*Send Echo Request*/
        DEBUG("gnrc_ppp: Sending echo request (link monitor)");
        gnrc_pktsnip_t *pkt = pkt_build(GNRC_NETTYPE_LCP, PPP_ECHO_REQ, ((lcp_t *) lcp)->monitor_id++, NULL);
        gnrc_ppp_send(protocol->pppdev, pkt);
        return 0;
    }
    else if (ppp_event == PPP_UL_FINISHED) {
        send_ppp_event(&protocol->msg, ppp_msg_set(protocol->lower_layer, PPP_UL_FINISHED));
        return 0;
    }
    else {
        return fsm_handle_ppp_msg(protocol, ppp_event, args);
    }
}

int lcp_init(gnrc_pppdev_t *ppp_dev, ppp_protocol_t *protocol)
{
    ppp_fsm_t *lcp = (ppp_fsm_t *) protocol;

    ppp_protocol_init((ppp_protocol_t *) lcp, ppp_dev, lcp_handler, PROT_LCP);
    fsm_init(ppp_dev, lcp);
    lcp_config_init(lcp);

    lcp->supported_codes = FLAG_CONF_REQ | FLAG_CONF_ACK | FLAG_CONF_NAK | FLAG_CONF_REJ | FLAG_TERM_REQ | FLAG_TERM_ACK | FLAG_CODE_REJ | FLAG_ECHO_REQ | FLAG_ECHO_REP | FLAG_DISC_REQ;
    ((lcp_t *)lcp)->pr_id = 0;
    lcp->prottype = GNRC_NETTYPE_LCP;
    lcp->restart_timer = LCP_RESTART_TIMER;
    lcp->get_conf_by_code = &lcp_get_conf_by_code;
    ((ppp_protocol_t *) lcp)->lower_layer = PROT_DCP;
    ((ppp_protocol_t *) lcp)->upper_layer = PROT_AUTH;
    ((lcp_t *) lcp)->mru = LCP_DEFAULT_MRU;
    ((lcp_t *) lcp)->peer_mru = LCP_DEFAULT_MRU;
    ((lcp_t *) lcp)->remote_auth = 0;
    ((lcp_t *) lcp)->local_auth = 0;
    ((lcp_t *) lcp)->monitor_id = 0;
    return 0;
}

ppp_protocol_t *lcp_get_static_pointer(void)
{
    return (ppp_protocol_t *) &static_lcp;
}
