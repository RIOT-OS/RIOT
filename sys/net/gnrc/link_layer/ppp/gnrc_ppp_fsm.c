/*
 * Copyright (C) 2015 José Ignacio Alamos <jialamos@uc.cl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_gnrc_ppp
 * @file
 * @brief       Implementation of the Option Negotiation Automaton FSM
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 *
 * @see <a href="https://tools.ietf.org/html/rfc1661#page-11">
 *				RFC 1661, page 11.
 *		</a>
 * @}
 */
#include <errno.h>

#include "msg.h"
#include "thread.h"
#include "net/gnrc.h"
#include "net/ppptype.h"
#include "net/gnrc/ppp/ppp.h"
#include "net/gnrc/ppp/lcp.h"
#include "net/gnrc/ppp/ipcp.h"
#include "net/gnrc/ppp/fsm.h"
#include "net/hdlc/hdr.h"
#include "net/ppp/hdr.h"
#include <errno.h>
#include <string.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

#define FOR_EACH_OPTION(opt, buf, size) \
    for (ppp_option_t *opt = (ppp_option_t *) buf; opt != NULL; opt = ppp_opt_get_next(opt, (ppp_option_t *) buf, size))

#define FOR_EACH_CONF(conf, head) \
    for (fsm_conf_t *conf = head; conf != NULL; conf = conf->next)

ppp_hdr_t *_get_ppp_hdr(gnrc_pktsnip_t *pkt)
{
    return pkt->type == GNRC_NETTYPE_UNDEF ? (ppp_hdr_t *) pkt->next->data : (ppp_hdr_t *) pkt->data;
}

int _pkt_has_payload(ppp_hdr_t *hdr)
{
    return byteorder_ntohs(hdr->length) > sizeof(ppp_hdr_t);
}

void set_timeout(ppp_fsm_t *cp, uint32_t time)
{
    ppp_target_t self = ((ppp_protocol_t *)cp)->id;

    send_ppp_event_xtimer(&((ppp_protocol_t *) cp)->msg, &cp->xtimer, ppp_msg_set(self, PPP_TIMEOUT), cp->restart_timer);
}

void _reset_cp_conf(fsm_conf_t *conf)
{
    FOR_EACH_CONF(c, conf){
        conf->value = conf->default_value;
    }
}
size_t _opts_size(fsm_conf_t *head_conf)
{
    size_t size = 0;

    FOR_EACH_CONF(conf, head_conf){
        if (conf->flags & OPT_ENABLED) {
            size += 2 + conf->size;
        }
    }
    return size;
}

void _write_opts(fsm_conf_t *head_conf, uint8_t *buf)
{
    int cursor = 0;

    FOR_EACH_CONF(conf, head_conf){
        if (conf->flags & OPT_ENABLED) {
            cursor += ppp_opt_fill(buf + cursor, conf->type, (&conf->value) + sizeof(uint32_t) - conf->size, conf->size);
        }
    }
}

gnrc_pktsnip_t *_build_conf_req_options(ppp_fsm_t *cp)
{
    /* Get size of outgoing options */
    size_t size = _opts_size(cp->conf);

    if (!size) {
        return NULL;
    }

    /* Write opts to pkt */
    gnrc_pktsnip_t *opts = gnrc_pktbuf_add(NULL, NULL, size, GNRC_NETTYPE_UNDEF);
    _write_opts(cp->conf, (uint8_t *) opts->data);
    return opts;
}

static uint8_t get_scnpkt_data(ppp_fsm_t *cp, gnrc_pktsnip_t *pkt, uint16_t *n)
{
    uint8_t rej_size = 0;
    uint8_t nak_size = 0;
    uint8_t curr_type;

    fsm_conf_t *curr_conf;
    uint8_t curr_size;

    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        curr_type = ppp_opt_get_type(opt);
        curr_conf = cp->get_conf_by_code(cp, curr_type);
        curr_size = ppp_opt_get_length(opt);
        if (curr_conf == NULL) {
            rej_size += curr_size;
        }
        else if (!curr_conf->is_valid(opt)) {
            nak_size += curr_conf->build_nak_opts(NULL);
        }
    }

    /* Append required options */
    FOR_EACH_CONF(conf, cp->conf){
        if (conf->flags & OPT_REQUIRED) {
            nak_size += conf->size;
        }
    }

    *n = rej_size ? rej_size : nak_size;
    return rej_size ? PPP_CONF_REJ : PPP_CONF_NAK;
}

