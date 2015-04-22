/**
 * Copyright (C) 2015 Simon Brummer <brummer.simon@googlemail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup ng_socket.c
 * @{
 * @file
 * @brief Implementation of RIOTs ng_socket API.
 * @author Simon Brummer <brummer.simon@googlemail.com>
 */

#include <string.h>
#include "net/ng_socket.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_NG_UDP
#include "net/ng_udp.h"
#endif

#ifdef MODULE_NG_TCP
#include "net/ng_tcp.h"
#endif

/* Internal Socket Structures */
ng_socket_t _sockets[MAXSOCKETS];

/* Transport Layer Stubs. These Functions need to be implemented by Transport Layers */
/* TCP Stubs */
int __attribute__((weak)) ng_tcp_connect(ng_socket_t *socket, const ng_sockaddr6_t *address,
                                         ng_socklen_t address_len)
{
    (void)socket;
    (void)address;
    (void)address_len;
    DEBUG("ng_tcp_connect(): Error. Function not implemented");
    return -3;
}

int __attribute__((weak)) ng_tcp_listen(ng_socket_t *socket, int backlog)
{
    (void)socket;
    (void)backlog;
    DEBUG("ng_tcp_listen(): Error. Function not implemented");
    return -3;
}

int __attribute__((weak)) ng_tcp_accept(ng_socket_t *socket, const ng_sockaddr6_t *address,
                                        ng_socklen_t *address_len)
{
    (void) socket;
    (void) address;
    (void) address_len;
    DEBUG("ng_tcp_accept(): Error. Function not implemented");
    return -3;
}

ssize_t __attribute__((weak)) ng_tcp_recvfrom(ng_socket_t *socket, void *buf, size_t len, int flags,
                                              ng_sockaddr6_t *from,  ng_socklen_t *fromlen)
{
    (void) socket;
    (void) buf;
    (void) len;
    (void) flags;
    (void) from;
    (void) fromlen;
    DEBUG("ng_tcp_recvfrom(): Error. Function not implemented");
    return -3;
}

ssize_t __attribute__((weak)) ng_tcp_sendto(ng_socket_t *socket, const void *buf, size_t len, int flags,
                                            const ng_sockaddr6_t *to, ng_socklen_t tolen)
{
    (void) socket;
    (void) buf;
    (void) len;
    (void) flags;
    (void) to;
    (void) tolen;
    DEBUG("ng_tcp_sendto(): Error. Function not implemented");
    return -3;
}

int __attribute__((weak)) ng_tcp_shutdown(ng_socket_t *socket, int how)
{
    (void) socket;
    (void) how;
    DEBUG("ng_tcp_shutdown(): Error. Function not implemented");
    return -3;
}

/* UDP Stubs */
ssize_t __attribute__((weak)) ng_udp_recvfrom(ng_socket_t *socket, void *buf, size_t len, int flags,
                                              ng_sockaddr6_t *from,  ng_socklen_t *fromlen)
{
    (void) socket;
    (void) buf;
    (void) len;
    (void) flags;
    (void) from;
    (void) fromlen;
    DEBUG("ng_udp_recvfrom(): Error. Function not implemented");
    return -3;
}

ssize_t __attribute__((weak)) ng_udp_sendto(ng_socket_t *socket, const void *buf, size_t len, int flags,
                                            const ng_sockaddr6_t *to,  ng_socklen_t tolen)
{
    (void) socket;
    (void) buf;
    (void) len;
    (void) flags;
    (void) to;
    (void) tolen;
    DEBUG("ng_udp_sendto(): Error. Function not implemented");
    return -3;
}

/* Internal Functions */
static inline bool _socket_exists(int socket)
{
    return (0 <= socket && socket < MAXSOCKETS) ? true : false;
}

