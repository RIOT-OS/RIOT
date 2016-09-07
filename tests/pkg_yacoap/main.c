/*
 * Copyright (C) 2016 smlng
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief YaCoAP test application
 *
 * @author smlng <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coap.h"

const uint16_t wkclen = 128;
static char wkc[128] = "";

static const coap_resource_path_t path_well_known_core = {2, {".well-known", "core"}};
static int handle_get_well_known_core(const coap_resource_t *resource,
                                      const coap_packet_t *inpkt,
                                      coap_packet_t *pkt)
{
    printf(".. handle_get_well_known_core\n");
    return coap_make_response(inpkt->hdr.id, &inpkt->tok,
                              COAP_TYPE_ACK, COAP_RSPCODE_CONTENT,
                              resource->content_type,
                              (const uint8_t *)wkc, strlen(wkc),
                              pkt);
}

coap_resource_t resources[] =
{
    {COAP_RDY, COAP_METHOD_GET, COAP_TYPE_ACK,
        handle_get_well_known_core, &path_well_known_core,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_APP_LINKFORMAT)},
    {(coap_state_t)0, (coap_method_t)0, (coap_msgtype_t)0,
        NULL, NULL,
        COAP_SET_CONTENTTYPE(COAP_CONTENTTYPE_NONE)}
};

int main(void) {
    coap_state_t rc;
    printf(". make link format for ./well-known/core\n");
    if ((rc = coap_make_link_format(resources, wkc, wkclen)) > COAP_ERR) {
        printf("!! coap_make_link_format failed rc=%d !!\n", rc);
        return 1;
    }
    printf(".. %s\n", wkc);
    coap_packet_t req;
    uint16_t msgid = 42;
    printf(". make get request for ./well-known/core\n");
    if ((rc = coap_make_request(msgid, NULL, &resources[0], NULL, 0, &req)) > COAP_ERR) {
        printf("!! coap_make_request failed rc=%d\n !!", rc);
        return 1;
    }
    uint8_t buf[255];
    size_t buflen = sizeof(buf);
    printf(". build send buffer from packet\n");
    if ((rc = coap_build(&req, buf, &buflen)) > COAP_ERR) {
        printf("!! coap_build failed rc=%d\n !!", rc);
        return 1;
    }
    coap_packet_t tmp;
    printf(". parse recv buffer to packet\n");
    if ((rc = coap_parse(buf, buflen, &tmp)) > COAP_ERR) {
        printf("!! coap_parse failed, bad packet rc=%d !!\n", rc);
        return 1;
    }
    printf(". handle request and make response packet\n");
    for (coap_state_t state = COAP_RSP_WAIT; state != COAP_RSP_SEND; ) {
        coap_packet_t rsp;
        if ((state = coap_handle_request(resources, &tmp, &rsp)) > COAP_ERR) {
            printf("!! coap_handle_request failed, rc=%d !!\n", state);
            return 1;
        }
    }
    printf("SUCCESS\n");
    return 0;
}
