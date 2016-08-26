/* CoAP server for first ETSI CoAP plugtest, March 2012
 *
 * Copyright (C) 2012--2013 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use. 
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>

#include "config.h"
#include "uthash.h"
#include "coap.h"
#include "etsi_iot_01_largedata.h"

#define COAP_RESOURCE_CHECK_TIME_SEC  1

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

pthread_mutex_t mtx;

#define COAP_OPT_BLOCK_SZX_MAX 6 /**< allowed maximum for block szx value */

#define REQUIRE_ETAG 0x01 	/* flag for coap_payload_t: require ETag option  */
typedef struct {
    UT_hash_handle hh;
    coap_key_t resource_key; /* foreign key that points into resource space */
    unsigned int flags; /* some flags to control behavior */
    size_t max_data; /* maximum size allocated for @p data */
    uint16_t media_type; /* media type for this object */
    size_t length; /* length of data */
    unsigned char data[]; /* the actual contents */
} coap_payload_t;

coap_payload_t *test_resources = NULL;

/** 
 * This structure is used to store URIs for dynamically allocated
 * resources, usually by POST or PUT.
 */
typedef struct {
    UT_hash_handle hh;
    coap_key_t resource_key; /* foreign key that points into resource space */
    size_t length; /* length of data */
    unsigned char data[]; /* the actual contents */
} coap_dynamic_uri_t;

coap_dynamic_uri_t *test_dynamic_uris = NULL;

/* This variable is used to mimic long-running tasks that require
 * asynchronous responses. */
static coap_async_state_t *async = NULL;

#define INDEX "libcoap server for ETSI CoAP Plugtest, March 2012, Paris\n" \
   	      "Copyright (C) 2012 Olaf Bergmann <bergmann@tzi.org>\n\n"

coap_payload_t *
coap_new_payload(size_t size)
{
    coap_payload_t *p;
    p = (coap_payload_t *) coap_malloc(sizeof(coap_payload_t) + size);
    if (p) {
        memset(p, 0, sizeof(coap_payload_t));
        p->max_data = size;
    }

    return p;
}

static inline coap_payload_t *
coap_find_payload(const coap_key_t key)
{
    coap_payload_t *p;
    HASH_FIND(hh, test_resources, key, sizeof(coap_key_t), p);
    return p;
}

static inline void coap_add_payload(const coap_key_t key,
        coap_payload_t *payload, coap_dynamic_uri_t *uri)
{
    assert(payload);

    memcpy(payload->resource_key, key, sizeof(coap_key_t));
    HASH_ADD(hh, test_resources, resource_key, sizeof(coap_key_t), payload);

    if (uri) {
        memcpy(uri->resource_key, key, sizeof(coap_key_t));
        HASH_ADD(hh, test_dynamic_uris, resource_key, sizeof(coap_key_t), uri);
    }
}

static inline void coap_delete_payload(coap_payload_t *payload)
{
    if (payload) {
        coap_dynamic_uri_t *uri;
        HASH_FIND(hh, test_dynamic_uris, payload->resource_key,
                sizeof(coap_key_t), uri);
        if (uri) {
            HASH_DELETE(hh, test_dynamic_uris, uri);
            coap_free(uri);
        }
    }

    HASH_DELETE(hh, test_resources, payload);
    coap_free(payload);
}

void hnd_get_index(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    unsigned char buf[3];

    response->hdr->code = COAP_RESPONSE_CODE(205);

    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
            coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

    coap_add_option(response, COAP_OPTION_MAXAGE,
            coap_encode_var_bytes(buf, 0x2ffff), buf);

    coap_add_data(response, strlen(INDEX), (unsigned char *) INDEX);
}

