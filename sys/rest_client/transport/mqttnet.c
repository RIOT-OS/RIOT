/*
 * Copyright (C) 2006-2022 wolfSSL Inc.
 * Copyright (C) 2022 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     sys_rest_client_transport_mqtt
 *
 * @{
 * @file
 * @brief       Utility for wolfMQTT
 * @note        Code was derived from the wolfMQTT repository (see https://github.com/wolfSSL/wolfMQTT/blob/master/examples/mqttnet.c)
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

/* Include the autoconf generated config.h */
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "wolfmqtt/mqtt_client.h"

#include "kernel_defines.h"
#include <assert.h>

#include "rest_client/mqttnet.h"
#include "rest_client/mqttexample.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/sock/dns.h"
#include "net/sock/ip.h"
#include "net/sock/tcp.h"
#include "net/sock/udp.h"

#include "ztimer.h"

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define ENABLE_DEBUG REST_CLIENT_ENABLE_DEBUG
#include "debug.h"

#define SOCK_OPEN       socket
#define SOCKET_T        int
#define SOERROR_T       int
#define SELECT_FD(fd)   ((fd) + 1)
#define SOCKET_INVALID  ((SOCKET_T)0)
#define SOCK_CONNECT    connect
#define SOCK_SEND(s,b,l,f) send((s), (b), (size_t)(l), (f))
#define SOCK_RECV(s,b,l,f) recv((s), (b), (size_t)(l), (f))
#define SOCK_CLOSE      close
#define SOCK_ADDR_IN    struct sockaddr_storage
#define SOCK_ADDRINFO   struct addrinfo

/* Local context for Net callbacks */
typedef enum {
    SOCK_BEGIN = 0,
    SOCK_CONN,
} nb_stat_t;

typedef struct {
    SOCKET_T fd;
    nb_stat_t stat;
    SOCK_ADDR_IN addr;
    rest_client_mqtt_context_t *mqtt_context;
} socket_context_t;

#ifndef WOLFMQTT_NO_TIMEOUT
static void setup_timeout(struct timeval* tv, int timeout_ms)
{
    tv->tv_sec = timeout_ms / 1000;
    tv->tv_usec = (timeout_ms % 1000) * 1000;

    /* Make sure there is a minimum value specified */
    if (tv->tv_sec < 0 || (tv->tv_sec == 0 && tv->tv_usec <= 0)) {
        tv->tv_sec = 0;
        tv->tv_usec = 100;
    }
}

#ifdef WOLFMQTT_NONBLOCK
static void tcp_set_nonblocking(SOCKET_T *sockfd)
{
    int flags = fcntl(*sockfd, F_GETFL, 0);
    if (flags < 0)
        DEBUG("fcntl get failed!\n");
    flags = fcntl(*sockfd, F_SETFL, flags | O_NONBLOCK);
    if (flags < 0)
        DEBUG("fcntl set failed!\n");
}
#endif /* WOLFMQTT_NONBLOCK */
#endif /* !WOLFMQTT_NO_TIMEOUT */