static void build_nak_pkt(ppp_fsm_t *cp, gnrc_pktsnip_t *pkt, uint8_t *buf)
{
    fsm_conf_t *curr_conf;
    uint8_t curr_type;
    uint8_t cursor = 0;

    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        curr_type = ppp_opt_get_type(opt);
        curr_conf = cp->get_conf_by_code(cp, curr_type);

        if (curr_conf && !curr_conf->is_valid(opt)) {
            cursor += curr_conf->build_nak_opts(buf + cursor);
        }
    }
}

static void build_rej_pkt(ppp_fsm_t *cp, gnrc_pktsnip_t *pkt, uint8_t *buf)
{
    fsm_conf_t *curr_conf;

    uint8_t curr_type;
    uint16_t curr_size;

    uint8_t cursor = 0;

    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        curr_type = ppp_opt_get_type(opt);
        curr_conf = cp->get_conf_by_code(cp, curr_type);
        curr_size = ppp_opt_get_length(opt);

        if (curr_conf == NULL) {
            memcpy(buf + cursor, opt, curr_size);
            cursor += curr_size;
        }
    }
}

#if ENABLE_DEBUG
static void print_state(int state)
{
    switch (state) {
        case S_UNDEF:
            DEBUG("UNDEF");
            break;
        case S_INITIAL:
            DEBUG("INITIAL");
            break;
        case S_STARTING:
            DEBUG("STARTING");
            break;
        case S_CLOSED:
            DEBUG("CLOSED");
            break;
        case S_STOPPED:
            DEBUG("STOPPED");
            break;
        case S_CLOSING:
            DEBUG("CLOSING");
            break;
        case S_STOPPING:
            DEBUG("STOPPING");
            break;
        case S_REQ_SENT:
            DEBUG("REQ_SENT");
            break;
        case S_ACK_RCVD:
            DEBUG("ACK_RECV");
            break;
        case S_ACK_SENT:
            DEBUG("ACK_SENT");
            break;
        case S_OPENED:
            DEBUG("OPENED");
            break;
    }
}
static void print_event(uint8_t event)
{
    switch (event) {
        case E_UP:
            DEBUG("UP");
            break;
        case E_DOWN:
            DEBUG("DOWN");
            break;
        case E_OPEN:
            DEBUG("OPEN");
            break;
        case E_CLOSE:
            DEBUG("CLOSE");
            break;
        case E_TOp:
            DEBUG("TO+");
            break;
        case E_TOm:
            DEBUG("TO-");
            break;
        case E_RCRp:
            DEBUG("RCR+");
            break;
        case E_RCRm:
            DEBUG("RCR-");
            break;
        case E_RCA:
            DEBUG("RCA");
            break;
        case E_RCN:
            DEBUG("RCN");
            break;
        case E_RTR:
            DEBUG("RTR");
            break;
        case E_RTA:
            DEBUG("RTA");
            break;
        case E_RUC:
            DEBUG("RUC");
            break;
        case E_RXJp:
            DEBUG("RXJ+");
            break;
        case E_RXJm:
            DEBUG("RXJ-");
            break;
        case E_RXR:
            DEBUG("RXR");
            break;
    }
}
static void print_transition(int state, uint8_t event, int next_state)
{
    DEBUG("From state ");
    print_state(state);
    DEBUG(" with event ");
    print_event(event);
    DEBUG(". Next state is ");
    print_state(next_state);
    DEBUG("\n");
}
#endif

ppp_target_t _fsm_upper_layer(ppp_fsm_t *cp)
{
    return ((ppp_protocol_t *) cp)->upper_layer;
}

ppp_target_t _fsm_lower_layer(ppp_fsm_t *cp)
{
    return ((ppp_protocol_t *) cp)->lower_layer;
}
void tlu(ppp_fsm_t *cp, void *args)
{
    _reset_cp_conf(cp->conf);
    ((ppp_protocol_t *) cp)->state = PROTOCOL_UP;
    if (cp->on_layer_up) {
        cp->on_layer_up(cp);
    }
    send_ppp_event(&((ppp_protocol_t *)cp)->msg, ppp_msg_set(_fsm_upper_layer(cp), PPP_LINKUP));
    (void) cp;
}

