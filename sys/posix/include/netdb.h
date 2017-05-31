/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_posix
 * @{
 *
 * @file    netdb.h
 * @brief   netdb
 *
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/netdb.h.html">
 *              The Open Group Base Specifications Issue 7, <strings.h>
 *          </a>
 *
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 */
#ifndef STRINGS_H
#define STRINGS_H

#include <inttypes.h>

#define NI_MAXHOST      129
#define NI_MAXSERV      4

typedef uint32_t  socklen_t;    ///< POSIX compatible type for address length.

enum netdb_return_types {
    EAI_AGAIN,       //< The name could not be resolved at this time. Future attempts may succeed.
    EAI_BADFLAGS,    //< The flags had an invalid value.
    EAI_FAIL,        //< A non-recoverable error occurred.
    EAI_FAMILY,      //< The address family was not recognized or the address length was invalid for the specified family.
    EAI_MEMORY,      //< There was a memory allocation failure.
    EAI_NONAME,      //< The name does not resolve for the supplied parameters.
                     //< NI_NAMEREQD is set and the host's name cannot be located, or both nodename and servname were null.
    EAI_SERVICE,     //< The service passed was not recognized for the specified socket type.
    EAI_SOCKTYPE,    //< The intended socket type was not recognized.
    EAI_SYSTEM,      //< A system error occurred. The error code can be found in errno.
    EAI_OVERFLOW,    //< An argument buffer overflowed.
};

struct addrinfo {
    int               ai_flags;      //< Input flags.
    int               ai_family;     //< Address family of socket.
    int               ai_socktype;   //< Socket type.
    int               ai_protocol;   //< Protocol of socket.
    socklen_t         ai_addrlen;    //< Length of socket address.
    struct sockaddr  *ai_addr;       //< Socket address of socket.
    char             *ai_canonname;  //< Canonical name of service location.
    struct addrinfo  *ai_next;       //< Pointer to next in list.
};


#define AI_PASSIVE       1 << 0    //< Socket address is intended for bind().
#define AI_CANONNAME     1 << 1    //< Request for canonical name.
#define AI_NUMERICHOST   1 << 2    //< Return numeric host address as name.
#define AI_NUMERICSERV   1 << 3    //< Inhibit service name resolution.
#define AI_V4MAPPED      1 << 4    //< If no IPv6 addresses are found, query for IPv4 addresses and return them to the caller as IPv4-mapped IPv6 addresses.
#define AI_ALL           1 << 5    //< Query for both IPv4 and IPv6 addresses.
#define AI_ADDRCONFIG    1 << 6    //< Query for IPv4 addresses only when an IPv4 address is configured; query for IPv6 addresses only when an IPv6 address is configured.

#define NI_NOFQDN        1 << 0    //< Only the nodename portion of the FQDN is returned for local hosts.
#define NI_NUMERICHOST   1 << 1    //< The numeric form of the node's address is returned instead of its name.
#define NI_NAMEREQD      1 << 2    //< Return an error if the node's name cannot be located in the database.
#define NI_NUMERICSERV   1 << 3    //< The numeric form of the service address is returned instead of its name.
#define NI_NUMERICSCOPE  1 << 4    //< For IPv6 addresses, the numeric form of the scope identifier is returned instead of its name.
#define NI_DGRAM         1 << 5    //< Indicates that the service is a datagram service (SOCK_DGRAM).

void endhostent(void);
void endnetent(void);
void endprotoent(void);
void endservent(void);
void freeaddrinfo(struct addrinfo *);
const char *gai_strerror(int);
int getaddrinfo(const char *restrict, const char *restrict,
                const struct addrinfo *restrict,
                struct addrinfo **restrict);
struct hostent *gethostent(void);
int getnameinfo(const struct sockaddr *restrict, socklen_t,
                char *restrict, socklen_t, char *restrict,
                socklen_t, int);
struct netent *getnetbyaddr(uint32_t, int);
struct netent *getnetbyname(const char *);
struct netent *getnetent(void);
struct protoent *getprotobyname(const char *);
struct protoent *getprotobynumber(int);
struct protoent *getprotoent(void);
struct servent *getservbyname(const char *, const char *);
struct servent *getservbyport(int, const char *);
struct servent *getservent(void);
void sethostent(int);
void setnetent(int);
void setprotoent(int);
void setservent(int);

#endif /* STRINGS_H */
