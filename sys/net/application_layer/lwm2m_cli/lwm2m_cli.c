/*
 * Copyright (C) 2015 Alexandru Razvan Caciulescu
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>

#include "log.h"
#include "thread.h"
#include "net/gnrc.h"
#include "net/ipv6/addr.h"

#include "net/lwm2m_cli.h"
#include "net/lwm2m_cli/udp_helper.h"
#include "net/lwm2m_cli/objects.h"
#include "net/lwm2m_cli/object_util.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"


/**
 * @brief   LWM2M thread configuration
 * @{
 */
#define STACKSIZE               (THREAD_STACKSIZE_MAIN)
#define PRIO                    (THREAD_PRIORITY_MAIN - 2)
#define MSG_QUEUE_SIZE          (8U)
/** @} */

/**
 * @brief   Stack for the LWM2M thread
 */
static char stack[STACKSIZE];


/**
 * @brief   Global context for this client
 */
lwm2m_context_t *lwm2m_cli_ctx = NULL;

/**
 * @brief   Local object store
 */
#define OBJECT_NUMOF        (3U)
lwm2m_object_t *obj_store[OBJECT_NUMOF];

/**
 * @brief   Temporary connection information, for now we can handle only 1
 *          (IPv6) connection at a time...
 */
typedef struct {
    ipv6_addr_t addr;
    uint16_t port;
} connection_t;

/**
 * @brief   We can handle one connection at a time - this is where we save the
 *          connection details
 */
static connection_t connection = {IPV6_ADDR_UNSPECIFIED, 0};


static void *connect_cb(uint16_t sec_obj_id, void * arg)
{
    (void)sec_obj_id;
    (void)arg;

    /* as we can only one connection at a time, we return the local connection
     * data here */
    return &connection;
}

static uint8_t buf_send_cb(void *session, uint8_t *buf, size_t len,
                           void *userdata)
{
    DEBUG("send_cb: got session:  %p\n", session);
    DEBUG("send_cb: got userdata: %p\n", userdata);

    connection_t *conn = (connection_t *)session;
    if (conn == NULL) {
        LOG_WARNING("[lwm2m] send_cb: unable to parse connection data\n");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }
    if (udp_helper_send(&(conn->addr), conn->port, buf, len) == 0) {
        LOG_WARNING("[lwm2m] send_cb: sending data failed somehow\n");
        return COAP_500_INTERNAL_SERVER_ERROR;
    }
    /* TODO: update statistics */
    return COAP_NO_ERROR;
}

static void *lwm2m_cli_thread(void *arg)
{
    msg_t msg;
    msg_t msg_queue[MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t netreg;
    uint16_t port;
    gnrc_pktsnip_t *snip;
    time_t timer = 1;

    /* setup the message queue */
    msg_init_queue(msg_queue, MSG_QUEUE_SIZE);
    /* register for given port */
    port = *((uint16_t *)arg);
    netreg.pid = thread_getpid();
    netreg.demux_ctx = port;
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &netreg);

    /* carry out pending operations */
    DEBUG("timeofday is: %i\n", (int)lwm2m_gettime());
    DEBUG("thread: now stepping for the first time\n");
    lwm2m_step(lwm2m_cli_ctx, &timer);

    while (1) {
        DEBUG("timeofday is: %i\n", (int)lwm2m_gettime());
        DEBUG("waiting for incoming messages now\n");
        msg_receive(&msg);

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                LOG_INFO("[l2m2m] receive data packet\n");
                snip = (gnrc_pktsnip_t *)msg.content.ptr;

                /* pass incoming data to liblwm2m */
                DEBUG("handle_packet: giving connection to liblwm2m: %p\n",
                      (void *)&connection);
                lwm2m_handle_packet(lwm2m_cli_ctx, snip->data, (int)snip->size,
                                    &connection);
                /* TODO: care about statistics? */
                // conn_s_updateRxStatistic(obj_store[7], len, false);
                /* finally, trigger any pending operations */
                lwm2m_step(lwm2m_cli_ctx, &timer);

                gnrc_pktbuf_release(snip);
                break;
            default:
                LOG_INFO("[lwm2m] received unknown message type\n");
                break;
        }
    }
    /* never reached */
    return NULL;
}