void tld(ppp_fsm_t *cp, void *args)
{
    _reset_cp_conf(cp->conf);
    ((ppp_protocol_t *) cp)->state = PROTOCOL_DOWN;
    if (cp->on_layer_down) {
        cp->on_layer_down(cp);
    }
    send_ppp_event(&((ppp_protocol_t *) cp)->msg, ppp_msg_set(_fsm_upper_layer(cp), PPP_LINKDOWN));
    (void) cp;
}

void tls(ppp_fsm_t *cp, void *args)
{
    _reset_cp_conf(cp->conf);
    send_ppp_event(&((ppp_protocol_t *) cp)->msg, ppp_msg_set(_fsm_lower_layer(cp), PPP_UL_STARTED));
    (void) cp;
}

void tlf(ppp_fsm_t *cp, void *args)
{
    send_ppp_event(&((ppp_protocol_t *) cp)->msg, ppp_msg_set(_fsm_lower_layer(cp), PPP_UL_FINISHED));
    (void) cp;
}

void irc(ppp_fsm_t *cp, void *args)
{

    uint8_t cr = *((int *) args) & F_SCR;

    cp->restart_counter = cr ? PPP_MAX_CONFIG : PPP_MAX_TERMINATE;
}

void zrc(ppp_fsm_t *cp, void *args)
{
    cp->restart_counter = 0;
    set_timeout(cp, cp->restart_timer);
}


void scr(ppp_fsm_t *cp, void *args)
{

    /* Decrement configure counter */
    cp->restart_counter -= 1;

    /* Build options */
    gnrc_pktsnip_t *opts = _build_conf_req_options(cp);

    /*In case there are options, copy to sent opts*/
    if (opts) {
        memcpy(cp->cr_sent_opts, opts->data, opts->size);
        cp->cr_sent_size = opts->size;
    }

    /*Send configure request*/
    send_configure_request(((ppp_protocol_t *) cp)->pppdev, cp->prottype, ++cp->cr_sent_identifier, opts);
    set_timeout(cp, cp->restart_timer);
}

void sca(ppp_fsm_t *cp, void *args)
{
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) args;

    ppp_hdr_t *recv_ppp_hdr;

    gnrc_pktsnip_t *opts = NULL;

    recv_ppp_hdr = _get_ppp_hdr(pkt);

    if (_pkt_has_payload(recv_ppp_hdr)) {
        opts = gnrc_pktbuf_add(NULL, pkt->data, pkt->size, GNRC_NETTYPE_UNDEF);
    }

    send_configure_ack(((ppp_protocol_t *) cp)->pppdev, cp->prottype, recv_ppp_hdr->id, opts);
}

void scn(ppp_fsm_t *cp, void *args)
{
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) args;

    gnrc_pktsnip_t *opts;

    uint16_t scn_len;
    uint8_t type = get_scnpkt_data(cp, pkt, &scn_len);

    opts = gnrc_pktbuf_add(NULL, NULL, scn_len, GNRC_NETTYPE_UNDEF);

    switch (type) {
        case PPP_CONF_NAK:
            build_nak_pkt(cp, pkt, (uint8_t *)opts->data);
            send_configure_nak(((ppp_protocol_t *) cp)->pppdev, cp->prottype,((ppp_hdr_t *) pkt->next->data)->id, opts);
            break;
        case PPP_CONF_REJ:
            build_rej_pkt(cp, pkt, (uint8_t *) opts->data);
            send_configure_rej(((ppp_protocol_t *) cp)->pppdev, cp->prottype,((ppp_hdr_t *) pkt->next->data)->id, opts);
            break;
        default:
            DEBUG("gnrc_ppp: shouldn't be here...\n");
            break;
    }
}

void str(ppp_fsm_t *cp, void *args)
{

    send_terminate_req(((ppp_protocol_t *) cp)->pppdev, cp->prottype, cp->tr_sent_identifier++);
}