void hnd_get_resource(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    coap_key_t etag;
    unsigned char buf[2];
    coap_payload_t *test_payload;
    coap_block_t block;

    test_payload = coap_find_payload(resource->key);
    if (!test_payload) {
        response->hdr->code = COAP_RESPONSE_CODE(500);

        return;
    }

    response->hdr->code = COAP_RESPONSE_CODE(205);

    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
            coap_encode_var_bytes(buf, test_payload->media_type), buf);

    /* add etag for the resource */
    if (test_payload->flags & REQUIRE_ETAG) {
        memset(etag, 0, sizeof(etag));
        coap_hash(test_payload->data, test_payload->length, etag);
        coap_add_option(response, COAP_OPTION_ETAG, sizeof(etag), etag);
    }

    if (request) {
        int res;

        if (coap_get_block(request, COAP_OPTION_BLOCK2, &block)) {
            res = coap_write_block_opt(&block, COAP_OPTION_BLOCK2, response,
                    test_payload->length);

            switch (res) {
            case -2: /* illegal block */
                response->hdr->code = COAP_RESPONSE_CODE(400);
                goto error;
            case -1: /* should really not happen */
                assert(0);
                /* fall through if assert is a no-op */
            case -3: /* cannot handle request */
                response->hdr->code = COAP_RESPONSE_CODE(500);
                goto error;
            default: /* everything is good */
                ;
            }

            coap_add_block(response, test_payload->length, test_payload->data,
                    block.num, block.szx);
        } else {
            if (!coap_add_data(response, test_payload->length,
                    test_payload->data)) {
                /* set initial block size, will be lowered by
                 * coap_write_block_opt) automatically */
                block.szx = 6;
                coap_write_block_opt(&block, COAP_OPTION_BLOCK2, response,
                        test_payload->length);

                coap_add_block(response, test_payload->length,
                        test_payload->data, block.num, block.szx);
            }
        }
    } else { /* this is a notification, block is 0 */
        /* FIXME: need to store block size with subscription */
    }

    return;

    error: coap_add_data(response,
            strlen(coap_response_phrase(response->hdr->code)),
            (unsigned char *) coap_response_phrase(response->hdr->code));
}

/* DELETE handler for dynamic resources created by POST /test */
void hnd_delete_resource(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    coap_payload_t *payload;

    payload = coap_find_payload(resource->key);

    if (payload)
        coap_delete_payload(payload);

    coap_delete_resource(ctx, resource->key);

    response->hdr->code = COAP_RESPONSE_CODE(202);
}

void hnd_post_test(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option;
    coap_payload_t *test_payload;
    size_t len;
    size_t l = 6 + sizeof(void *);
    coap_dynamic_uri_t *uri;
    unsigned char *data;

#define BUFSIZE 20
    int res;
    unsigned char _buf[BUFSIZE];
    unsigned char *buf = _buf;
    size_t buflen = BUFSIZE;

    coap_get_data(request, &len, &data);

    /* allocate storage for resource and to hold URI */
    test_payload = coap_new_payload(len);
    uri = (coap_dynamic_uri_t *) coap_malloc(sizeof(coap_dynamic_uri_t) + l);
    if (!(test_payload && uri)) {
        coap_log(LOG_CRIT, "cannot allocate new resource under /test");
        response->hdr->code = COAP_RESPONSE_CODE(500);
        coap_free(test_payload);
        coap_free(uri);
    } else {
        coap_resource_t *r;

        memset(uri, 0, sizeof(coap_dynamic_uri_t));
        uri->length = min(l,
                snprintf((char * )uri->data, l, "test/%p", test_payload));
        test_payload->length = len;

        memcpy(test_payload->data, data, len);

        r = coap_resource_init(uri->data, uri->length, 0);
        coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);
        coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_resource);

        /* set media_type if available */
        option = coap_check_option(request, COAP_OPTION_CONTENT_TYPE,
                &opt_iter);
        if (option) {
            test_payload->media_type = coap_decode_var_bytes(
                    COAP_OPT_VALUE(option), COAP_OPT_LENGTH(option));
        }

        coap_add_resource(ctx, r);
        coap_add_payload(r->key, test_payload, uri);

        /* add Location-Path */
        res = coap_split_path(uri->data, uri->length, buf, &buflen);

        while (res--) {
            coap_add_option(response, COAP_OPTION_LOCATION_PATH,
                    COAP_OPT_LENGTH(buf), COAP_OPT_VALUE(buf));

            buf += COAP_OPT_SIZE(buf);
        }

        response->hdr->code = COAP_RESPONSE_CODE(201);
    }

}

