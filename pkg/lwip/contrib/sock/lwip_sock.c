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
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include "lwip/sock_internal.h"

#include "net/af.h"
#include "net/ipv4/addr.h"
#include "net/ipv6/addr.h"
#include "net/sock.h"
#include "timex.h"

#include "lwip/err.h"
#include "lwip/ip.h"
#include "lwip/netif.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "lwip/opt.h"

#include "sock_types.h"

#if !LWIP_IPV4 && !LWIP_IPV6
#error "lwip_sock needs IPv4 or IPv6 support"
#endif

/**
 * @brief   Checks if an address family is *not* supported by the lwIP
 *          implementation
 *
 * @param[in] af    An address family
 *
 * @return  true, if @p af is *not* supported.
 * @return  false, if @p af is supported.
 */
static inline bool _af_not_supported(int af)
{
    switch (af) {
#if LWIP_IPV4
        case AF_INET:
            return false;
#endif
#if LWIP_IPV6
        case AF_INET6:
            return false;
#endif
        default:
            return true;
    }
}

#if LWIP_IPV4 && LWIP_IPV6
static inline u8_t lwip_af_to_ip_addr_type(int af)
{
    switch (af) {
        case AF_INET:
            return IPADDR_TYPE_V4;
        case AF_INET6:
        case AF_UNSPEC: /* in case of any address */
            return IPADDR_TYPE_V6;
        default:
            return 0xff;
    }
}
#endif

static bool _ep_isany(const struct _sock_tl_ep *ep)
{
    uint8_t *ep_addr;

    if (ep == NULL) {
        return true;
    }
    ep_addr = (uint8_t *)&ep->addr;
    for (unsigned i = 0; i < sizeof(ep->addr); i++) {
#if LWIP_IPV4
        /* stop checking IPv6 for IPv4 addresses */
        if ((ep->family == AF_INET) && i >= sizeof(ep->addr.ipv4)) {
            break;
        }
#endif
        if (ep_addr[i] != 0) {
            return false;
        }
    }
    return true;
}

static const ip_addr_t *_netif_to_bind_addr(int family, uint16_t netif_num)
{
    struct netif *netif;
    if (netif_num > UINT8_MAX) {
        return NULL;
    }
    /* cppcheck-suppress uninitvar ; assigned by macro */
    NETIF_FOREACH(netif) {
        if (netif->num == (netif_num - 1)) {
            switch (family) {
#if LWIP_IPV4
                case AF_INET:
                    return &netif->ip_addr;
#endif
#if LWIP_IPV6
                case AF_INET6:
                    /* link-local address is always the 0th */
                    return &netif->ip6_addr[0];
#endif
                default:
                    return NULL;
            }
        }
    }
    return NULL;
}

static bool _addr_on_netif(int family, int netif_num, const ip_addr_t *addr)
{
    struct netif *netif;
    assert(addr != NULL);
    assert((netif_num >= 0) && (netif_num <= UINT8_MAX));
    /* cppcheck-suppress uninitvar ; assigned by macro */
    NETIF_FOREACH(netif) {
        if (netif->num == (netif_num - 1)) {
            switch (family) {
#if LWIP_IPV4
                case AF_INET:
                    return ip_2_ip4(&netif->ip_addr)->addr == ip_2_ip4(addr)->addr;
#endif
#if LWIP_IPV6
                case AF_INET6: {
                    LOCK_TCPIP_CORE();
                    /* link-local address is always the 0th */
                    s8_t match = netif_get_ip6_addr_match(netif, ip_2_ip6(addr));
                    UNLOCK_TCPIP_CORE();
                    return match >= 0;
                }
#endif
                default:
                    return false;
            }
        }
    }
    return false;
}

#ifdef MODULE_LWIP_SOCK_IP
#define _set_port(p, ep, type)   \
    if (!((type) & NETCONN_RAW)) { \
        p = (ep)->port; \
    }
#else
#define _set_port(p, ep, type)   \
    p = (ep)->port;
#endif

