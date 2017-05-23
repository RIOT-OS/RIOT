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
    NETIF_TYPE_UNDEF = 0;       /**< Interface is of no specific type (raw @ref drivers_netdev) */
#if defined(MODULE_GNRC) || DOXYGEN
    /**
     * @brief   Default GNRC network interface (without software MAC)
     *
     * @note only available with @ref net_gnrc "GNRC" module.
     */
    NETIF_TYPE_GNRC_NETIF,
#endif
#if defined(MODULE_LWIP_ETHERNET) || DOXYGEN
    /**
     * @brief   lwIP Ethernet network interface
     *
     * @note only available with `lwip_ethernet` module of the @ref pkg_lwip package.
     */
    NETIF_TYPE_LWIP_ETHERNET,
#endif /* MODULE_LWIP */
#if defined(MODUE_LWIP_SIXLOWPAN) || DOXYGEN
    /**
     * @brief   lwIP 6LoWPAN network interface
     *
     * @note only available with `lwip_sixlowpan` module of the @ref pkg_lwip package.
     */
    NETIF_TYPE_LWIP_6LO,
#endif /* MODULE_LWIP */
#if defined(MODULE_EMB6) || DOXYGEN
    /**
     * @brief   emb6 interface
     *
     * @note only available with @ref pkg_emb6
     */
    NETIF_TYPE_EMB6,
#endif
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

/**
 * @brief   Network interface enumeration type
 */
typedef intptr_t netif_t;

/**
 * @brief   Gets a network interface parameter struct from an array of parameter
 *          structs by network device parameters
 *
 * @param[in] netif_params  Array of network interface parameters.
 * @param[in] dev_pararms   A struct of network device parameters.
 *
 * @return  The parameters with netif_params_t::dev_params == @p dev_params
 * @return  NULL, if there are no such parameters in @p netif_params
 */
netif_params_t *netif_params_get_by_dev(netif_params_t *netif_params,
                                        const void *dev_params);

/**
 * @brief   Initialize interface
 *
 * @param[in] params    Initialization parameters of the interface
 *
 * @return  The interface enumerator on success
 * @return  @ref NETIF_INVALID on error.
 */
netif_t netif_init(const netif_params_t *params);

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

#ifdef __cplusplus
}
#endif

#endif /* NETIF_H_ */
/** @} */