static inline void _clear_socket(int idx)
{
    /* Clear Data Structure */
    memset( &(_sockets[idx]), 0, sizeof(ng_socket_t) );

    /* Fill structure with sane defaults */
    ng_ipv6_addr_set_unspecified(&(_sockets[idx].local_addr.sin6_addr));
    ng_ipv6_addr_set_unspecified(&(_sockets[idx].peer_addr.sin6_addr));
    _sockets[idx].in_use = false;
}

static inline bool _is_socket_tcp(int idx)
{
    return _sockets[idx].type == SOCK_STREAM ? true : false;
}

static inline bool _is_socket_udp(int idx)
{
    return _sockets[idx].type == SOCK_DGRAM ? true : false;
}

/* Implmentation API */
void ng_socket_init(void){
    for(int i = 0; i < MAXSOCKETS; i++){
        _clear_socket(i);
    }
}

int ng_socket_socket(int domain, int type, int protocol)
{
    /* Search for next unused socket */
    int idx = 0;
    for(idx=0; idx<MAXSOCKETS; idx++){
        if( _sockets[idx].in_use == false ){
            break;
        }
    }

    if(idx == MAXSOCKETS){
        DEBUG("ng_socket_socket(): Error. All Sockets are in Use\n");
        return -1;
    }

    /* Unused Socket found at Index idx */
    ng_socket_t *sock = &(_sockets[idx]);

    /* Perform sanity checks */
    switch(domain){
        case AF_INET6:   sock->domain = AF_INET6;
                         break;

        default:         DEBUG("ng_socket_socket(): Error. Unknown Domain\n");
                         return -2;
    }

    switch(type){
        case SOCK_DGRAM:   sock->type = SOCK_DGRAM;
                           break;

        case SOCK_STREAM:  sock->type = SOCK_STREAM;
                           break;

        default:           DEBUG("ng_socket_socket(): Error. Unknown Socket Type\n");
                           _clear_socket(idx);
                           return -3;
    }

    switch(protocol){
        case IPPROTO_IP:  sock->protocol = IPPROTO_IP;
                          break;

        case IPPROTO_TCP: sock->protocol = IPPROTO_TCP;
                          break;

        case IPPROTO_UDP: sock->protocol = IPPROTO_UDP;
                          break;

        default:          DEBUG("ng_socket_socket(): Error. Unknown Protocol Type\n");
                          _clear_socket(idx);
                          return -4;
    }

    /* Additional Checks */
    if(sock->domain == AF_INET6){
        if(sock->type == SOCK_DGRAM){
            sock->protocol = IPPROTO_UDP;
        }
        if(sock->type == SOCK_STREAM){
            sock->protocol = IPPROTO_TCP;
        }
    }

    /* Everything is okay. Use this Socket */
    sock->in_use = true;
    return idx;
}

int ng_socket_bind(int socket, const ng_sockaddr6_t *address, ng_socklen_t address_len)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_bind(): Error. Socket doesn't exist\n");
        return -1;
    }

    ng_socket_t *sock = &(_sockets[socket]);

    if(!ng_ipv6_addr_is_unspecified(&(sock->local_addr.sin6_addr))) {
        DEBUG("ng_socket_bind(): Error. Socket is already bound to an address\n");
        return -2;
    }

    if(sock->domain != address->sin6_family){
        DEBUG("ng_socket_bind(): Error. Address is not valid for Socket Domain\n");
        return -3;
    }

    for(int i=0; i<MAXSOCKETS; i++){
        if(sock->type == _sockets[i].type
        && ng_ipv6_addr_equal(&(address->sin6_addr), &(_sockets[i].local_addr.sin6_addr))
        && address->sin6_port == _sockets[i].local_addr.sin6_port)
        {
            DEBUG("ng_socket_bind(): Error. Address in use for this Socket type.\n");
            return -4;
        }
    }

    /* Everything is okay. Bind address to this socket */
    memcpy(&(sock->local_addr), address, address_len);
    return 0;
}