void hnd_put_test(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option;
    coap_payload_t *payload;
    size_t len;
    unsigned char *data;

    response->hdr->code = COAP_RESPONSE_CODE(204);

    coap_get_data(request, &len, &data);

    payload = coap_find_payload(resource->key);
    if (payload && payload->max_data < len) { /* need more storage */
        coap_delete_payload(payload);
        payload = NULL;
        /* bug: when subsequent coap_new_payload() fails, our old contents
         is gone */
    }

    if (!payload) { /* create new payload */
        payload = coap_new_payload(len);
        if (!payload)
            goto error;

        coap_add_payload(resource->key, payload, NULL);
    }
    payload->length = len;
    memcpy(payload->data, data, len);

    option = coap_check_option(request, COAP_OPTION_CONTENT_TYPE, &opt_iter);
    if (option) {
        /* set media type given in request */
        payload->media_type = coap_decode_var_bytes(COAP_OPT_VALUE(option),
                COAP_OPT_LENGTH(option));
    } else {
        /* set default value */
        payload->media_type = COAP_MEDIATYPE_TEXT_PLAIN;
    }
    /* FIXME: need to change attribute ct of resource.
     To do so, we need dynamic management of the attribute value
     */

    return;
    error:
    warn("cannot modify resource\n");
    response->hdr->code = COAP_RESPONSE_CODE(500);
}

void hnd_delete_test(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    /* the ETSI validation tool does not like empty resources... */
#if 0
    coap_payload_t *payload;
    payload = coap_find_payload(resource->key);

    if (payload)
    payload->length = 0;
#endif

    response->hdr->code = COAP_RESPONSE_CODE(202);
}

void hnd_get_query(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    coap_opt_iterator_t opt_iter;
    coap_opt_filter_t f;
    coap_opt_t *q;
    size_t len, L;
    unsigned char buf[70];

    response->hdr->code = COAP_RESPONSE_CODE(205);

    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
            coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

    coap_option_filter_clear(f);
    coap_option_setb(f, COAP_OPTION_URI_QUERY);

    coap_option_iterator_init(request, &opt_iter, f);

    len = 0;
    while ((len < sizeof(buf)) && (q = coap_option_next(&opt_iter))) {
        L = min(sizeof(buf) - len, 11);
        memcpy(buf + len, "Uri-Query: ", L);
        len += L;

        L = min(sizeof(buf) - len, COAP_OPT_LENGTH(q));
        memcpy(buf + len, COAP_OPT_VALUE(q), L);
        len += L;

        if (len < sizeof(buf))
            buf[len++] = '\n';
    }

    coap_add_data(response, len, buf);
}

/* handler for TD_COAP_CORE_16 */
void hnd_get_separate(coap_context_t *ctx, struct coap_resource_t *resource,
        coap_address_t *peer, coap_pdu_t *request, str *token,
        coap_pdu_t *response)
{
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option;
    coap_opt_filter_t f;
    unsigned long delay = 5;

    if (async) {
        if (async->id != request->hdr->id) {
            coap_opt_filter_t f;
            coap_option_filter_clear(f);
            response->hdr->code = COAP_RESPONSE_CODE(503);
        }
        return;
    }

    /* search for option delay in query list */
    coap_option_filter_clear(f);
    coap_option_setb(f, COAP_OPTION_URI_QUERY);

    coap_option_iterator_init(request, &opt_iter, f);

    while ((option = coap_option_next(&opt_iter))) {
        if (strncmp("delay=", (char *) COAP_OPT_VALUE(option), 6) == 0) {
            int i;
            unsigned long d = 0;

            for (i = 6; i < COAP_OPT_LENGTH(option); ++i)
                d = d * 10 + COAP_OPT_VALUE(option)[i] - '0';

            /* don't allow delay to be less than COAP_RESOURCE_CHECK_TIME*/
            delay = d < COAP_RESOURCE_CHECK_TIME_SEC ?
                    COAP_RESOURCE_CHECK_TIME_SEC : d;
            debug("set delay to %lu\n", delay);
            break;
        }
    }

    async = coap_register_async(ctx, peer, request, COAP_ASYNC_SEPARATE,
            (void *) (COAP_TICKS_PER_SECOND * delay));
}

void check_async(coap_context_t *ctx, coap_tick_t now)
{
    coap_pdu_t *response;
    coap_async_state_t *tmp;
    unsigned char buf[2];
    size_t size = sizeof(coap_hdr_t) + 8;

    if (!async || now < async->created + (unsigned long) async->appdata)
        return;

    size += async->tokenlen;

    response = coap_pdu_init(
            async->flags & COAP_ASYNC_CONFIRM ?
                    COAP_MESSAGE_CON : COAP_MESSAGE_NON,
            COAP_RESPONSE_CODE(205), 0, size);
    if (!response) {
        debug("check_async: insufficient memory, we'll try later\n");
        async->appdata = (void *) ((unsigned long) async->appdata
                + 15 * COAP_TICKS_PER_SECOND);
        return;
    }

    response->hdr->id = coap_new_message_id(ctx);

    if (async->tokenlen)
        coap_add_token(response, async->tokenlen, async->token);

    coap_add_option(response, COAP_OPTION_CONTENT_TYPE,
            coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);

    coap_add_data(response, 4, (unsigned char *) "done");

    if (coap_send(ctx, &async->peer, response) == COAP_INVALID_TID) {
        debug("check_async: cannot send response for message %d\n",
                response->hdr->id);
    }
    coap_delete_pdu(response);

    coap_remove_async(ctx, async->id, &tmp);
    coap_free_async(async);
    async = NULL;
}

