/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup posix
 *
 * @{
 * @file
 * @brief  posix netdb implementation
 * @author Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#ifndef NETDB_H
#define NETDB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/bytes.h"

/** Errors used by the DNS API functions */
/**
 * @brief The name could not be resolved at this time. Future attempts may succeed.
 */
#define EAI_AGAIN       1
/**
 * @brief The flags parameter had an invalid value.
 */
#define EAI_BADFLAGS    2
/**
 * @brief A non-recoverable error occurred when attempting to resolve the name.
 */
#define EAI_FAIL        3
/**
 * @brief The address family was not recognized.
 */
#define EAI_FAMILY      4
/**
 * @brief There was a memory allocation failure when trying to allocate storage
 *        for the return value.
 */
#define EAI_MEMORY      5
/**
 * @brief The name does not resolve for the supplied parameters. Neither
 *        nodename nor servname were supplied. At least one of these need to
 *        be supplied.
 */
#define EAI_NONAME      6
/**
 * @brief The service passed was not recognized for the specified socket type.
 */
#define EAI_SERVICE     7
/**
 * @brief The intended socket type was not recognized.
 */
#define EAI_SOCKTYPE    8
/**
 * @brief A system error occurred.
 */
#define EAI_SYSTEM      9

/* Input flags for struct addrinfo */
/**
 * @brief Returned address information shall be suitable for use in binding a
 *        socket for accepting incoming connections for the specified service.
 */
#define AI_PASSIVE      0x01
/**
 * @brief Determine the canonical name corresponding to nodename.
 */
#define AI_CANONNAME    0x02
/**
 * @brief Supplied nodename string is a numeric host address string.
 */
#define AI_NUMERICHOST  0x04
/**
 * @brief Supplied servname string is a numeric port string.
 */
#define AI_NUMERICSERV  0x08
/**
 * @brief When specified along with ai_family = AF_INET6, getaddrinfo() returns
 *        IPv4-mapped IPv6 addresses on finding no matching IPv6 addresses.
 *
 * @note: Not supported yet
 * @todo: Implement
 */
#define AI_V4MAPPED     0x10
/**
 * @brief When AI_V4MAPPED is specified, then getaddrinfo() returns all matching
 *        IPv6 and IPv4 addresses. Without AI_V4MAPPED this flag will be ignored.
 *
 * @note: Not supported yet
 * @todo: Implement
 */
#define AI_ALL          0x20
/**
 * @brief IPv4 addresses are returned only if an IPv4 address is configured on
 *        the local system, and IPv6 addresses are returned only if an IPv6
 *        address is configured on the local system.
 *
 * @note: Not supported yet
 * @todo: Implement
 */
#define AI_ADDRCONFIG   0x40

/**
 * @brief Structure that contains information about the address of a service provider
 */
struct addrinfo {
    int               ai_flags;      /**< Input flags. */
    int               ai_family;     /**< Address family of socket. */
    int               ai_socktype;   /**< Socket type. */
    int               ai_protocol;   /**< Protocol of socket. */
    socklen_t         ai_addrlen;    /**< Length of socket address. */
    struct sockaddr  *ai_addr;       /**< Socket address of socket. */
    char             *ai_canonname;  /**< Canonical name of service location. */
    struct addrinfo  *ai_next;       /**< Pointer to next in list. */
};

/**
 *
 * @brief Free the addrinfo reference returned by getaddrinfo()
 *
 * Frees one or more addrinfo structures returned by getaddrinfo(), along with
 * any additional storage associated with those structures. If the ai_next field
 * of the structure is not null, the entire list of structures is freed.
 *
 * @param ai struct addrinfo to free
 */
void freeaddrinfo(struct addrinfo *ai);

/**
 *
 * @brief Translates the name of a service location and/or a service name and
 *        returns a set of socket addresses.
 *
 * @note  Due to implementation limitations only a single entry is returned.
 *
 * Translates the name of a service location (for example, a host name) and/or
 * a service name and returns a set of socket addresses and associated
 * information to be used in creating a socket with which to address the
 * specified service.
 * Memory for the result is allocated internally and must be freed by calling
 * freeaddrinfo()!
 *
 * @param nodename      descriptive name or address string of the host (may be
 *                      NULL -> local address)
 * @param servname      port number as string of NULL
 * @param hints         structure containing input values that set socktype and
 *                      protocol
 * @param res           pointer to a pointer where to store the result (is set
 *                      to NULL in case of failure)
 *
 * @return 0            On success, non-zero on failure
 * @retval EAI_AGAIN    Recoverable error, try again later
 * @retval EAI_BADFLAGS Invalid value in flag parameters (hints->ai_flags)
 * @retval EAI_FAIL     Non-recoverable error
 * @retval EAI_FAMILY   Unsupported address family
 * @retval EAI_MEMORY   No memory available to receive data or to allocate result object
 * @retval EAI_NONAME   Name does not resolve
 * @retval EAI_SERVICE  Service does not resolve
 */
int getaddrinfo(const char *nodename, const char *servname,
                const struct addrinfo *hints, struct addrinfo **res);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif /* NETDB_H */
