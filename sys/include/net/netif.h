/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_netif   Network interfaces
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

#include "net/netdev.h"
#include "net/netopt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Identifier for an invalid network interface
 */
#define NETIF_INVALID       (0x0-1)

/**
 * @brief   Maximum length for an interface name
 */
#ifndef NETIF_NAMELENMAX
#define NETIF_NAMELENMAX    (8U)
#endif

/**
 * @brief   Type of the interface
 */
typedef enum {
    /**
     * @brief   Default GNRC network interface (without software MAC)
     */
    NETIF_TYPE_GNRC_DEFAULT,
    /**
     * @brief   lwIP network interface
     */
    NETIF_TYPE_LWIP,
    /**
     * @brief   emb6 interface
     */
    NETIF_TYPE_EMB6,
    /* extend list if needed */
} netif_type_t;

/**
 * @brief struct holding all parameters needed for interface initialization
 */
typedef struct {
    /**
     * @brief Network device parameters
     *
     * The network device parameters are used to associate a network device to
     * the network interface.
     */
    const void *dev_params;
    netif_type_t type;      /**< type of the interface */
    const char *name;       /**< name of the interface */
} netif_params_t;

#include "netif_params.h"

/**
 * @brief   Maximum number of network interfaces
 */
#ifndef NETIF_NUMOF
#define NETIF_NUMOF         (sizeof(netif_params) / sizeof(netif_params[0]))
#endif

/**
 * @brief   Network interface enumeration type
 */
typedef intptr_t netif_t;

/**
 * @brief   Gets a network interface parameter struct from an array of parameter
 *          structs by network device parameters
 *
 * @param[in] netif_params  Array of network interface parameters.
 * @param[in] dev_params    A struct of network device parameters.
 *
 * @return  The parameters with netif_params_t::dev_params == @p dev_params
 * @return  NULL, if there are no such parameters in @p netif_params
 */
const netif_params_t *netif_params_get_by_dev(const netif_params_t *netif_params,
                                              const void *dev_params);

/**
 * @brief   Set-up interface
 *
 * @param[in] params    Initialization parameters of the interface
 * @param[in] subtype   netif_params_t::type specific sub-type based on the nature
 *                      of @p netdev (e.g. specifies that the device is an
 *                      Ethernet or IEEE 802.15.4 device). May be 0 and ignored
 *                      if not required by network stack.
 * @param[in] netdev    The set-up'd (but not initialized) network device, based
 *                      on device specific parameters netif_params_t::dev_params
 * @param[in] priv_data Private, stack-dependent data.
 *
 * @return  The interface enumerator on success
 * @return  @ref NETIF_INVALID on error.
 */
netif_t netif_setup(const netif_params_t *params, unsigned subtype,
                    netdev_t *netdev, void *priv_data);

/**
 * @brief   Iterator for the interfaces
 *
 * Returns interface after @p last. To start use `last == NETIF_INVALID`.
 *
 * @note    Supposed to be implemented by the networking module
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
 * @note    Supposed to be implemented by the networking module
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
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] name  The name of an interface. Must not be `NULL`.
 *
 * @return  The identifier of the interface on success.
 * @return  @ref NETIF_INVALID if no interface is named @p name.
 */
netif_t netif_get_by_name(const char *name);

/**
 * @brief   Gets option from an interface
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in]   netif   A network interface.
 * @param[in]   opt     Option type.
 * @param[out]  value   Pointer to store the option's value in.
 * @param[in]   max_len Maximal amount of byte that fit into @p value.
 *
 * @return  Number of bytes written to @p value.
 * @return  `< 0` on error, 0 on success.
 */
int netif_get_opt(netif_t netif, netopt_t opt, void *value, size_t max_len);

/**
 * @brief   Sets option to an interface
 *
 * @note    Supposed to be implemented by the networking module
 *
 * @param[in] netif     A network interface.
 * @param[in] opt       Option type.
 * @param[in] value     Pointer to store the option's value in.
 * @param[in] value_len The length of @p value.
 *
 * @return Number of bytes used from @p value.
 * @return `< 0` on error, 0 on success.
 */
int netif_set_opt(netif_t netif, netopt_t opt, void *value, size_t value_len);

#ifdef __cplusplus
}
#endif

#endif /* NETIF_H_ */
/** @} */
