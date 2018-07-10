/*******************************************************************************
*
* Copyright (c) 2015 Intel Corporation and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* and Eclipse Distribution License v1.0 which accompany this distribution.
*
* The Eclipse Public License is available at
*    http://www.eclipse.org/legal/epl-v10.html
* The Eclipse Distribution License is available at
*    http://www.eclipse.org/org/documents/edl-v10.php.
*
* Contributors:
*    David Navarro, Intel Corporation - initial API and implementation
*    Christian Renz - Please refer to git log
*    Christian Manal - Ported to RIOT OS
*
*******************************************************************************/

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Handles DTLS/networking of the Wakaama example client
 *
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "connection.h"
#include "memarray_limits.h"
#include "xtimer.h"

/*
 * RIOT debugging
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

#define URI_LENGTH 256

#ifdef MODULE_TINYDTLS
static dtls_context_t *dtls_context;
#endif /* MODULE_TINYDTLS */

/********************* Security Obj Helpers **********************/
static char *security_get_uri(lwm2m_object_t *obj, int instance_id, char *uri_buffer,
                              int buffer_size)
{
    int size = 1;
    lwm2m_data_t *data = lwm2m_data_new(size);

    data->id = 0; /* security server uri */

    obj->readFunc(instance_id, &size, &data, obj);
    if (data != NULL && data->type == LWM2M_TYPE_STRING &&
        data->value.asBuffer.length > 0) {
        if ((size_t)buffer_size > data->value.asBuffer.length) {
            strncpy(uri_buffer, (char *)data->value.asBuffer.buffer,
                    data->value.asBuffer.length);
            lwm2m_data_free(size, data);
            return uri_buffer;
        }
    }
    lwm2m_data_free(size, data);
    return NULL;
}

#ifdef MODULE_TINYDTLS
static int64_t security_get_mode(lwm2m_object_t *obj, int instance_id)
{
    int64_t mode;
    int size = 1;
    lwm2m_data_t *data = lwm2m_data_new(size);

    data->id = 2; /* security mode */

    obj->readFunc(instance_id, &size, &data, obj);
    if (0 != lwm2m_data_decode_int(data, &mode)) {
        lwm2m_data_free(size, data);
        return mode;
    }

    lwm2m_data_free(size, data);
    fprintf(stderr, "Unable to get security mode : use not secure mode");
    return LWM2M_SECURITY_MODE_NONE;
}

static char *security_get_key(lwm2m_object_t *obj, int key_type, int instance_id, int *length)
{
    int size = 1;

    /*
     * 3 = Public Key or ID
     * 4 = Server Public Key or ID
     * 5 = Secret Key
     */
    if (key_type < 3 || key_type > 5) {
        DEBUG("Invalid key type\n");
        return NULL;
    }

    lwm2m_data_t *data = lwm2m_data_new(size);
    data->id = key_type;

    obj->readFunc(instance_id, &size, &data, obj);
    if (data != NULL && data->type == LWM2M_TYPE_OPAQUE) {
        char *buff;

        buff = (char *)lwm2m_malloc(data->value.asBuffer.length);
        if (buff != NULL) {
            memcpy(buff, data->value.asBuffer.buffer,
                   data->value.asBuffer.length);
            *length = data->value.asBuffer.length;
        }
        lwm2m_data_free(size, data);

        return buff;
    }
    else {
        return NULL;
    }
}
#endif /* MODULE_TINYDTLS */

/********************* Security Obj Helpers Ends **********************/

static int send_data(lwm2m_connection_t *conn, uint8_t *buffer, size_t length)
{
    ssize_t bytes_sent = sock_udp_send(&(conn->sock), buffer, length, NULL);

    /* TODO Error handling */
    if (bytes_sent <= 0) {
        DEBUG("%s: Error code for sock_udp_send %i", __func__, bytes_sent);
        return -1;
    }
    conn->last_send = lwm2m_gettime();
    return 0;
}

