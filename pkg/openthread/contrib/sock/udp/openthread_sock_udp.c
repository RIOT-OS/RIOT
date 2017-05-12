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
#include "net/udp.h"
#include "net/sock.h"
#include "timex.h"
#include "net/af.h"
#include "ot.h"
#include "openthread/thread.h"
#include "openthread/udp.h"
#include "byteorder.h"
#include "net/protnum.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

static otUdpSocket msocket;

typedef struct 
{
    ipv6_addr_t ip_addr;
    uint16_t port;
    void *payload;
    size_t len;
} udp_pkt_t;

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

static OT_JOB _set_panid(otInstance *ot_instance, void *data)
{
    uint16_t panid = *((uint16_t*) data);
    otLinkSetPanId(ot_instance, panid);
}

static OT_JOB _get_panid(otInstance *ot_instance, void *data)
{
    *((uint16_t*) data) = otLinkGetPanId(ot_instance);
    printf("PanID: %04x\n", *((uint16_t*) data));
}

static OT_JOB _thread_start(otInstance *ot_instance, void *data)
{
    printf("Starting OpenThread\n");
    otIp6SetEnabled(ot_instance, true);
    otThreadSetEnabled(ot_instance, true);
}

static OT_JOB _read_state(otInstance *ot_instance, void *data)
{
    uint8_t state = otThreadGetDeviceRole(ot_instance);
    printf("State is: %i\n", state);
}

static OT_JOB _get_ip_addresses(otInstance *ot_instance, void *data)
{
    for(const otNetifAddress *addr=otIp6GetUnicastAddresses(ot_instance); addr; addr=addr->mNext)
    {
        char addrstr[IPV6_ADDR_MAX_STR_LEN];
        printf("inet6 %s\n", ipv6_addr_to_str(addrstr, (ipv6_addr_t*) &addr->mAddress.mFields, sizeof(addrstr)));
    }
}

static OT_JOB _create_udp_socket(otInstance *ot_instance, void *data)
{
    otSockAddr sockaddr;
    memset(&sockaddr, 0, sizeof(otSockAddr));
    sockaddr.mPort = *((uint16_t*) data);

    otUdpOpen(ot_instance, &mSocket, _handle_receive, NULL);
    otUdpBind(&mSocket, &sockaddr);
}

static OT_JOB _send_udp_pkt(otInstance *ot_instance, void *data)
{
    udp_pkt_t *pkt = (udp_pkt_t*) data;
    otMessage *message;


    otUdpSocket socket;
    memset(&socket, 0, sizeof(otUdpSocket));

    otUdpOpen(ot_instance, &socket, _handle_receive, NULL);

    message = otUdpNewMessage(ot_instance, true);
    int error;
    error = otMessageSetLength(message, pkt->len);
    error = otMessageWrite(message, 0, pkt->payload, pkt->len);
    (void) error;

    otMessageInfo mPeer;
    
    //Set dest address
    memcpy(&mPeer.mPeerAddr.mFields, &(pkt->ip_addr), sizeof(ipv6_addr_t));

    //Set dest port
    mPeer.mPeerPort = pkt->port;

    otUdpSend(&socket, message, &mPeer);
    otUdpClose(&socket);
}

int sock_udp_create(sock_udp_t *sock, const sock_udp_ep_t *local,
                    const sock_udp_ep_t *remote, uint16_t flags)
{
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
        printf("Socket port %d \n", socket_port);
        ot_exec_job(_create_udp_socket, &(socket_port));
    }
    else
    {
        printf("Null local\n");
    }
    sock->flags = flags;

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
    udp_pkt_t pkt;
    uint16_t src_port = 0, dst_port;
    sock_ip_ep_t local;
    sock_ip_ep_t *rem;

    assert((sock != NULL) || (remote != NULL));
    assert((len == 0) || (data != NULL)); /* (len != 0) => (data != NULL) */

    if (remote != NULL) {
        if (remote->port == 0) {
            return -EINVAL;
        }
        else if (openthread_ep_addr_any((const sock_ip_ep_t *)remote)) {
            return -EINVAL;
        }
        else if (openthread_af_not_supported(remote->family)) {
            return -EAFNOSUPPORT;
        }
        else if ((sock != NULL) &&
                 (sock->local.netif != SOCK_ADDR_ANY_NETIF) &&
                 (remote->netif != SOCK_ADDR_ANY_NETIF) &&
                 (sock->local.netif != remote->netif)) {
            return -EINVAL;
        }
    }
    else if (sock->remote.family == AF_UNSPEC) {
        return -ENOTCONN;
    }
    /* compiler evaluates lazily so this isn't a redundundant check and cppcheck
     * is being weird here anyways */
    /* cppcheck-suppress nullPointerRedundantCheck */
    /* cppcheck-suppress nullPointer */
    if ((sock == NULL) || (sock->local.family == AF_UNSPEC)) {
        /* no sock or sock currently unbound */
        if (sock != NULL) {
            /* bind sock object implicitly */
            sock->local.port = src_port;
            if (remote == NULL) {
                sock->local.family = sock->remote.family;
            }
            else {
                sock->local.family = remote->family;
            }
            ot_exec_job(_create_udp_socket, &(src_port));
        }
    }
    else {
        src_port = sock->local.port;
        memcpy(&local, &sock->local, sizeof(local));
    }
    /* sock can't be NULL at this point */
    if (remote == NULL) {
        rem = (sock_ip_ep_t *)&sock->remote;
        dst_port = sock->remote.port;
    }
    else {
        rem = (sock_ip_ep_t *)remote;
        dst_port = remote->port;
    }
    /* check for matching address families in local and remote */
    if (local.family == AF_UNSPEC) {
        local.family = rem->family;
    }
    else if (local.family != rem->family) {
        return -EINVAL;
    }

    remote = (struct _sock_tl_ep *) remote;

    /* build packet and send */
    memcpy(&pkt.ip_addr, &remote->addr, sizeof(remote->addr));
    pkt.port = dst_port;
    pkt.payload = (void *) data;
    pkt.len = len;

    ot_exec_job(_send_udp_pkt, &pkt);

    return 0;
}

/** @} */