void sta(ppp_fsm_t *cp, void *args)
{
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) args;
    gnrc_pktsnip_t *recv_pkt = NULL;

    ppp_hdr_t *recv_ppp_hdr = _get_ppp_hdr(pkt);

    if (_pkt_has_payload(recv_ppp_hdr)) {
        recv_pkt = gnrc_pktbuf_add(NULL, pkt->data, pkt->size, GNRC_NETTYPE_UNDEF);
    }
    send_terminate_ack(((ppp_protocol_t *) cp)->pppdev, cp->prottype, recv_ppp_hdr->id, recv_pkt);
}
void scj(ppp_fsm_t *cp, void *args)
{
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) args;

    gnrc_pktsnip_t *payload = gnrc_pktbuf_add(NULL, pkt->data, pkt->size, cp->prottype);

    send_code_rej(((ppp_protocol_t *) cp)->pppdev, cp->prottype, cp->cr_sent_identifier++, payload);
}
void ser(ppp_fsm_t *cp, void *args)
{
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) args;
    gnrc_pktsnip_t *ppp_hdr = gnrc_pktbuf_mark(pkt, sizeof(ppp_hdr_t), cp->prottype);
    ppp_hdr_t *hdr = ppp_hdr->data;
    uint8_t id = hdr->id;

    uint8_t code = hdr->code;
    gnrc_pktsnip_t *data = NULL;

    if (pkt != ppp_hdr) {
        data = gnrc_pktbuf_add(NULL, pkt->data, pkt->size, GNRC_NETTYPE_UNDEF);
    }

    switch (code) {
        case PPP_ECHO_REQ:
            send_echo_reply(((ppp_protocol_t *) cp)->pppdev, cp->prottype, id, data);
            break;
        case PPP_ECHO_REP:
            break;
        case PPP_DISC_REQ:
            break;
    }

    /*Send PPP_LINK_ALIVE to lower layer*/
    send_ppp_event(&((ppp_protocol_t *) cp)->msg, ppp_msg_set(_fsm_lower_layer(cp), PPP_LINK_ALIVE));
}

/* Call functions depending on function flag*/
static void _event_action(ppp_fsm_t *cp, uint8_t event, gnrc_pktsnip_t *pkt)
{
    int flags;

    flags = actions[event][cp->state];

    if (flags & F_TLU) {
        tlu(cp, NULL);
    }
    if (flags & F_TLD) {
        tld(cp, NULL);
    }
    if (flags & F_TLS) {
        tls(cp, NULL);
    }
    if (flags & F_TLF) {
        tlf(cp, NULL);
    }
    if (flags & F_IRC) {
        irc(cp, (void *) &flags);
    }
    if (flags & F_ZRC) {
        zrc(cp, NULL);
    }
    if (flags & F_SCR) {
        scr(cp, (void *) pkt);
    }
    if (flags & F_SCA) {
        sca(cp, (void *) pkt);
    }
    if (flags & F_SCN) {
        scn(cp, (void *) pkt);
    }
    if (flags & F_STR) {
        str(cp, NULL);
    }
    if (flags & F_STA) {
        sta(cp, (void *) pkt);
    }
    if (flags & F_SCJ) {
        scj(cp, (void *) pkt);
    }
    if (flags & F_SER) {
        ser(cp, (void *) pkt);
    }
}

int trigger_fsm_event(ppp_fsm_t *cp, int event, gnrc_pktsnip_t *pkt)
{
    if (event < 0) {
        return -EBADMSG;
    }
    int next_state;
    next_state = state_trans[event][cp->state];
#if ENABLE_DEBUG
    DEBUG("%i> ", ((ppp_protocol_t *)cp)->id);
    print_transition(cp->state, event, next_state);
#endif

    /* Keep in same state if there's something wrong (RFC 1661) */
    if (next_state != S_UNDEF) {
        _event_action(cp, event, pkt);
        cp->state = next_state;
    }
    else {
        DEBUG("gnrc_ppp: fsm received illegal transition. \n");
    }
    /*Check if next state doesn't have a running timer*/
    if (cp->state < S_CLOSING || cp->state == S_OPENED) {
        xtimer_remove(&cp->xtimer);
    }
    return 0;
}

int fsm_init(gnrc_netdev2_t *ppp_dev, ppp_fsm_t *cp)
{
    cp->state = S_INITIAL;
    cp->cr_sent_identifier = 0;
    return 0;
}

static int _opt_is_ack(ppp_fsm_t *cp, ppp_option_t *opt)
{
    fsm_conf_t *curr_conf = NULL;

    curr_conf = cp->get_conf_by_code(cp, ppp_opt_get_type(opt));
    return curr_conf && curr_conf->is_valid(opt);
}

