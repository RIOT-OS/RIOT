/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    Network interfaces
 * @ingroup     net
 * @brief       Common network interface API
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef NETIF_H_
#define NETIF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Identifier for an invalid network interface
 */
#define NETIF_INVALID       (0x0-1)

#ifndef NETIF_NAMELENMAX
#define NETIF_NAMELENMAX    (8U)
#endif

/**
 * @brief   Network interface enumeration type
 */
typedef intptr_t netif_t;

/**
 * @brief   Iterator for the interfaces
 *
 * Returns interface after @p last. To start use `last == NETIF_INVALID`.
 *
 * @return next network interface.
 * @return @ref NETIF_INVALID, if there is no interface after @p last
 */
netif_t netif_iter(netif_t last);

/**
 * @brief   Gets name of an interface
 *
 * @pre `name != NULL`
 * @pre name holds at least @ref NETIF_NAMELENMAX characters
 *
 * @param[in] netif A network interface.
 * @param[out] name The name of the interface. Must not be `NULL`. Must at least
 *                  hold @ref NETIF_NAMELENMAX bytes.
 *
 * @return  length of @p name on success
 * @return  0, if @p netif was not a valid interface.
 */
int netif_get_name(netif_t netif, char *name);

/**
 * @brief   Gets interface by name
 *
 * @pre `name != NULL`
 *
 * @param[in] name  The name of an interface. Must not be `NULL`.
 *
 * @return  The identifier of the interface on success.
 * @return  @ref NETIF_INVALID if no interface is named @p name.
 */
netif_t netif_get_by_name(const char *name);

#ifdef __cplusplus
}
#endif

#endif /* NETIF_H_ */
/** @} */
