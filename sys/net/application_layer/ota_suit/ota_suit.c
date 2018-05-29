/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
#include "net/ota_suit.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"
#include "firmware/manifest.h"

#define COAP_INBUF_SIZE     (256U)
#define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define PRIO                (THREAD_PRIORITY_MAIN)
#define TNAME               "coap"
/* initialize nanocoap server instance */
static char _stack[STACKSIZE];

ssize_t _manifest_post_handler(coap_pkt_t* pkt, uint8_t *buf, size_t len)
{
    int result = COAP_CODE_CREATED;
    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);
    bool more = (!blockwise || !block1.more) ? false : true;

    if (firmware_manifest_putbytes(pkt->payload, pkt->payload_len, block1.offset, more) < 0) {
        return COAP_CODE_SERVICE_UNAVAILABLE;
    }
    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    uint8_t *pkt_pos = (uint8_t*)pkt->hdr + reply_len;
    pkt_pos += coap_put_block1_ok(pkt_pos, &block1, 0);
    return pkt_pos - (uint8_t*)pkt->hdr;
}

ssize_t _manifest_handler(coap_pkt_t* pkt, uint8_t *buf, size_t len,
        void *context)
{
    (void)context;
    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));
    switch(method_flag) {
        case COAP_POST:
            return _manifest_post_handler(pkt, buf, len);
    }
    return coap_reply_simple(pkt, COAP_CODE_METHOD_NOT_ALLOWED,
            buf, len , COAP_FORMAT_NONE, NULL, 0);
}

ssize_t _slot_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len,
        void *context)
{
    uint32_t slot = (uint32_t)context;
    (void)slot;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
}

static void *_ota_suit_thread(void* arg)
{
    (void)arg;
    msg_t msg_queue[4];
    msg_init_queue(msg_queue, 4);
    /* initialize nanocoap server instance */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));
    return NULL;
}

void ota_suit_run(void)
{
    thread_create(_stack, STACKSIZE, PRIO, THREAD_CREATE_STACKTEST,
                        _ota_suit_thread, NULL, TNAME);
}

/* must be sorted by path (alphabetically) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/sys/firmware", COAP_POST, _manifest_handler, NULL },
    { "/sys/firmware/0", COAP_GET, _slot_handler, (void *)0 },
    { "/sys/firmware/1", COAP_GET, _slot_handler, (void *)1 },
};

const unsigned coap_resources_numof = sizeof(coap_resources) / sizeof(coap_resources[0]);