static void _convert_ip_addr(ip_addr_t *lwip_addr, int family,
                             const void *sock_addr, size_t sock_addr_size)
{
    memcpy(lwip_addr, sock_addr, sock_addr_size);
#if LWIP_IPV6 && LWIP_IPV4
    if (family == AF_INET6) {
        ip6_addr_clear_zone(&lwip_addr->u_addr.ip6);
    }
    lwip_addr->type = lwip_af_to_ip_addr_type(family);
#elif LWIP_IPV6
    (void)family;
    ip6_addr_clear_zone(lwip_addr);
#else
    (void)family;
#endif
}

static int _sock_ep_to_netconn_pars(const struct _sock_tl_ep *local,
                                    const struct _sock_tl_ep *remote,
                                    ip_addr_t *local_addr, u16_t *local_port,
                                    ip_addr_t *remote_addr, u16_t *remote_port,
                                    int *type)
{
    bool res = 0;
    int family = AF_UNSPEC;
    uint16_t netif = SOCK_ADDR_ANY_NETIF;

    if ((local != NULL) && (remote != NULL) &&
        (remote->netif != SOCK_ADDR_ANY_NETIF) &&
        (local->netif != SOCK_ADDR_ANY_NETIF) &&
        (remote->netif != local->netif)) {
        return -EINVAL;
    }

#if LWIP_IPV6
    *type &= ~NETCONN_TYPE_IPV6;
#else
    (void)type; /* is read but not set => compiler complains */
#endif
    if (local != NULL) {
        if (_af_not_supported(local->family)) {
            return -EAFNOSUPPORT;
        }
        if (local->netif != SOCK_ADDR_ANY_NETIF) {
            netif = local->netif;
        }
        family = local->family;
        _set_port(*local_port, local, *type);
    }
    if (remote != NULL) {
        if (_af_not_supported(remote->family) ||
            ((local != NULL) && (local->family != remote->family))) {
            return -EAFNOSUPPORT;
        }
        if ((remote->netif != SOCK_ADDR_ANY_NETIF) &&
            (local != NULL) && (local->netif != SOCK_ADDR_ANY_NETIF)) {
            netif = remote->netif;
        }
        family = remote->family;
        _convert_ip_addr(remote_addr, family, &remote->addr,
                         sizeof(remote->addr));
        if (ip_addr_isany(remote_addr)) {
            return -EINVAL;
        }
        _set_port(*remote_port, remote, *type);
    }

#if LWIP_IPV6
    if (family == AF_INET6) {
        *type |= NETCONN_TYPE_IPV6;
    }
#endif

    if (netif != SOCK_ADDR_ANY_NETIF) {
        if (_ep_isany(local)) {
            const ip_addr_t *tmp = _netif_to_bind_addr(family, netif);
            if (tmp != NULL) {
                memcpy(local_addr, tmp, sizeof(ip_addr_t));
                res = 1;
            }
            else {
                /* netif was not a valid interface */
                return -EINVAL;
            }
        }
        /* case (local == NULL) is included in _ep_isany() */
        /* cast to ip_addr_t alright, since type field is never used */
        else if (_addr_on_netif(family, netif, (ip_addr_t *)&local->addr)) {
            _convert_ip_addr(local_addr, family, &local->addr,
                             sizeof(local->addr));
            res = 1;
        }
        else {
            return -EINVAL;
        }
    }
    else if (local != NULL) {
        _convert_ip_addr(local_addr, family, &local->addr, sizeof(local->addr));
        res = 1;
    }

    return res;
}