int handle_rcr(ppp_fsm_t *cp, gnrc_pktsnip_t *pkt)
{
    /* If the packet doesn't have options, it's considered as valid. */
    if (!pkt) {
        return E_RCRp;
    }

    /* Check if options are valid */
    if (ppp_conf_opts_valid(pkt, pkt->size) <= 0) {
        return -EBADMSG;
    }

    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        if (!_opt_is_ack(cp, opt)) {
            return E_RCRm;
        }
    }

    /*Check if there's an option that is required but not sent */
    uint8_t found;
    FOR_EACH_CONF(conf, cp->conf){
        if (!(conf->flags & OPT_REQUIRED)) {
            continue;
        }
        found = false;
        FOR_EACH_OPTION(opt, pkt->data, pkt->size){
            if (conf->type == ppp_opt_get_type(opt)) {
                found = true;
                break;
            }
        }

        if (!found) {
            return E_RCRm;
        }
    }

    /* Valid options... set them before SCA */
    fsm_conf_t *curr_conf;
    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        curr_conf = cp->get_conf_by_code(cp, ppp_opt_get_type(opt));
        if (curr_conf) {
            curr_conf->set(cp, opt, true);
        }
        else {
            DEBUG("gnrc_ppp: fsm: handle_rcr inconsistency in pkt. Shouldn't be here\n");
        }
    }

    return E_RCRp;
}

int handle_rca(ppp_fsm_t *cp, ppp_hdr_t *hdr, gnrc_pktsnip_t *pkt)
{
    uint8_t pkt_id = hdr->id;
    uint8_t pkt_length = byteorder_ntohs(hdr->length);

    void *opts = NULL;

    if (pkt) {
        if (ppp_conf_opts_valid(pkt, pkt->size) <= 0) {
            return -EBADMSG;
        }
        opts = pkt->data;
    }

    if (pkt_id != cp->cr_sent_identifier || (pkt && memcmp(cp->cr_sent_opts, opts, pkt_length - sizeof(ppp_hdr_t)))) {
        return -EBADMSG;
    }

    /*Write options in corresponding devices*/
    if (pkt) {
        fsm_conf_t *conf;
        FOR_EACH_OPTION(opt, opts, pkt->size){
            conf = cp->get_conf_by_code(cp, ppp_opt_get_type(opt));
            if (!conf) {
                /*Received invalid ACK*/
                DEBUG("gnrc_ppp: fsm: Peer sent inconsistent ACK\n");
                return -EBADMSG;
            }
            conf->set(cp, opt, false);
        }
    }
    return E_RCA;
}

int handle_rcn_nak(ppp_fsm_t *cp, ppp_hdr_t *hdr, gnrc_pktsnip_t *pkt)
{
    if (!pkt) {
        /* If the packet doesn't have options, it's considered as invalid. */
        DEBUG("gnrc_ppp: fsm: Received NAK packet without options. Discard\n");
        return -EBADMSG;
    }

    /* Check if options are valid */
    if (ppp_conf_opts_valid(pkt, pkt->size) <= 0) {
        DEBUG("gnrc_ppp: fsm: Received NAK pkt with invalid options. Discard\n");
        return -EBADMSG;
    }


    if (hdr->id != cp->cr_sent_identifier) {
        DEBUG("gnrc_ppp: fsm: ID Mismatch in NAK packet\n");
        return -EBADMSG;
    }

    /*Handle nak for each option*/
    fsm_conf_t *curr_conf;
    network_uint32_t value;
    uint8_t *payload;

    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        curr_conf = cp->get_conf_by_code(cp, ppp_opt_get_type(opt));
        if (curr_conf != NULL) {
            if (!(curr_conf->flags & OPT_ENABLED)) {
                curr_conf->flags |= OPT_ENABLED;
            }
            else if (curr_conf->is_valid(opt)) {
                value = byteorder_htonl(0);
                ppp_opt_get_payload(opt, (void **) &payload);
                memcpy(&value + (4 - curr_conf->size), payload, curr_conf->size);
                curr_conf->value = value;
            }
            else {
                curr_conf->flags &= ~OPT_ENABLED;
            }
        }
    }
    return E_RCN;
}

int handle_rcn_rej(ppp_fsm_t *cp, ppp_hdr_t *hdr, gnrc_pktsnip_t *pkt)
{
    if (!pkt || hdr->id != cp->cr_sent_identifier || ppp_conf_opts_valid(pkt, pkt->size) <= 0 || byteorder_ntohs(hdr->length) - sizeof(ppp_hdr_t) != cp->cr_sent_size) {
        return -EBADMSG;
    }


    uint16_t size = cp->cr_sent_size;

    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        if (!ppp_opt_is_subset(opt, cp->cr_sent_opts, size)) {
            return -EBADMSG;
        }
    }

    /* Disable every REJ option */
    fsm_conf_t *curr_conf;
    FOR_EACH_OPTION(opt, pkt->data, pkt->size){
        curr_conf = cp->get_conf_by_code(cp, ppp_opt_get_type(opt));
        if (curr_conf == NULL) {
            DEBUG("gnrc_ppp: Shouldn't be here\n");
            return -EBADMSG;
        }
        curr_conf->flags &= ~OPT_ENABLED;
    }
    return E_RCN;
}