int lwm2m_cli_init(char *name, ipv6_addr_t *addr, uint16_t port)
{
    int res;

    /* set the local connection variable */
    DEBUG("init: setting connection variable\n");
    memcpy(&(connection.addr), addr, sizeof(ipv6_addr_t));
    connection.port = port;

    /* setting up the object store */
    /* TODO: separate the object store and make it expandable */
    DEBUG("init: initializing the object store\n");
    lwm2m_cli_obj_init(obj_store, OBJECT_NUMOF);

    /* initialize the LWM2M context data structure for the client */
    DEBUG("init: calling lwm2m_init()\n");
    DEBUG("init: giving connection as argument to init function: %p\n",
          (void *)&connection);
    lwm2m_cli_ctx = lwm2m_init(connect_cb, buf_send_cb, &connection);
    if (lwm2m_cli_ctx == NULL) {
        puts("error: lwm2m_init() failed");
        return 1;
    }

#ifdef LWM2M_BOOTSTRAP
    /* for now, we do not do any bootstrapping by default */
    DEBUG("init: setting bootstrap state to NOT_BOOTSTRAPPED\n");
    lwm2m_cli_ctx->bsState = NOT_BOOTSTRAPPED;
#endif

    /* now let's initialize the library, giving it a (unique) name of the client
     * and a pointer to the prepared object store */
    DEBUG("init: calling lwm2m_configure()\n");
    res = lwm2m_configure(lwm2m_cli_ctx, name, NULL, NULL, OBJECT_NUMOF, obj_store);
    if (res != 0) {
        printf("error: lwm2m_configure() failed [%i]\n", res);
        return 1;
    }

    /* start the client */
    DEBUG("init: starting the server\n");
    res = lwm2m_start(lwm2m_cli_ctx);
    if (res != 0) {
        printf("error: lwm2m_start() failed [%i]\n", res);
        return 1;
    }

    /* create and run the LWM2M thread */
    thread_create(stack, sizeof(stack), PRIO, CREATE_STACKTEST,
                  lwm2m_cli_thread, &port, "lwm2m");
    return 0;
}

int lwm2m_cli_res_change(lwm2m_uri_t *uri, uint8_t *val, size_t len)
{

    /* test if the given URI specifies a resource */
    if ((uri == NULL) || !(uri->flag & LWM2M_URI_FLAG_RESOURCE_ID)) {
        LOG_WARNING("[lwm2m_cli] res_change: invalid URI given\n");
        return -1;
    }

    /* get the effected object instance */
    lwm2m_object_t *obj = lwm2m_cli_obj_find(lwm2m_cli_ctx, uri->objectId);
    if (obj == NULL) {
        LOG_WARNING("[lwm2m_cli] res_change: invalid object given\n");
        return -1;
    }

    lwm2m_tlv_t * tlvP;
    int result;

    tlvP = lwm2m_tlv_new(1);
    tlvP->flags = LWM2M_TLV_FLAG_STATIC_DATA | LWM2M_TLV_FLAG_TEXT_FORMAT;

    tlvP->id = uri->resourceId;
    tlvP->length = len;
    tlvP->value = (uint8_t*) val;

    result = obj->writeFunc(uri->instanceId, 1, tlvP, obj);

    if (COAP_204_CHANGED != result)
    {
        printf("error: Failed to change value!\n");
    }
    else
    {
        printf("value changed!\n");
        lwm2m_resource_value_changed(lwm2m_cli_ctx, uri);
    }
    lwm2m_tlv_free(1, tlvP);

    return 1;
}
