/*
 * Copyright (C) 2014 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>

#include "coap_handlers.h"
#include "pdu.h"
#include "str.h"

#define INDEX "I'm a test server made with libcoap!"
#define EMPTY "resource is empty"
#define SEPARATE "I got separated."

#define LARGE "CoAP is a RESTful transfer protocol for constrained nodes and\n" \
              "networks.  Basic CoAP messages work well for the small payloads we\n" \
              "expect from temperature sensors, light switches, and similar\n" \
              "building-automation devices.  Occasionally, however, applications\n" \
              "will need to transfer larger payloads -- for instance, for firmware\n" \
              "updates.  With HTTP, TCP does the grunt work of slicing large\n" \
              "payloads up into multiple packets and ensuring that they all arrive\n" \
              "and are handled in the right order.\n" \
              "CoAP is based on datagram transports such as UDP or DTLS, which\n" \
              "limits the maximum size of resource representations that can be\n" \
              "transferred without too much fragmentation.  Although UDP supports\n" \
              "larger payloads through IP fragmentation, it is limited to 64 KiB\n" \
              "and, more importantly, doesn't really work well for constrained\n" \
              "applications and networks.\n" \
              "Instead of relying on IP fragmentation, this specification extends\n" \
              "basic CoAP with a pair of \"Block\" options, for transferring multiple\n" \
              "blocks of information from a resource representation in multiple\n" \
              "request-response pairs.  In many important cases, the Block options\n" \
              "enable a server to be truly stateless: the server can handle each\n" \
              "block transfer separately, with no need for a connection setup or\n" \
              "other server-side memory of previous block transfers.\n" \
              "In summary, the Block options provide a minimal way to transfer\n" \
              "larger representations in a block-wise fashion."


#define ACCEPT_PLAIN "This is plain text."
#define ACCEPT_XML "<?xml version=\"1.0\" encoding=\"UTF-8\"?><text>This is XML.</text>"

#define NO_ETAG "No ETag-Option provided.\n"

static str *local_data = NULL;
static coap_key_t local_key;

static inline void set_and_hash(size_t len, unsigned char *data)
{
    if (local_data) {
        coap_delete_string(local_data);
    }

    local_data = coap_new_string(len);
    local_data->length = len;
    memcpy(local_data->s, data, len);

    coap_str_hash(local_data, local_key);

    printf("local_data->length: %i\n", local_data->length);
    printf("local_data->s: '%.*s'\n", local_data->length, local_data->s);
}

void init_local_data(void)
{
    set_and_hash(strlen(INDEX), (unsigned char *)INDEX);
}

void index_handler(coap_context_t  *ctx, struct coap_resource_t *resource,
                   const coap_endpoint_t *local_interface,
                   coap_address_t *peer, coap_pdu_t *request, str *token,
                   coap_pdu_t *response)
{
    /* Many simple handlers don't need access to the following
       parameters. The only one always needed is response in order to
       set a response code. */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) request;
    (void) resource;
    (void) token;

    unsigned char buf[3];

    response->hdr->code = COAP_RESPONSE_CODE(205);

    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

    coap_add_option(response, COAP_OPTION_MAXAGE,
                    coap_encode_var_bytes(buf, 0x2ffff), buf);

    coap_add_data(response, strlen(INDEX), (unsigned char *)INDEX);
}

/* Identifier:	TD_COAP_CORE_01 */
/* Objective:	Perform GET transaction (CON mode) */
/* Pre-test conditions: Server offers the resource /test with resource
   content is not empty that handles GET with an arbitrary payload */
void td_coap_core_01(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) request;
    (void) resource;
    (void) token;

    response->hdr->code = COAP_RESPONSE_CODE(205); /* 2.05 Content */

    /* Content-format option */
    unsigned char buf[3];
    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

    if (local_data && (local_data->length > 0)) {
        coap_add_data(response, local_data->length, local_data->s);
    }
    else {
        coap_add_data(response, strlen(EMPTY), (unsigned char *)EMPTY);
    }
}

/* Identifier:	TD_COAP_CORE_02 */
/* Objective:	Perform DELETE transaction (CON mode) */
/* Pre-test conditions:	Server offers a /test resource that handles DELETE */
void td_coap_core_02(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) request;
    (void) resource;
    (void) token;

    /* The DELETE method requests that the resource identified by the */
    /* request URI be deleted.  A 2.02 (Deleted) response code SHOULD be */
    /* used on success or in case the resource did not exist before the */
    /* request. */
    /* DELETE is not safe, but is idempotent. */

    if (local_data) {
        coap_delete_string(local_data);
        local_data = NULL;
    }

    response->hdr->code = COAP_RESPONSE_CODE(202);
}