/**************************  TinyDTLS Callbacks  ************************/
#ifdef MODULE_TINYDTLS

#ifdef DTLS_PSK
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int get_psk_info(struct dtls_context_t *ctx, const session_t *session,
                        dtls_credentials_type_t type, const unsigned char *id,
                        size_t id_len, unsigned char *result,
                        size_t result_length)
{
    (void)session;
    lwm2m_connection_t *conn = (lwm2m_connection_t *)ctx->app;
    lwm2m_connection_t *cnx = lwm2m_connection_find((lwm2m_connection_t *)ctx->app,
                                                    &(conn->sock.remote));

    (void)id;
    (void)id_len;

    if (cnx == NULL) {
        printf("GET PSK session not found\n");
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    switch (type) {
        case DTLS_PSK_IDENTITY: {
            int idLen;
            char *id;
            id = security_get_key(cnx->security_obj, 3, cnx->security_inst_id,
                                  &idLen);
            if (result_length < (size_t)idLen) {
                printf("cannot set psk_identity -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, id, idLen);
            lwm2m_free(id);
            return idLen;
        }
        case DTLS_PSK_KEY: {
            int keyLen;
            char *key;
            key = security_get_key(cnx->security_obj, 5, cnx->security_inst_id,
                                   &keyLen);

            if (result_length < (size_t)keyLen) {
                printf("cannot set psk -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, key, keyLen);
            lwm2m_free(key);
            return keyLen;
        }
        default:
            printf("unsupported request type: %d\n", type);
    }

    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static int get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                         const dtls_ecdsa_key_t **result)
{
    int keyLen;

    lwm2m_connection_t *conn = (lwm2m_connection_t *)ctx->app;
    lwm2m_connection_t *cnx = lwm2m_connection_find((lwm2m_connection_t *)ctx->app,
                                                    &(conn->sock.remote));

    if (cnx == NULL) {
        printf("GET ECDSA KEY session not found\n");
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    dtls_ecdsa_key_t *res = lwm2m_malloc(sizeof(dtls_ecdsa_key_t));
    if (res == NULL) {
        printf("malloc() error\n");
        return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }

    res->priv_key = (const unsigned char *)security_get_key(
        cnx->security_obj, 3, cnx->security_inst_id, &keyLen);

    res->pub_key_x = (const unsigned char *)security_get_key(
        cnx->security_obj, 4, cnx->security_inst_id, &keyLen);

    res->pub_key_y = (const unsigned char *)security_get_key(
        cnx->security_obj, 5, cnx->security_inst_id, &keyLen);

    *result = res;

    return 0;
}

/* TODO: Actually verify keys... */
static int verify_ecdsa_key(struct dtls_context_t *ctx,
                            const session_t *session,
                            const unsigned char *other_pub_x,
                            const unsigned char *other_pub_y,
                            size_t key_size)
{
    (void)ctx;
    (void)session;
    (void)other_pub_x;
    (void)other_pub_y;
    (void)key_size;
    return 0;
}
#endif /* DTLS_ECC */


static int send_to_peer(struct dtls_context_t *ctx, session_t *session,
                        uint8 *data, size_t len)
{
    (void)session;
    lwm2m_connection_t *conn = (lwm2m_connection_t *)ctx->app;
    lwm2m_connection_t *cnx = lwm2m_connection_find((lwm2m_connection_t *)ctx->app,
                                                    &(conn->sock.remote));
    DEBUG("send_to_peer(%d)\n", len);
    if (cnx != NULL) {
        int err = send_data(cnx, data, len);
        if (COAP_NO_ERROR != err) {
            return -1;
        }
        return len;
    }
    return -1;
}

static int read_from_peer(struct dtls_context_t *ctx, session_t *session,
                          uint8 *data, size_t len)
{
    (void)session;
    DEBUG("read_from_peer(%d)\n", len);
    lwm2m_connection_t *conn = (lwm2m_connection_t *)ctx->app;
    lwm2m_connection_t *cnx = lwm2m_connection_find((lwm2m_connection_t *)ctx->app,
                                                    &(conn->sock.remote));
    if (cnx != NULL) {
        lwm2m_handle_packet(cnx->lwm2mH, (uint8_t *)data, len, (void *)cnx);
        return 0;
    }
    return -1;
}
/**************************   TinyDTLS Callbacks Ends ************************/

static dtls_handler_t cb = {
    .write = send_to_peer,
    .read = read_from_peer,
    .event = NULL,
    #ifdef DTLS_PSK
    .get_psk_info = get_psk_info,
    #endif /* DTLS_PSK */
    #ifdef DTLS_ECC
    .get_ecdsa_key = get_ecdsa_key,
    .verify_ecdsa_key = verify_ecdsa_key
    #endif /* DTLS_ECC */
};

static dtls_context_t *get_dtls_context(lwm2m_connection_t *conn_list)
{
    if (dtls_context == NULL) {
        dtls_context = dtls_new_context(conn_list);
        if (dtls_context == NULL) {
            fprintf(stderr, "Failed to create the DTLS context\r\n");
        }
        dtls_set_handler(dtls_context, &cb);
    }
    else {
        dtls_context->app = conn_list;
    }
    return dtls_context;
}
#endif /* MODULE_TINYDTLS */

lwm2m_connection_t *lwm2m_connection_find(lwm2m_connection_t *conn_list,
                                          const sock_udp_ep_t *remote)
{
    lwm2m_connection_t *conn;

    conn = conn_list;
    while (conn != NULL) {
        if (conn->sock.remote.port == remote->port &&
            memcmp(&conn->sock.remote.addr.ipv6, &remote->addr.ipv6, sizeof(remote->addr.ipv6)) == 0) {
            return conn;
        }

        conn = conn->next;
    }

    return conn;
}

static lwm2m_connection_t *connection_new_incoming(lwm2m_connection_t *conn_list,
                                                   const sock_udp_ep_t *remote)
{
    lwm2m_connection_t *conn;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    ssize_t res;

    conn = lwm2m_malloc(sizeof(lwm2m_connection_t));
    if (conn != NULL) {
        memset(conn, 0, sizeof(lwm2m_connection_t));
        conn->next = conn_list;

        local.port = LWM2M_DTLS_PORT;
        res = sock_udp_create(&(conn->sock), &local, remote, 0);
        if (res < 0) {
            fprintf(stderr, "ERROR: sock_udp_create failed\n");
            goto free_conn;
        }
        conn->last_send = lwm2m_gettime();

#ifdef MODULE_TINYDTLS
        conn->no_sec = false;
        conn->dtls_session = lwm2m_malloc(sizeof(session_t));
        memset(conn->dtls_session, 0, sizeof(session_t));
        if (memcpy(&(conn->dtls_session->addr), remote->addr.ipv6, (sizeof(uint8_t) * 16)) == NULL) {
            fprintf(stderr, "ERROR: memcpu failed\n");
            goto free_conn;
        }
        conn->dtls_session->size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
        conn->dtls_session->port = remote->port;
#endif  /* MODULE_TINYDTLS */
    }

    return conn;

free_conn:
    lwm2m_free(conn);
    return NULL;
}

lwm2m_connection_t *lwm2m_connection_create(lwm2m_connection_t *conn_list,
                                            lwm2m_object_t *security_obj,
                                            int instance_id, lwm2m_context_t *lwm2mH)
{
    lwm2m_connection_t *conn = NULL;
    char uri_buf[URI_LENGTH+1];
    char *uri;
    char *host;
    char *tmp;
    char *port;
    char *defaultport;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    memset(uri_buf, 0, URI_LENGTH+1);

    uri = security_get_uri(security_obj, instance_id, uri_buf, URI_LENGTH);
    if (uri == NULL) {
        fprintf(stderr, "lwm2m_connection_create(): uri == NULL\n");
        return NULL;
    }

    /* parse uri in the form "coaps://[host]:[port]" */
    if (0 == strncmp(uri, "coaps://", strlen("coaps://"))) {
        host = uri + strlen("coaps://");
        defaultport = LWM2M_DTLS_PORT_STR;
    }
    else if (0 == strncmp(uri, "coap://", strlen("coap://"))) {
        host = uri + strlen("coap://");
        defaultport = LWM2M_STANDARD_PORT_STR;
    }
    else {
        fprintf(stderr, "lwm2m_connection_create(): Invalid protocol in URI\n");
        return NULL;
    }

    tmp = host;
    DEBUG("lwm2m_connection_create(): getting port from %s\n", host);
    if (tmp[0] == '[') {
        DEBUG("lwm2m_connection_create(): getting port: IPv6 address\n");
        host++;
        tmp = strrchr(tmp, ']');
    }

    port = strrchr(tmp, ':');
    if (port == NULL) {
        *tmp = '\0';
        DEBUG("lwm2m_connection_create(): using defaultport %s\n",
              defaultport);
        port = defaultport;
    }
    else {
        *(port - 1) = '\0';
        port++;
    }

    DEBUG("lwm2m_connection_create(): ipv6_addr_from_str(&addr, %s)\n", host);
    if (ipv6_addr_from_str((ipv6_addr_t *)&remote.addr, host) == NULL) {
        fprintf(stderr, "lwm2m_connection_create(): IPv6 address malformed\n");
        return NULL;
    }

    if (ipv6_addr_is_unspecified((const ipv6_addr_t *)&remote.addr)) {
        fprintf(stderr, "lwm2m_connection_create(): Invalid remote address ([::])\n");
        return NULL;
    }

    remote.port = atoi(port);

    /* If the address is a link-local one and we only have one netif, default to that.
     * Otherwise throw an error, so we don't perma-clog the send queue.
     */
    if (ipv6_addr_is_link_local((ipv6_addr_t *)&remote.addr.ipv6)) {
        if (gnrc_netif_numof() == 1) {
            gnrc_netif_t *netif = gnrc_netif_iter(NULL);
            remote.netif = netif->pid;
        }
        else {
            fprintf(stderr, "lwm2m_connection_create(): Can't use link-local address without interface number\n");
        }
    }

    DEBUG("lwm2m_connection_create(): connection_new_incoming(%s, %d)\n", host, remote.port);
    conn = connection_new_incoming(conn_list, &remote);

    /* do we need to start tinydtls? */
    if (conn != NULL) {
        conn->lwm2mH = lwm2mH;
#ifdef MODULE_TINYDTLS
        conn->security_obj = security_obj;
        conn->security_inst_id = instance_id;

        if (security_get_mode(conn->security_obj, conn->security_inst_id) !=
            LWM2M_SECURITY_MODE_NONE) {
            conn->dtls_context = get_dtls_context(conn);
        }
        else {
            /* no dtls session */
            conn->no_sec = true;
        }
#endif  /* MODULE_TINYDTLS */
    }

    return conn;
}

void lwm2m_connection_free(lwm2m_connection_t *conn_list)
{
#ifdef MODULE_TINYDTLS
    dtls_free_context(dtls_context);
    dtls_context = NULL;
#endif /* MODULE_TINYDTLS */
    while (conn_list != NULL) {
        lwm2m_connection_t *next;

        next = conn_list->next;
        sock_udp_close(&conn_list->sock);
#ifdef MODULE_TINYDTLS
        lwm2m_free(conn_list->dtls_session);
#endif  /* MODULE_TINYDTLS */
        lwm2m_free(conn_list);

        conn_list = next;
    }
}

static int connection_send(lwm2m_connection_t *conn, uint8_t *buffer, size_t length)
{
    DEBUG("connection_send(%d)\n", length);
#ifdef MODULE_TINYDTLS
    if (conn->no_sec) {
#endif /* MODULE_TINYDTLS */
        /* no security */
        if (0 != send_data(conn, buffer, length)) {
            return -1;
        }
#ifdef MODULE_TINYDTLS
    }
    else {
        ssize_t bw = 0;
        if (DTLS_NAT_TIMEOUT > 0 &&
            (lwm2m_gettime() - conn->last_send) > DTLS_NAT_TIMEOUT) {
            /* we need to rehandhake because our source IP/port probably changed
             * for the server
             */
            if (lwm2m_connection_rehandshake(conn, true) != 0) {
                printf("can't send due to rehandshake error\n");
                return -1;
            }
        }
        DEBUG("dtls_write(%d)\n", length);
        bw = dtls_write(conn->dtls_context, conn->dtls_session, buffer, length);
        if (bw <= 0) {
            DEBUG("dtls_write() failed (%d)\n", bw);
            return -1;
        }
        DEBUG("dtls_write() wrote %d bytes\n", bw);
    }
#endif /* MODULE_TINYDTLS */

    return 0;
}

int lwm2m_connection_handle_packet(lwm2m_connection_t *conn, uint8_t *buffer,
                                   size_t num_bytes)
{
#ifdef MODULE_TINYDTLS
    if (!conn->no_sec) {
        /* Let liblwm2m respond to the query depending on the context */
        DEBUG("lwm2m_connection_handle_packet() -> dtls_handle_message()\n");
        int result = dtls_handle_message(conn->dtls_context, conn->dtls_session,
                                         buffer, num_bytes);
        if (result != 0) {
            printf("error dtls handling message %d\n", result);
        }
        return result;
    }
    else {
#endif /* MODULE_TINYDTLS */
    /* no security, just give the plaintext buffer to liblwm2m */
    DEBUG("lwm2m_connection_handle_packet() -> lwm2m_handle_packet()\n");
    lwm2m_handle_packet(conn->lwm2mH, buffer, num_bytes, (void *)conn);
    return 0;
#ifdef MODULE_TINYDTLS
}
#endif /* MODULE_TINYDTLS */
}

#ifdef MODULE_TINYDTLS
int lwm2m_connection_rehandshake(lwm2m_connection_t *conn, bool send_close_notify)
{
    /* if not a dtls connection we do nothing */
    if (conn->no_sec) {
        return 0;
    }

    /* reset current session */
    dtls_peer_t *peer = dtls_get_peer(conn->dtls_context, conn->dtls_session);
    if (peer != NULL) {
        if (!send_close_notify) {
            peer->state = DTLS_STATE_CLOSED;
        }
        dtls_reset_peer(conn->dtls_context, peer);
    }

    /* start a fresh handshake */
    int result = dtls_connect(conn->dtls_context, conn->dtls_session);
    if (result != 0) {
        printf("error dtls reconnection %d\n", result);
    }
    return result;
}
#endif /* MODULE_TINYDTLS */

uint8_t lwm2m_buffer_send(void *sessionH, uint8_t *buffer, size_t length,
                          void *userdata)
{
    lwm2m_connection_t *conn = (lwm2m_connection_t *)sessionH;

    (void)userdata;

    if (conn == NULL) {
        fprintf(stderr, "#> failed sending %zu bytes, missing connection\r\n",
                length);
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    if (-1 == connection_send(conn, buffer, length)) {
        fprintf(stderr, "#> failed sending %zu bytes\r\n", length);
        return COAP_500_INTERNAL_SERVER_ERROR;
    }

    return COAP_NO_ERROR;
}

bool lwm2m_session_is_equal(void *session1, void *session2, void *user_data)
{
    (void)user_data;
    return (session1 == session2);
}
