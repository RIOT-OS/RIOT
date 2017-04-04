/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup net_netif  Network interfaces
 * @ingroup  net
 * @brief   Common network interface API
 * @{
 *
 * @file
 * @brief   Network interface API definition
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NETIF_H_
#define NETIF_H_

#include "net/netdev2.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Special identifier for "any interface" e.g. for usage with socks
 *
 * @todo    Add reference to sock module when #5758 was merged.
 */
#define NETIF_ANY       (0)

/**
 * @brief   Maximum length of the optional interface names
 */
#define NETIF_NAME_LEN  (4U)

#ifndef NETIF_NAMED
#if defined(DEVELHELP) || defined(DOXYGEN)
/**
 * @brief   Switch to have named interfaces or not
 *
 * This macro is 1 by default if `DEVELHELP` is defined, otherwise it is
 * undefined.
 */
#define NETIF_NAMED     (1)
#endif
#endif

/**
 * @brief   Representation of an interface
 *
 * @note    Don't change any of these fields externally unless you are 100% sure
 *          what you are doing.
 */
typedef struct netif {
    struct netif *next; /**< pointer to next interface */
    netdev2_t *dev;     /**< pointer to handling network device */
    void *internal;     /**< pointer to stack-internal representation */
#if NETIF_NAMED
    /**
     * @brief   name of the interface
     *
     * @note    To use maximum space this will not be NULL terminated.
     */
    char name[NETIF_NAME_LEN];
#endif
} netif_t;

/**
 * @brief   The global list of interfaces
 */
extern netif_t *netifs;

/**
 * @brief   Add a new interface
 *
 * @pre `netif != NULL && netif->next == NULL` (@p netif is not already in
 *      @ref netifs).
 *
 * @param[in] netif An interface.
 *
 * @return  0 on success
 * @return  -EEXIST, if @ref NETIF_NAMED is set and netif_t::name of
 *          @p netif already exists in @ref netifs.
 */
int netif_add(netif_t *netif);

/**
 * @brief   Get ID (position in @ref netifs, with 1 being the first) of
 *          interface
 *
 * @param[in] netif An interface.
 *
 * @return  The ID of @p netif.
 * @return  -ENOENT, if @p netif is not a valid interface
 */
int netif_get_id(netif_t *netif);

/**
 * @brief   Get ID (position in @ref netifs, with 1 being the first) of
 *          interface
 *
 * @param[in] id    An ID for an interface. This parameter is of type `int`,
 *                  so it is easily usable with the result from
 *                  `netif_get_id()`.
 *
 * @return  The ID of @p netif.
 * @return  NULL, if there is no interface with ID @p id
 */
netif_t *netif_by_id(int id);

#if NETIF_NAMED || defined(DOXYGEN)
/**
 * @brief   Get interface by name
 *
 * @pre `name != NULL`
 *
 * @note    Only available when @ref NETIF_NAMED "NETIF_NAMED != 0"
 *
 * If @p name is longer than @ref NETIF_NAME_LEN characters, it will be
 * truncated.
 *
 * @param[in] name  A name for an interface.
 *
 * @return  The interface named @p name.
 * @return  NULL, if there is no interface named @p name
 */
netif_t *netif_by_name(char *name);

/**
 * @brief   Set a new name for an interface
 *
 * @pre `netif != NULL && name != NULL`
 *
 * @note    Only available when @ref NETIF_NAMED "NETIF_NAMED != 0"
 *
 * If @p name is longer than @ref NETIF_NAME_LEN characters, it will be
 * truncated.
 *
 * @param[in] netif The interface that should be named.
 * @param[in] name  A name for an interface.
 *
 * @return  0 on success
 * @return  -EEXIST, if there is already an interface != @p netif named this way
 * @return  -ENOENT, if @p netif is not a valid interface
 */
int netif_set_name(netif_t *netif, char *name);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NETIF_H_ */
/** @} */