static void _netconn_cb(struct netconn *conn, enum netconn_evt evt,
                        u16_t len)
{
#if IS_ACTIVE(SOCK_HAS_ASYNC)
    lwip_sock_base_t *sock = netconn_get_callback_arg(conn);
    if (sock && conn->pcb.raw &&
        /* lwIP's TCP implementation initializes callback_arg.socket with -1
         * when not provided */
        (conn->callback_arg.socket != -1)) {
        sock_async_flags_t flags = 0;

        (void)len;
        switch (evt) {
            case NETCONN_EVT_RCVPLUS:
                if (LWIP_TCP && (conn->type & NETCONN_TCP)) {
#if LWIP_TCP    /* additional guard needed due to dependent member access */
                    switch (conn->pcb.tcp->state) {
                        case CLOSED:
                        case CLOSE_WAIT:
                        case CLOSING:
                            flags |= SOCK_ASYNC_CONN_FIN;
                            break;
                        default:
                            break;
                    }
                    if (mbox_avail(&conn->acceptmbox.mbox)) {
                        flags |= SOCK_ASYNC_CONN_RECV;
                    }
                    if (mbox_avail(&conn->recvmbox.mbox)) {
                        flags |= SOCK_ASYNC_MSG_RECV;
                    }
#endif
                }
                else {
                    flags |= SOCK_ASYNC_MSG_RECV;
                }
                break;
            case NETCONN_EVT_SENDPLUS:
                flags |= SOCK_ASYNC_MSG_SENT;
                break;
            case NETCONN_EVT_ERROR:
                if (LWIP_TCP && (conn->type & NETCONN_TCP)) {
                    /* try to report this */
                    flags |= SOCK_ASYNC_CONN_FIN;
                }
                break;
            case NETCONN_EVT_RCVMINUS:
            case NETCONN_EVT_SENDMINUS:
                break;
            default:
                LWIP_ASSERT("unknown event", 0);
                break;
        }
        if (flags && sock->async_cb.gen) {
            sock->async_cb.gen(sock, flags, sock->async_cb_arg);
        }
    }
#else
    (void)conn;
    (void)evt;
    (void)len;
#endif
}

static int _create(int type, int proto, uint16_t flags, struct netconn **out)
{
    if ((*out = netconn_new_with_proto_and_callback(
            type, proto,
            IS_ACTIVE(SOCK_HAS_ASYNC) ? _netconn_cb : NULL)) == NULL) {
        return -ENOMEM;
    }
    netconn_set_callback_arg(*out, NULL);
#if SO_REUSE
    if (flags & SOCK_FLAGS_REUSE_EP) {
        ip_set_option((*out)->pcb.ip, SOF_REUSEADDR);
    }
#else
    (void)flags;
#endif
    return 0;
}

#include <assert.h>
#include <stdio.h>

int lwip_sock_create(struct netconn **conn, const struct _sock_tl_ep *local,
                     const struct _sock_tl_ep *remote, int proto,
                     uint16_t flags, int type)
{
    assert(conn != NULL);
#if LWIP_IPV6
    assert(!(type & NETCONN_TYPE_IPV6));
#endif

    ip_addr_t local_addr, remote_addr;
    u16_t local_port = 0, remote_port = 0;   /* 0 is used by lwIP to
                                              * automatically generate
                                              * port */
    /* convert parameters */
    int bind = _sock_ep_to_netconn_pars(local, remote, &local_addr, &local_port,
                                        &remote_addr, &remote_port, &type);

    /* error occurred during parameter conversion */
    if (bind < 0) {
        return bind;
    }
    if ((remote != NULL) && ip_addr_isany_val(remote_addr)) {
        return -EINVAL;
    }
    /* if local or remote parameters are given */
    else if ((local != NULL) || (remote != NULL)) {
        int res = 0;
        if ((res = _create(type, proto, flags, conn)) < 0) {
            return res;
        }
        /* if parameters (local->netif, remote->netif, local->addr or
         * local->port) demand binding */
        if (bind) {
            switch (netconn_bind(*conn, &local_addr, local_port)) {
#if LWIP_TCP
                case ERR_BUF:
                    res = -ENOMEM;
                    break;
#endif
                case ERR_USE:
                    res = -EADDRINUSE;
                    break;
                case ERR_VAL:
                    res = -EINVAL;
                    break;
                default:
                    break;
            }
            if (res < 0) {
                netconn_delete(*conn);
                return res;
            }
        }
        if (remote != NULL) {
            switch (netconn_connect(*conn, &remote_addr, remote_port)) {
#if LWIP_TCP
                case ERR_BUF:
                    res = -ENOMEM;
                    break;
                case ERR_INPROGRESS:
                    res = -EINPROGRESS;
                    break;
                case ERR_ISCONN:
                    res = -EISCONN;
                    break;
                case ERR_IF:
                case ERR_RTE:
                    res = -ENETUNREACH;
                    break;
                case ERR_ABRT:
                    res = -ETIMEDOUT;
                    break;
#endif
                case ERR_USE:
                    res = -EADDRINUSE;
                    break;
                case ERR_VAL:
                    res = -EINVAL;
                    break;
                default:
                    break;
            }
            if (res < 0) {
                netconn_delete(*conn);
                return res;
            }
        }
    }
    return 0;
}