/* Identifier:	TD_COAP_CORE_03 */
/* Objective:	Perform PUT transaction (CON mode) */
/* Pre-test conditions: Server offers already available resource /test
   or accepts creation of new resource on /test that handles PUT */
void td_coap_core_03(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) resource;
    (void) token;

    size_t len;
    unsigned char *data;

    coap_opt_iterator_t opt_iter;
    coap_opt_t *match = coap_check_option(request, COAP_OPTION_IF_MATCH, &opt_iter);

    if (match) {
        if (!(coap_opt_length(match) == sizeof(coap_key_t)) ||
            (memcmp(coap_opt_value(match), &local_key, sizeof(coap_key_t)))) {
            response->hdr->code = COAP_RESPONSE_CODE(412);
            return;
        }
    }

    if (coap_get_data(request, &len, &data)) {
        if (local_data) {
            response->hdr->code = COAP_RESPONSE_CODE(204);
        }
        else {
            response->hdr->code = COAP_RESPONSE_CODE(201);
        }

        set_and_hash(len, data);
    }
    else {
        response->hdr->code = COAP_RESPONSE_CODE(500);
    }
}

/* Identifier:	TD_COAP_CORE_04 */
/* Objective:	Perform POST transaction (CON mode) */
/* Pre-test conditions:	Server accepts POST request on /test */
void td_coap_core_04(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) request;
    (void) resource;
    (void) token;

    /* Server sends response containing: */
    /* Code = 2.01 (Created) or 2.04 (Changed) */
    response->hdr->code = COAP_RESPONSE_CODE(201);

    /* Zero or more Location-path options */
    coap_add_option(response, COAP_OPTION_LOCATION_PATH, 9, (unsigned char *)"location1");
    coap_add_option(response, COAP_OPTION_LOCATION_PATH, 9, (unsigned char *)"location2");
    coap_add_option(response, COAP_OPTION_LOCATION_PATH, 9, (unsigned char *)"location3");

    /* Content-format option if payload non-empty */
    unsigned char buf[3];
    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
}

/* Tests TD_COAP_CORE_05 to TD_COAP_CORE_08 (NON mode tests) get
   handled by the same handlers as above. */

/* Identifier:	TD_COAP_CORE_09 */
/* Objective:	Perform GET transaction with separate response (CON mode, no piggyback) */
void td_coap_core_09(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) resource;
    (void) token;

    /* Server sends response containing: */
    /* Type = 2 (ACK) */
    /* Code = 0 */
    /* Message ID = CMID */
    /* Empty Payload */
    if (request->hdr->type == COAP_MESSAGE_CON) {
        coap_tid_t ack = coap_send_ack(ctx, local_interface, peer, request);

        if (ack == COAP_INVALID_TID) {
            printf("Error sending ACK to peer\n");
        }
        else {
            printf("Sent ACK to peer.\n");
        }
    }

    /* Some time (a couple of seconds) elapses.  */
    printf("Go and sleep\n");
    vtimer_usleep(2 * 1000000);

    /* Server sends response containing: */
    /* Type = 0 (CON) */
    response->hdr->type = request->hdr->type;
    /* Code = 2.05 (Content) */
    response->hdr->code = COAP_RESPONSE_CODE(205);
    /* Server-generated Message ID (➔ SMID) */
    /* ?? */
    /* Token = CTOK */

    /* Content-format option */
    unsigned char buf[3];
    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

    /* Non-empty Payload */
    coap_add_data(response, strlen(SEPARATE), (unsigned char *)SEPARATE);
}

/* Identifier:	TD_COAP_CORE_19 */
/* Objective:	Perform POST transaction with responses containing several Location-Query options (CON mode) */
void td_coap_core_19(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) request;
    (void) resource;
    (void) token;

    response->hdr->code = COAP_RESPONSE_CODE(201);

    coap_add_option(response, COAP_OPTION_LOCATION_QUERY, 7, (unsigned char *)"first=1");
    coap_add_option(response, COAP_OPTION_LOCATION_QUERY, 8, (unsigned char *)"second=2");
}

/* Identifier:	TD_COAP_CORE_20 */
/* Objective:	Perform GET transaction containing the Accept option (CON mode) */
void td_coap_core_20(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) resource;
    (void) token;

    response->hdr->code = COAP_RESPONSE_CODE(205);

    unsigned char buf[3];
    coap_opt_iterator_t opt_iter;
    coap_opt_t *accept = coap_check_option(request, COAP_OPTION_ACCEPT, &opt_iter);

    /* In this one specific case we serve XML */
    if (accept && (coap_opt_length(accept) == 1
                   && (unsigned char)*coap_opt_value(accept) == COAP_MEDIATYPE_APPLICATION_XML)) {
        coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                        coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_XML), buf);
        coap_add_data(response, strlen(ACCEPT_XML), (unsigned char *)ACCEPT_XML);
    }
    /* in all other cases we ship text/plain */
    else {
        coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                        coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
        coap_add_data(response, strlen(ACCEPT_PLAIN), (unsigned char *)ACCEPT_PLAIN);
    }
}