int ng_socket_connect(int socket, ng_sockaddr6_t *address, ng_socklen_t address_len)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_connect: Error. Socket doesn't exist\n");
        return -1;
    }
    if(!_is_socket_tcp(socket)){
        DEBUG("ng_socket_connect: Error. Socket is not of type tcp\n");
        return -2;
    }

    /* Everything is okay. Connect to given address */
    return ng_tcp_connect(&(_sockets[socket]), address, address_len);
}

int ng_socket_listen(int socket, int backlog)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_listen: Error. Socket doesn't exist\n");
        return -1;
    }

    if(!_is_socket_tcp(socket)){
        DEBUG("ng_socket_listen: Error. Socket is not of type tcp\n");
        return -2;
    }

    /* Everything is okay. Listen on socket */
    return ng_tcp_listen(&(_sockets[socket]), backlog);
}

int ng_socket_accept(int socket, ng_sockaddr6_t *address, ng_socklen_t *address_len)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_accept: Error. Socket doesn't exist\n");
        return -1;
    }

    if(!_is_socket_tcp(socket)){
        DEBUG("ng_socket_accept: Error. Socket is not of type tcp\n");
        return -2;
    }

    /* Everything is okay. Accept waiting connection */
    return ng_tcp_accept(&(_sockets[socket]), address, address_len);
}

ssize_t ng_socket_recv(int socket, void *buf, size_t len, int flags)
{
    return ng_socket_recvfrom(socket, buf, len, flags, NULL, NULL);
}

ssize_t ng_socket_recvfrom(int socket, void *buf, size_t len, int flags,
                           ng_sockaddr6_t *from, ng_socklen_t *fromlen)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_recvfrom(): Error. Socket doesn't exist\n");
        return -1;
    }

    if(_is_socket_tcp(socket)){
        return ng_tcp_recvfrom(&(_sockets[socket]), buf, len, flags, from, fromlen);
    }

    if(_is_socket_udp(socket)){
        return ng_udp_recvfrom(&(_sockets[socket]), buf, len, flags, from, fromlen);
    }

    DEBUG("ng_socket_recvfrom(): Error. Unsupported Protocol\n");
    return -2;
}

ssize_t ng_socket_send(int socket, const void *buf, size_t len, int flags)
{
    return ng_socket_sendto(socket, buf, len, flags, NULL, 0);
}

ssize_t ng_socket_sendto(int socket, const void *buf, size_t len, int flags,
                         const ng_sockaddr6_t *to, ng_socklen_t tolen)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_sendto(): Error. Socket doesn't exist\n");
        return -1;
    }

    if(_is_socket_tcp(socket)){
        return ng_tcp_sendto(&(_sockets[socket]), buf, len, flags, to, tolen);
    }

    if(_is_socket_udp(socket)){
        return ng_udp_sendto(&(_sockets[socket]), buf, len, flags, to, tolen);
    }

    DEBUG("ng_socket_sendto(): Error. Unsupported Protocol\n");
    return -2;
}

int ng_socket_shutdown(int socket, int how)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_shutdown(): Error. Socket doesn't exist\n");
        return -1;
    }

    if(how != SHUTDOWN_RD && how != SHUTDOWN_WR && how != SHUTDOWN_RDWR){
        DEBUG("ng_socket_shutdown(): Error. \"how\" is unknown.\n");
        return -2;
    }

    /* Shut down tcp connection */
    if(_is_socket_tcp(socket)){
        return ng_tcp_shutdown(&(_sockets[socket]), how);
    }

    return 0;
}

int ng_socket_close(int socket)
{
    /* Perform sanity checks */
    if(!_socket_exists(socket)){
        DEBUG("ng_socket_close(): Error. Socket doesn't exist\n");
        return -1;
    }

    /* Shutdown connection gracefully */
    ng_socket_shutdown(socket, SHUTDOWN_RDWR);

    /* Cleanup */
    _clear_socket(socket);
    return 0;
}

/**
 * @}
 */
