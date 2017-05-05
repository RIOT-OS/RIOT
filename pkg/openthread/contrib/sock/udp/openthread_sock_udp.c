/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <errno.h>

#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "timex.h"
#include "net/af.h"
#include "ot.h"
#include "openthread/thread.h"
#include "openthread/udp.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

otUdpSocket socket;

/**
 * @brief   Internal helper functions for OPENTHREAD
 * @internal
 * @{
 */
/**
 * @brief   Checks if address family is not supported
 * @internal
 */
static inline bool openthread_af_not_supported(int af)
{
    /* TODO: add AF_INET support */
    return (af != AF_INET6);
}

/**
 * @brief   Check if end point points to any address
 * @internal
 */
static inline bool openthread_ep_addr_any(const sock_ip_ep_t *ep)
{
    assert(ep != NULL);
    const uint8_t *p = (uint8_t *)&ep->addr;
    for (uint8_t i = 0; i < sizeof(ep->addr); i++) {
        if (p[i] != 0) {
            return false;
        }
    }
    return true;
}

static void _handle_receive(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo)
{
    size_t payload_len = otMessageGetLength(aMessage)-otMessageGetOffset(aMessage);

    char buf[100];
    otMessageRead(aMessage, otMessageGetOffset(aMessage), buf, payload_len);

    printf("Message: ");
    for(int i=0;i<payload_len;i++)
    {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

static OT_JOB _create_udp_socket(otInstance *ot_instance, void *data)
{
    otSockAddr sockaddr;
    memset(&sockaddr, 0, sizeof(otSockAddr));
    sockaddr.mPort = *((uint16_t*) data);

    otUdpOpen(ot_instance, &socket, _handle_receive, NULL);
    otUdpBind(&socket, &sockaddr);
}

static OT_JOB _send_udp_pkt(otInstance *ot_instance, void *data)
{
    udp_pkt_t *pkt = (udp_pkt_t*) data;
    otMessage *message;
    message = otUdpNewMessage(ot_instance, true);
    otMessageSetLength(message, pkt->len);
    printf("%s\n", (uint8_t*) pkt->payload);
    printf("%i\n", pkt->len);
    otMessageWrite(message, 0, pkt->payload, pkt->len);
    otUdpSocket socket;
    otMessageInfo mPeer;
    
    //Set dest address
    memcpy(&mPeer.mPeerAddr.mFields, &(pkt->ip_addr), sizeof(ipv6_addr_t));

    //Set dest port
    mPeer.mPeerPort = pkt->port;
    printf("%p\n", message);

    otUdpSend(&socket, message, &mPeer);
    puts("ASD");
    printf("Pkt sent\n");
}

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
    DEBUG("openthread: sock_udp_create\n");

    uint16_t socket_port;

    assert(sock);
    assert(local == NULL || local->port != 0);
    assert(remote == NULL || remote->port != 0);
    if ((local != NULL) && (remote != NULL) &&
        (local->netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (local->netif != remote->netif)) {
        return -EINVAL;
    }
    
    memset(&sock->local, 0, sizeof(sock_udp_ep_t));
    
    if (local != NULL) {
        if (openthread_af_not_supported(local->family)) {
            return -EAFNOSUPPORT;
        }
        memcpy(&sock->local, local, sizeof(sock_udp_ep_t));
    }
    
    memset(&sock->remote, 0, sizeof(sock_udp_ep_t));
    
    if (remote != NULL) {
        if (openthread_af_not_supported(remote->family)) {
            return -EAFNOSUPPORT;
        }
        if (openthread_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        memcpy(&sock->remote, remote, sizeof(sock_udp_ep_t));
    }
    
    socket_port = local->port;

    if (local != NULL) {
        ot_exec_job(_create_udp_socket, &(socket_port));
    }
    sock->flags = flags;
    return 0;

    return 0;
}

void sock_udp_close(sock_udp_t *sock)
{

}

int sock_udp_get_local(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    return 0;
}

int sock_udp_get_remote(sock_udp_t *sock, sock_udp_ep_t *ep)
{
    return 0;
}

ssize_t sock_udp_recv(sock_udp_t *sock, void *data, size_t max_len,
                      uint32_t timeout, sock_udp_ep_t *remote)
{
    return 0;
}

ssize_t sock_udp_send(sock_udp_t *sock, const void *data, size_t len,
                      const sock_udp_ep_t *remote)
{
    return 0;
}

/** @} */