uint16_t lwip_sock_bind_addr_to_netif(const ip_addr_t *bind_addr)
{
    assert(bind_addr != NULL);

    if (!ip_addr_isany(bind_addr)) {
        struct netif *netif;
        LOCK_TCPIP_CORE();
        /* cppcheck-suppress uninitvar ; assigned by macro */
        NETIF_FOREACH(netif) {
            if (IP_IS_V6(bind_addr)) {  /* XXX crappy API yields crappy code */
#if LWIP_IPV6
                if (netif_get_ip6_addr_match(netif, ip_2_ip6(bind_addr)) >= 0) {
                    UNLOCK_TCPIP_CORE();
                    return (int)netif->num + 1;
                }
#endif
            }
            else {
#if LWIP_IPV4
                if (netif_ip4_addr(netif)->addr == ip_2_ip4(bind_addr)->addr) {
                    UNLOCK_TCPIP_CORE();
                    return (int)netif->num + 1;
                }
#endif
            }
        }
        UNLOCK_TCPIP_CORE();
    }
    return SOCK_ADDR_ANY_NETIF;
}

int lwip_sock_get_addr(struct netconn *conn, struct _sock_tl_ep *ep, u8_t local)
{
    ip_addr_t addr;
    int res;
#ifdef MODULE_LWIP_SOCK_IP
    u16_t port = UINT16_MAX;
    u16_t *port_ptr = &port;
    /* addr needs to be NULL because netconn_getaddr returns error on connected
     * conns as "as connecting is only a helper for upper layers [sic]" */
    memset(&addr, 0, sizeof(addr));
#else
    u16_t *port_ptr = &ep->port;
#endif

    assert(ep != NULL);
    if (conn == NULL) {
        return 1;
    }
#ifdef MODULE_LWIP_SOCK_IP
    if (!(conn->type & NETCONN_RAW)) {
        port_ptr = &ep->port;
    }
#endif
    if ((res = netconn_getaddr(conn, &addr, port_ptr, local)) != ERR_OK
#ifdef MODULE_LWIP_SOCK_IP
        /* XXX lwIP's API is very inconsistent here so we need to check if addr
         * was changed */
            && !local && ip_addr_isany_val(addr)
#endif
        ) {
        return res;
    }
    if (NETCONNTYPE_ISIPV6(conn->type)) {
        ep->family = AF_INET6;
    }
    else if (IS_ACTIVE(LWIP_IPV4)) {
        ep->family = AF_INET;
    }
    else {
        ep->family = AF_UNSPEC;
    }
    if (local) {
        ep->netif = lwip_sock_bind_addr_to_netif(&addr);
    }
    else {
        ep->netif = SOCK_ADDR_ANY_NETIF;
    }
    memcpy(&ep->addr, &addr, sizeof(ep->addr));
    return 0;
}