static int _net_connect(void *context, const char *host, uint16_t port, int timeout_ms)
{
    socket_context_t *sock = (socket_context_t *)context;
    int rc = -1;
    SOERROR_T so_error = 0;
    rest_client_mqtt_context_t *mqtt_context = sock->mqtt_context;
    (void)mqtt_context;

    /* Get address information for host and locate IPv4 */
    switch(sock->stat) {
    case SOCK_BEGIN:
    {
        DEBUG("_net_connect: Host %s, Port %u, Timeout %d ms\n", host, port, timeout_ms);

        /* Default to error */
        rc = -1;

        /* Create socket */
        sock->fd = SOCK_OPEN(sock->addr.ss_family, SOCK_STREAM, 0);

        if (sock->fd == SOCKET_INVALID)
            goto exit;

        sock->stat = SOCK_CONN;

        FALL_THROUGH;
    }

    case SOCK_CONN:
    {
    #ifndef WOLFMQTT_NO_TIMEOUT
        fd_set fdset;
        struct timeval tv;

        /* Setup timeout and FD's */
        setup_timeout(&tv, timeout_ms);
        FD_ZERO(&fdset);
        FD_SET(sock->fd, &fdset);
    #endif /* !WOLFMQTT_NO_TIMEOUT */

    #if !defined(WOLFMQTT_NO_TIMEOUT) && defined(WOLFMQTT_NONBLOCK)
        if (mqtt_context->use_non_block_mode) {
            /* Set socket as non-blocking */
            tcp_set_nonblocking(&sock->fd);
        }
    #endif

        /* Default to error */
        rc = -1;

        /* Start connect */
        if (sock->addr.ss_family == AF_INET) {
            rc = SOCK_CONNECT(sock->fd, (struct sockaddr *) &sock->addr, sizeof(struct sockaddr_in));
        }
        else if (sock->addr.ss_family == AF_INET6) {
            rc = SOCK_CONNECT(sock->fd, (struct sockaddr *) &sock->addr, sizeof(struct sockaddr_in6));
        }

        if (rc < 0) {
            /* Check for error */
            socklen_t len = sizeof(so_error);
            getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &so_error, &len);

            /* set default error case */
            rc = MQTT_CODE_ERROR_NETWORK;
        #ifdef WOLFMQTT_NONBLOCK
            if (errno == EINPROGRESS || so_error == EINPROGRESS) {
            #ifndef WOLFMQTT_NO_TIMEOUT
                /* Wait for connect */
                if (select((int)SELECT_FD(sock->fd), NULL, &fdset, NULL, &tv) > 0) {
                    rc = MQTT_CODE_SUCCESS;
                }
            #else
                rc = MQTT_CODE_CONTINUE;
            #endif
            }
        #endif
        }
        break;
    }
    default:
        rc = -1;
    } /* switch */

    (void)timeout_ms;

exit:
    /* Show error */
    if (rc != 0) {
        DEBUG("_net_connect: Rc=%d, SoErr=%d\n", rc, so_error);
    }

    return rc;
}

#ifdef WOLFMQTT_SN
static int _sn_net_connect(void *context, const char *host, uint16_t port,
    int timeout_ms)
{
    socket_context_t *sock = (socket_context_t *)context;
    int rc = 0;
    SOERROR_T so_error = 0;

    DEBUG("_sn_net_connect: Host %s, Port %u, Timeout %d ms\n", host, port, timeout_ms);

    /* Default to success */
    rc = 0;

    /* Create socket */
    sock->fd = SOCK_OPEN(sock->addr.ss_family, SOCK_DGRAM, 0);

    if (sock->fd == SOCKET_INVALID) {
        rc = -1;
    }

    if (rc == 0) {
    #ifndef WOLFMQTT_NO_TIMEOUT
        fd_set fdset;
        struct timeval tv;

        /* Setup timeout and FD's */
        setup_timeout(&tv, timeout_ms);
        FD_ZERO(&fdset);
        FD_SET(sock->fd, &fdset);
    #else
        (void)timeout_ms;
    #endif /* !WOLFMQTT_NO_TIMEOUT */

        /* Default to error */
        rc = -1;

        /* Start connect */
        if (sock->addr.ss_family == AF_INET) {
            rc = SOCK_CONNECT(sock->fd, (struct sockaddr *) &sock->addr, sizeof(struct sockaddr_in));
        }
        else if (sock->addr.ss_family == AF_INET6) {
            rc = SOCK_CONNECT(sock->fd, (struct sockaddr *) &sock->addr, sizeof(struct sockaddr_in6));
        }
    }

    /* Show error */
    if (rc != 0) {
        SOCK_CLOSE(sock->fd);
        DEBUG("_sn_net_connect: Rc=%d, SoErr=%d\n", rc, so_error);
    }

    return rc;
}
#endif

static int _net_write(void *context, const uint8_t *buf, int buf_len, int timeout_ms)
{
    socket_context_t *sock = (socket_context_t *)context;
    int rc;
    SOERROR_T so_error = 0;
#ifndef WOLFMQTT_NO_TIMEOUT
    struct timeval tv;
#endif

    if (context == NULL || buf == NULL || buf_len <= 0) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (sock->fd == SOCKET_INVALID)
        return MQTT_CODE_ERROR_BAD_ARG;

#ifndef WOLFMQTT_NO_TIMEOUT
    /* Setup timeout */
    setup_timeout(&tv, timeout_ms);
    setsockopt(sock->fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));
