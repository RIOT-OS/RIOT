/*
 * Copyright (C) 2017-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_epsim
 * @{
 *
 * @file
 * @brief       CoRE RD simple registration endpoint implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "net/gcoap.h"
#include "net/cord/epsim.h"
#include "net/cord/config.h"
#include "net/cord/common.h"
#include "net/ipv6/addr.h"

#define BUFSIZE             (128U)

/* we don't want to allocate the CoAP packet and scratch buffer on the stack,
 * as they are too large for that. */
static coap_pkt_t pkt;
static uint8_t buf[BUFSIZE];

/* keep state of the latest registration attempt */
static int _state = CORD_EPSIM_ERROR;

static void _req_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu,
                         const sock_udp_ep_t *remote)
{
    (void)remote;
    (void)pdu;
    _state = (memo->state == GCOAP_MEMO_RESP) ? CORD_EPSIM_OK : CORD_EPSIM_ERROR;
}

int cord_epsim_register(const sock_udp_ep_t *rd_ep)
{
    assert(rd_ep);

    if (_state == CORD_EPSIM_BUSY) {
        return CORD_EPSIM_BUSY;
    }

    /* build the initial CON packet */
    if (gcoap_req_init(&pkt, buf, sizeof(buf), COAP_METHOD_POST,
                             "/.well-known/rd") < 0) {
        return CORD_EPSIM_ERROR;
    }
    /* make packet confirmable */
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    /* add Uri-Query options */
    if (cord_common_add_qstring(&pkt) < 0) {
        return CORD_EPSIM_ERROR;
    }
    /* finish, we don't have any payload */
    ssize_t len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    _state = CORD_EPSIM_BUSY;
    if (gcoap_req_send(buf, len, rd_ep, _req_handler, NULL, GCOAP_SOCKET_TYPE_UNDEF) == 0) {
        return CORD_EPSIM_ERROR;
    }

    return CORD_EPSIM_OK;
}

int cord_epsim_state(void)
{
    return _state;
}