coap_payload_t *
make_large(char *large_string)
{
    coap_payload_t *payload;

    payload = coap_new_payload(strlen(large_string));
    if (!payload)
        return NULL;

    memcpy(payload->data, payload, strlen(large_string));
    payload->length = strlen(large_string);
    payload->media_type = 41;

    return payload;
}

void init_resources(coap_context_t *ctx)
{
    coap_resource_t *r;
    coap_payload_t *test_payload;

    test_payload = coap_new_payload(200);
    if (!test_payload)
        coap_log(LOG_CRIT, "cannot allocate resource /test");
    else {
        test_payload->length = 13;
        memcpy(test_payload->data, "put data here", test_payload->length);
        /* test_payload->media_type is 0 anyway */

        r = coap_resource_init((unsigned char *) "test", 4, 0);
        coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);
        coap_register_handler(r, COAP_REQUEST_POST, hnd_post_test);
        coap_register_handler(r, COAP_REQUEST_PUT, hnd_put_test);
        coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete_test);

        coap_add_attr(r, (unsigned char *) "ct", 2, (unsigned char *) "0", 1,
                0);
        coap_add_attr(r, (unsigned char *) "rt", 2, (unsigned char *) "test", 4,
                0);
        coap_add_attr(r, (unsigned char *) "if", 2, (unsigned char *) "core#b",
                6, 0);
#if 0
        coap_add_attr(r, (unsigned char *)"obs", 3, NULL, 0, 0);
#endif
        coap_add_resource(ctx, r);
        coap_add_payload(r->key, test_payload, NULL);
    }

    /* TD_COAP_BLOCK_01
     * TD_COAP_BLOCK_02 */
    test_payload = make_large(large_string);
    if (!test_payload)
        coap_log(LOG_CRIT, "cannot allocate resource /large\n");
    else {
        r = coap_resource_init((unsigned char *) "large", 5, 0);
        coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);

        coap_add_attr(r, (unsigned char *) "ct", 2, (unsigned char *) "41", 2,
                0);
        coap_add_attr(r, (unsigned char *) "rt", 2, (unsigned char *) "large",
                5, 0);
        coap_add_resource(ctx, r);

        test_payload->flags |= REQUIRE_ETAG;

        coap_add_payload(r->key, test_payload, NULL);
    }

    /* For TD_COAP_CORE_12 */
    test_payload = coap_new_payload(20);
    if (!test_payload)
        coap_log(LOG_CRIT, "cannot allocate resource /seg1/seg2/seg3\n");
    else {
        test_payload->length = 10;
        memcpy(test_payload->data, "segsegseg!", test_payload->length);
        /* test_payload->media_type is 0 anyway */

        r = coap_resource_init((unsigned char *) "seg1/seg2/seg3", 14, 0);
        coap_register_handler(r, COAP_REQUEST_GET, hnd_get_resource);

        coap_add_attr(r, (unsigned char *) "ct", 2, (unsigned char *) "0", 1,
                0);
        coap_add_resource(ctx, r);

        coap_add_payload(r->key, test_payload, NULL);
    }

    /* For TD_COAP_CORE_13 */
    r = coap_resource_init((unsigned char *) "query", 5, 0);
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_query);

    coap_add_attr(r, (unsigned char *) "ct", 2, (unsigned char *) "0", 1, 0);
    coap_add_resource(ctx, r);

    /* For TD_COAP_CORE_16 */
    r = coap_resource_init((unsigned char *) "separate", 8, 0);
    coap_register_handler(r, COAP_REQUEST_GET, hnd_get_separate);

    coap_add_attr(r, (unsigned char *) "ct", 2, (unsigned char *) "0", 1, 0);
    coap_add_attr(r, (unsigned char *) "rt", 2, (unsigned char *) "separate", 8,
            0);
    coap_add_resource(ctx, r);
}

