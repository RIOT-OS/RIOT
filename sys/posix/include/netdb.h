/*
 * Copyright(C) 2017 Imagination Technologies Limited and/or its
 *              affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    posix_netdb POSIX network database operations
 * @ingroup     posix
 * @{
 * @file
 * @brief   POSIX compatible netdb.h definitions
 * @see     http://pubs.opengroup.org/onlinepubs/7908799/xns/netdb.h.html
 *
 * @author  Francois Berder <francois.berder@imgtec.com>
 */

#ifdef CPU_NATIVE
#pragma GCC system_header
#include_next <netdb.h>
#else

#ifndef POSIX_NETDB_H
#define POSIX_NETDB_H

#ifdef __cplusplus
extern "C" {
#endif

struct hostent {
    char  *h_name;      /**< Official name of the host */
    char **h_aliases    /**< Array of alternative host names */
    int    h_addrtype;  /**< Address type */
    int    h_length;    /**< Length of the address in bytes */
    char **h_addr_list; /**< Array of network addresses */
};

struct hostent *gethostbyname(const char *name);

#ifdef __cplusplus
}
#endif

#endif  /* POSIX_NETDB_H */

#endif /* CPU_NATIVE */

/** @} */