/* Identifier:	TD_COAP_CORE_21 */
/* Objective:	Perform GET transaction containing the ETag option (CON mode) */
/* Pre-test conditions:	 */
/* Server should offer a /validate resource which may be made to vary over time */
/* Client & server supports ETag option */
/* The Client’s cache must be purged */


void td_coap_core_21(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) resource;
    (void) token;

    coap_add_option(response, COAP_OPTION_ETAG, sizeof(coap_key_t), (const unsigned char *)&local_key);
    coap_opt_iterator_t opt_iter;
    coap_opt_t *opt_etag = coap_check_option(request, COAP_OPTION_ETAG, &opt_iter);

    if (opt_etag && (coap_opt_length(opt_etag) == sizeof(coap_key_t)) &&
        (!memcmp(coap_opt_value(opt_etag), &local_key, sizeof(coap_key_t)))) {
        response->hdr->code = COAP_RESPONSE_CODE(203);
    }
    else {
        unsigned char buf[3];
        coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                        coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
        coap_add_data(response, local_data->length, local_data->s);
        response->hdr->code = COAP_RESPONSE_CODE(205);
    }
}

/* Identifier:	TD_COAP_CORE_23 */
/* Objective:	Perform PUT transaction containing the If-None-Match option (CON mode) */
void td_coap_core_23(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) resource;
    (void) token;

    static int create1;
    coap_opt_iterator_t opt_iter;
    coap_opt_t *none_match = coap_check_option(request, COAP_OPTION_IF_NONE_MATCH, &opt_iter);

    if (create1 && none_match) {
        response->hdr->code = COAP_RESPONSE_CODE(412);
        create1 = 0;
    }
    else {
        response->hdr->code = COAP_RESPONSE_CODE(201);
        create1 = 1;
    }
}

void td_coap_link_09(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) request;
    (void) resource;
    (void) token;

    response->hdr->code = COAP_RESPONSE_CODE(205);
    unsigned char buf[3];

    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(buf, COAP_MEDIATYPE_APPLICATION_LINK_FORMAT), buf);
    const char *link = "</path/sub1>";
    coap_add_data(response, strlen(link), (unsigned char *)link);
}


void td_coap_block_01(coap_context_t  *ctx, struct coap_resource_t *resource,
                      const coap_endpoint_t *local_interface,
                      coap_address_t *peer, coap_pdu_t *request, str *token,
                      coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) resource;
    (void) token;

    response->hdr->code = COAP_RESPONSE_CODE(205);
    unsigned char buf[3];
    coap_block_t block;
    int ret;

    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
                    coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

    if (coap_get_block(request, COAP_OPTION_BLOCK2, &block)) {
        ret = coap_write_block_opt(&block, COAP_OPTION_BLOCK2, response, strlen(LARGE));

        coap_add_block(response, strlen(LARGE), (unsigned char *)LARGE, block.num, block.szx);
    }
    else {
        /* 2**(4+2) = 64 bytes block-size avoids 6LoWPan fragmentation */
        block.szx = 2;
        ret = coap_write_block_opt(&block, COAP_OPTION_BLOCK2, response, strlen(LARGE));
        // TODO: fix return value
        printf("ret: %i\n", ret);
        coap_add_block(response, strlen(LARGE), (unsigned char *)LARGE, 0, block.szx);
    }
}

void threads_handler(coap_context_t  *ctx, struct coap_resource_t *resource,
                     const coap_endpoint_t *local_interface,
                     coap_address_t *peer, coap_pdu_t *request, str *token,
                     coap_pdu_t *response)
{
    /* see index_handler */
    (void) ctx;
    (void) local_interface;
    (void) peer;
    (void) request;
    (void) resource;
    (void) token;

    unsigned char buf[(MAXTHREADS + 1) * 8] = "[";
    unsigned int length = 1;

    response->hdr->code = COAP_RESPONSE_CODE(205);

    for (int i = 0; i < MAXTHREADS; i++) {
        tcb_t *p = (tcb_t *)sched_threads[i];

        if (p != NULL) {
            length  += sprintf((char *)(buf + length), "%i,", p->pid);
        }
    }

    memcpy((buf + length - 1), "]", 1);
    coap_add_data(response, length, buf);
}