#endif

    rc = (int)SOCK_SEND(sock->fd, buf, buf_len, 0);
    if (rc == -1) {
        /* Get error */
        socklen_t len = sizeof(so_error);
        getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
        if (so_error == 0) {
            rc = 0; /* Handle signal */
        }
        else {
        #ifdef WOLFMQTT_NONBLOCK
            if (so_error == EWOULDBLOCK || so_error == EAGAIN) {
                return MQTT_CODE_CONTINUE;
            }
        #endif
            rc = MQTT_CODE_ERROR_NETWORK;
            DEBUG("_net_write: Error %d\n", so_error);
        }
    }

    (void)timeout_ms;

    return rc;
}

static int _net_read_ex(void *context, uint8_t *buf, int buf_len, int timeout_ms, uint8_t peek)
{
    socket_context_t *sock = (socket_context_t *)context;
    rest_client_mqtt_context_t *mqtt_context = sock->mqtt_context;
    int rc = -1, timeout = 0;
    SOERROR_T so_error = 0;
    int bytes = 0;
    int flags = 0;
#ifndef WOLFMQTT_NO_TIMEOUT
    fd_set recvfds;
    fd_set errfds;
    struct timeval tv;
#endif

    (void)mqtt_context;

    if (context == NULL || buf == NULL || buf_len <= 0) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (sock->fd == SOCKET_INVALID) {
        return MQTT_CODE_ERROR_BAD_ARG;
    }

    if (peek == 1) {
        flags |= MSG_PEEK;
    }

#ifndef WOLFMQTT_NO_TIMEOUT
    /* Setup timeout */
    setup_timeout(&tv, timeout_ms);

    /* Setup select file descriptors to watch */
    FD_ZERO(&errfds);
    FD_SET(sock->fd, &errfds);
    FD_ZERO(&recvfds);
    FD_SET(sock->fd, &recvfds);
#else
    (void)timeout_ms;
#endif /* !WOLFMQTT_NO_TIMEOUT */

    /* Loop until buf_len has been read, error or timeout */
    while (bytes < buf_len) {
        int do_read = 0;

    #ifndef WOLFMQTT_NO_TIMEOUT
        #ifdef WOLFMQTT_NONBLOCK
        if (mqtt_context->use_non_block_mode) {
            do_read = 1;
        }
        else
        #endif
        {
            /* Wait for rx data to be available */
            rc = select((int)SELECT_FD(sock->fd), &recvfds, NULL, &errfds, &tv);
            if (rc > 0)
            {
                if (FD_ISSET(sock->fd, &recvfds)) {
                    do_read = 1;
                }

                /* Check if rx or error */
                if (FD_ISSET(sock->fd, &errfds)) {
                    // Todo: find out reason why we can ignore this case
                    //rc = -1;
                    //break;
                }
            }
            else {
                timeout = 1;
                break; /* timeout or signal */
            }
        }
    #else
        do_read = 1;
    #endif /* !WOLFMQTT_NO_TIMEOUT */

        if (do_read) {
            /* Try and read number of buf_len provided,
                minus what's already been read */
            rc = (int)SOCK_RECV(sock->fd,
                           &buf[bytes],
                           buf_len - bytes,
                           flags);
            if (rc <= 0) {
                rc = -1;
                goto exit; /* Error */
            }
            else {
                bytes += rc; /* Data */
            }
        }

        /* no timeout and non-block should always exit loop */
    #ifdef WOLFMQTT_NONBLOCK
        if (mqtt_context->use_non_block_mode) {
            break;
        }
    #endif
    #ifdef WOLFMQTT_NO_TIMEOUT
        break;
    #endif
    } /* while */

exit:

    if (rc == 0 && timeout) {
        rc = MQTT_CODE_ERROR_TIMEOUT;
    }
    else if (rc < 0) {
        /* Get error */
        socklen_t len = sizeof(so_error);
        getsockopt(sock->fd, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            rc = 0; /* Handle signal */
        }
        else {
        #ifdef WOLFMQTT_NONBLOCK
            if (so_error == EWOULDBLOCK || so_error == EAGAIN) {
                return MQTT_CODE_CONTINUE;
            }
        #endif
            rc = MQTT_CODE_ERROR_NETWORK;
            DEBUG("_net_read: Error %d\n", so_error);
        }
    }
    else {
        rc = bytes;
    }

    return rc;
}

static int _net_read(void *context, uint8_t *buf, int buf_len, int timeout_ms)
{
    return _net_read_ex(context, buf, buf_len, timeout_ms, 0);
}

#ifdef WOLFMQTT_SN
static int _net_peek(void *context, uint8_t *buf, int buf_len, int timeout_ms)
{
    return _net_read_ex(context, buf, buf_len, timeout_ms, 1);
}
#endif