#if defined(MODULE_LWIP_SOCK_UDP) || defined(MODULE_LWIP_SOCK_IP)
int lwip_sock_recv(struct netconn *conn, uint32_t timeout, struct netbuf **buf)
{
    int res;

    if (conn == NULL) {
        return -EADDRNOTAVAIL;
    }
#if LWIP_SO_RCVTIMEO
    if ((timeout != 0) && (timeout != SOCK_NO_TIMEOUT)) {
        netconn_set_recvtimeout(conn, timeout / US_PER_MS);
    }
    else
#endif
    if ((timeout == 0) && !mbox_avail(&conn->recvmbox.mbox)) {
        return -EAGAIN;
    }
    switch (netconn_recv(conn, buf)) {
        case ERR_OK:
            res = 0;
            break;
#if LWIP_SO_RCVTIMEO
        case ERR_TIMEOUT:
            res = -ETIMEDOUT;
            break;
#endif
        case ERR_MEM:
            res = -ENOMEM;
            break;
        default:
            res = -EPROTO;
            break;
    }
    /* unset flags */
#if LWIP_SO_RCVTIMEO
    netconn_set_recvtimeout(conn, 0);
#endif
#if IS_ACTIVE(SOCK_HAS_ASYNC)
    lwip_sock_base_t *sock = netconn_get_callback_arg(conn);

    if (sock && sock->async_cb.gen && mbox_avail(&conn->recvmbox.mbox)) {
        sock->async_cb.gen(sock, SOCK_ASYNC_MSG_RECV, sock->async_cb_arg);
    }
#endif
    return res;
}
#endif /* defined(MODULE_LWIP_SOCK_UDP) || defined(MODULE_LWIP_SOCK_IP) */

ssize_t lwip_sock_send(struct netconn *conn, const void *data, size_t len,
                       int proto, const struct _sock_tl_ep *remote, int type)
{
    ip_addr_t remote_addr;
    struct netconn *tmp;
    struct netbuf *buf;
    int res;
    err_t err;
    u16_t remote_port = 0;

#if LWIP_IPV6
    assert(!(type & NETCONN_TYPE_IPV6));
#endif
    if (remote != NULL) {
        if ((res = _sock_ep_to_netconn_pars(NULL, remote, NULL, NULL,
                                            &remote_addr, &remote_port,
                                            &type)) < 0) {
            return res;
        }
        if (ip_addr_isany_val(remote_addr)) {
            return -EINVAL;
        }
    }

    buf = netbuf_new();
    if ((buf == NULL) || (netbuf_alloc(buf, len) == NULL) ||
        (netbuf_take(buf, data, len) != ERR_OK)) {
        netbuf_delete(buf);
        return -ENOMEM;
    }
    if ((conn == NULL) && (remote != NULL)) {
        if ((res = _create(type, proto, 0, &tmp)) < 0) {
            netbuf_delete(buf);
            return res;
        }
    }
    else if (conn != NULL) {
        ip_addr_t addr;
        u16_t port;

        if (((remote != NULL) &&
             (remote->netif != SOCK_ADDR_ANY_NETIF) &&
             (netconn_getaddr(conn, &addr, &port, 1) == 0) &&
             (remote->netif != lwip_sock_bind_addr_to_netif(&addr)))) {
            return -EINVAL;
        }
        tmp = conn;
    }
    else {
        netbuf_delete(buf);
        return -ENOTCONN;
    }
    res = len;  /* set for non-TCP calls */
    if (remote != NULL) {
        err = netconn_sendto(tmp, buf, &remote_addr, remote_port);
    }
#if LWIP_TCP
    else if (tmp->type & NETCONN_TCP) {
        err = netconn_write_partly(tmp, data, len, 0, (size_t *)(&res));
    }
#endif /* LWIP_TCP */
    else {
        err = netconn_send(tmp, buf);
    }
    switch (err) {
        case ERR_OK:
            break;
        case ERR_BUF:
        case ERR_MEM:
            res = -ENOMEM;
            break;
        case ERR_RTE:
        case ERR_IF:
            res = -EHOSTUNREACH;
            break;
        case ERR_VAL:
        default:
            res = -EINVAL;
            break;
    }
    netbuf_delete(buf);
    if (conn == NULL) {
        netconn_delete(tmp);
    }
    return res;
}

/** @} */