void usage(const char *program, const char *version)
{
    const char *p;

    p = strrchr(program, '/');
    if (p)
        program = ++p;

    fprintf( stderr, "%s v%s -- ETSI CoAP plugtest server\n"
            "(c) 2012 Olaf Bergmann <bergmann@tzi.org>\n\n"
            "usage: %s [-A address] [-p port]\n\n"
            "\t-A address\tinterface address to bind to\n"
            "\t-p port\t\tlisten on specified port\n"
            "\t-v num\t\tverbosity level (default: 3)\n", program, version,
            program);
}

coap_context_t *
get_context(const char *node, const char *port)
{
    coap_context_t *ctx = NULL;
    int s;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Coap uses UDP */
    hints.ai_flags = AI_PASSIVE | AI_NUMERICHOST;

    s = getaddrinfo(node, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return NULL;
    }

    /* iterate through results until success */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        coap_address_t addr;

        if (rp->ai_addrlen <= sizeof(addr.addr)) {
            coap_address_init(&addr);
            addr.size = rp->ai_addrlen;
            memcpy(&addr.addr, rp->ai_addr, rp->ai_addrlen);

            ctx = coap_new_context(&addr);
            if (ctx) {
                /* TODO: output address:port for successful binding */
                goto finish;
            }
        }
    }

    fprintf(stderr, "no context available for interface '%s'\n", node);

    finish: freeaddrinfo(result);
    return ctx;
}

void *helper(void *p)
{
    coap_context_t *ctx = (coap_context_t *) p;
    struct timeval tv, *timeout;
    coap_queue_t *nextpdu;
    coap_tick_t now;

    while (1) {
        pthread_mutex_lock(&mtx);
        nextpdu = coap_peek_next(ctx);

        coap_ticks(&now);
        while (nextpdu && nextpdu->t <= now) {
            coap_retransmit(ctx, coap_pop_next(ctx));
            nextpdu = coap_peek_next(ctx);
        }

        if (nextpdu && nextpdu->t <= now + COAP_RESOURCE_CHECK_TIME_SEC) {
            /* set timeout if there is a pdu to send before our automatic timeout occurs */
            tv.tv_usec = ((nextpdu->t - now) % COAP_TICKS_PER_SECOND)
                    * 1000000/ COAP_TICKS_PER_SECOND;
            tv.tv_sec = (nextpdu->t - now) / COAP_TICKS_PER_SECOND;
            timeout = &tv;
        } else {
            tv.tv_usec = 0;
            tv.tv_sec = COAP_RESOURCE_CHECK_TIME_SEC;
            timeout = &tv;
        }
        /* check if we have to send asynchronous responses */
        check_async(ctx, now);

        pthread_mutex_unlock(&mtx);

#define SEC_IN_USEC 1000000
        usleep(timeout->tv_sec + timeout->tv_usec * SEC_IN_USEC);
    }

    return NULL;
}


/**
 * call with: etsi_main <addr> <port> <log_level>
 */
int etsi_main(int argc, char **argv)
{
    coap_context_t *ctx;
    char addr_str[NI_MAXHOST] = "::";
    char port_str[NI_MAXSERV] = "5683";
    coap_log_t log_level = LOG_WARNING;

    if (argc < 3) {
        usage(argv[0], PACKAGE_VERSION);
        return -1;
    }

    strncpy(addr_str, argv[1], NI_MAXHOST - 1);
    strncpy(port_str, argv[2], NI_MAXSERV - 1);
    log_level = strtol(argv[3], NULL, 10);

    coap_set_log_level(log_level);

    ctx = get_context(addr_str, port_str);
    if (!ctx)
        return -1;

    coap_register_option(ctx, COAP_OPTION_BLOCK2);

    init_resources(ctx);

    pthread_t th_id;
    pthread_attr_t th_attr;
    pthread_attr_init(&th_attr);
    pthread_mutex_init(&mtx, NULL);
    pthread_create(&th_id, &th_attr, helper, (void *) ctx);

    while (1) {
        pthread_mutex_lock(&mtx);
        coap_read(ctx, &mtx); /* read received data */
        coap_dispatch(ctx); /* and dispatch PDUs from receivequeue */
        pthread_mutex_unlock(&mtx);
    }

    size_t res;
    pthread_join(th_id, (void **) &res);
    pthread_mutex_destroy(&mtx);
    pthread_attr_destroy(&th_attr);

    coap_free_context(ctx);

    return 0;
}