static int _net_disconnect(void *context)
{
    socket_context_t *sock = (socket_context_t *)context;
    if (sock) {
        if (sock->fd != SOCKET_INVALID) {
            SOCK_CLOSE(sock->fd);
            sock->fd = -1;
        }

        sock->stat = SOCK_BEGIN;
    }
    return 0;
}

int mqttnet_init(MqttNet *net, rest_client_mqtt_context_t *mqtt_context)
{
    /* wait until connection has been established */
    ztimer_sleep(ZTIMER_SEC, 10);

    if (net) {
        socket_context_t *sockCtx;

        XMEMSET(net, 0, sizeof(MqttNet));
        net->connect = _net_connect;
        net->read = _net_read;
        net->write = _net_write;
        net->disconnect = _net_disconnect;

        sockCtx = (socket_context_t *)WOLFMQTT_MALLOC(sizeof(socket_context_t));
        if (sockCtx == NULL) {
            return MQTT_CODE_ERROR_MEMORY;
        }
        net->context = sockCtx;
        XMEMSET(sockCtx, 0, sizeof(socket_context_t));
        sockCtx->fd = SOCKET_INVALID;
        sockCtx->stat = SOCK_BEGIN;
        sockCtx->mqtt_context = mqtt_context;

        switch (mqtt_context->addr->type) {
    #ifdef SOCK_HAS_IPV4
        case AF_INET:
        {
            struct sockaddr_in *sa4 = (struct sockaddr_in *) &sockCtx->addr;
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(mqtt_context->port);
            XMEMCPY(&(sa4->sin_addr), mqtt_context->addr->ipv4, sizeof(struct in_addr));
            break;
        }
    #endif
    #ifdef SOCK_HAS_IPV6
        case AF_INET6:
        {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *) &sockCtx->addr;
            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(mqtt_context->port);
            XMEMCPY(&(sa6->sin6_addr), mqtt_context->addr->ipv6, sizeof(struct in6_addr));
            break;
        }
    #endif
            default:
                assert(0);
                return MQTT_CODE_ERROR_BAD_ARG;

        }
    }

    return MQTT_CODE_SUCCESS;
}

#ifdef WOLFMQTT_SN
int mqttnet_sn_init(MqttNet *net, rest_client_mqtt_context_t *mqtt_context)
{
    /* wait until connection has been established */
    ztimer_sleep(ZTIMER_SEC, 10);

    if (net) {
        socket_context_t *sockCtx;

        XMEMSET(net, 0, sizeof(MqttNet));
        net->connect = _sn_net_connect;
        net->read = _net_read;
        net->write = _net_write;
        net->peek = _net_peek;
        net->disconnect = _net_disconnect;

        sockCtx = (socket_context_t *)WOLFMQTT_MALLOC(sizeof(socket_context_t));
        if (sockCtx == NULL) {
            return MQTT_CODE_ERROR_MEMORY;
        }
        net->context = sockCtx;
        XMEMSET(sockCtx, 0, sizeof(socket_context_t));
        sockCtx->stat = SOCK_BEGIN;
        sockCtx->mqtt_context = mqtt_context;

        switch (mqtt_context->addr->type) {
    #ifdef SOCK_HAS_IPV4
        case AF_INET:
        {
            struct sockaddr_in *sa4 = (struct sockaddr_in *) &sockCtx->addr;
            sa4->sin_family = AF_INET;
            sa4->sin_port = htons(mqtt_context->port);
            XMEMCPY(&(sa4->sin_addr), mqtt_context->addr->ipv4, sizeof(struct in_addr));
            break;
        }
    #endif
    #ifdef SOCK_HAS_IPV6
        case AF_INET6:
        {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *) &sockCtx->addr;
            sa6->sin6_family = AF_INET6;
            sa6->sin6_port = htons(mqtt_context->port);
            XMEMCPY(&(sa6->sin6_addr), mqtt_context->addr->ipv6, sizeof(struct in6_addr));
            break;
        }
    #endif
            default:
                assert(0);
                return MQTT_CODE_ERROR_BAD_ARG;

        }
    }

    return MQTT_CODE_SUCCESS;
}
#endif

int mqttnet_deinit(MqttNet *net)
{
    if (net) {
        if (net->context) {
            WOLFMQTT_FREE(net->context);
        }
        XMEMSET(net, 0, sizeof(MqttNet));
    }
    return 0;
}
