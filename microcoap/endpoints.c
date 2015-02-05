/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       microcoap example server endpoints
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * @}
 */

#include <stdbool.h>
#include <stdbool.h>
#include <string.h>
#include "coap.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define MAX_RESPONSE_LEN 1500
static uint8_t response[MAX_RESPONSE_LEN] = "";

static const coap_endpoint_path_t path = {2, {"foo", "bar"}};

void create_response_payload(const uint8_t *buffer)
{
    char *response = "1337";
    memcpy((void*)buffer, response, strlen(response));
}

/* The handler which handles the path /foo/bar */
static int handle_get_response(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    DEBUG("[endpoints]  %s()\n",  __func__);
    create_response_payload(response);
    /* NOTE: COAP_RSPCODE_CONTENT only works in a packet answering a GET. */
    return coap_make_response(scratch, outpkt, response, strlen((char*)response),
                              id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
}

const coap_endpoint_t endpoints[] =
{
    {COAP_METHOD_GET, handle_get_response, &path, "ct=0"},
    {(coap_method_t)0, NULL, NULL, NULL} /* marks the end of the endpoints array */
};