int handle_coderej(ppp_hdr_t *hdr, gnrc_pktsnip_t *pkt)
{
    ppp_hdr_t *rej_hdr = (ppp_hdr_t *) pkt->data;

    uint8_t code = rej_hdr->code;

    if (code >= PPP_CONF_REQ && code <= PPP_TERM_ACK) {
        return E_RXJm;
    }
    else {
        return E_RXJp;
    }

}


int handle_term_ack(ppp_fsm_t *cp, gnrc_pktsnip_t *pkt)
{
    ppp_hdr_t *ppp_hdr = pkt->data;

    int id = ppp_hdr->id;

    if (id == cp->tr_sent_identifier) {
        return E_RTA;
    }
    return -EBADMSG;
}


static int handle_conf_pkt(ppp_fsm_t *cp, int type, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *ppp_hdr = gnrc_pktbuf_mark(pkt, sizeof(ppp_hdr_t), cp->prottype);
    gnrc_pktsnip_t *payload = pkt->size == 0 ? NULL : pkt;
    ppp_hdr_t *hdr = (ppp_hdr_t *) ppp_hdr->data;

    int event;

    switch (type) {
        case PPP_CONF_REQ:
            event = handle_rcr(cp, payload);
            break;
        case PPP_CONF_ACK:
            event = handle_rca(cp, hdr, payload);
            break;
        case PPP_CONF_NAK:
            event = handle_rcn_nak(cp, hdr, payload);
            break;
        case PPP_CONF_REJ:
            event = handle_rcn_rej(cp, hdr, payload);
            break;
        default:
            DEBUG("Shouldn't be here...\n");
            return -EBADMSG;
            break;
    }
    return event;
}

int fsm_event_from_pkt(ppp_fsm_t *cp, gnrc_pktsnip_t *pkt)
{
    ppp_hdr_t *hdr = (ppp_hdr_t *) pkt->data;

    int code = hdr->code;
    int supported = cp->supported_codes & (1 << (code - 1));
    int type = supported ? code : PPP_UNKNOWN_CODE;

    int event;

    switch (type) {
        case PPP_CONF_REQ:
        case PPP_CONF_ACK:
        case PPP_CONF_NAK:
        case PPP_CONF_REJ:
            event = handle_conf_pkt(cp, type, pkt);
            break;
        case PPP_TERM_REQ:
            event = E_RTR;
            break;
        case PPP_TERM_ACK:
            event = handle_term_ack(cp, pkt);
            break;
        case PPP_CODE_REJ:
            event = handle_coderej(hdr, pkt);
            break;
        case PPP_ECHO_REQ:
        case PPP_ECHO_REP:
        case PPP_DISC_REQ:
            event = E_RXR;
            break;
        default:
            event = E_RUC;
            break;
    }

    return event;
}

int fsm_handle_ppp_msg(struct ppp_protocol_t *protocol, uint8_t ppp_event, void *args)
{
    ppp_fsm_t *target = (ppp_fsm_t *) protocol;
    int event;
    gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) args;

    switch (ppp_event) {
        case PPP_RECV:
            event = fsm_event_from_pkt(target, pkt);
            if (event > 0) {
                trigger_fsm_event(target, event, pkt);
            }
            gnrc_pktbuf_release(pkt);
            return event < 0 ? event : 0;
            break;
        case PPP_LINKUP:
            protocol->state = PROTOCOL_STARTING;
            trigger_fsm_event(target, E_UP, NULL);
            break;
        case PPP_LINKDOWN:
            trigger_fsm_event(target, E_DOWN, NULL);
            break;
        case PPP_UL_STARTED:
            if (target->state == S_OPENED) {
                send_ppp_event(&protocol->msg, ppp_msg_set(_fsm_upper_layer(target), PPP_LINKUP));
            }
            break;
        case PPP_TIMEOUT:
            if (target->restart_counter) {
                trigger_fsm_event(target, E_TOp, NULL);
            }
            else {
                trigger_fsm_event(target, E_TOm, NULL);
            }
            break;
    }
    return 0;
}
