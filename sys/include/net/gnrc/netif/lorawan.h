/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_netif
 * @{
 *
 * @file
 * @brief  LoRaWAN adaption for @ref net_gnrc_netif
 *
 * @author  Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */
#ifndef NET_GNRC_NETIF_LORAWAN_H
#define NET_GNRC_NETIF_LORAWAN_H

#include "net/gnrc/lorawan.h"

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A Link Check request was scheduled
 */
#define GNRC_NETIF_LORAWAN_FLAGS_LINK_CHECK                (0x1U)

/**
 * @brief   GNRC LoRaWAN interface descriptor
 */
typedef struct {
    uint8_t appskey[LORAMAC_APPSKEY_LEN];           /**< App SKey buffer */
    uint8_t fnwksintkey[LORAMAC_FNWKSINTKEY_LEN];   /**< Forwarding Network session integrity key buffer */
    uint8_t nwkkey[LORAMAC_NWKKEY_LEN];             /**< Network key buffer. Mapped to AppKey if LoRaWAN 1.0x */
#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    uint8_t appkey[LORAMAC_APPKEY_LEN];             /**< App Key buffer. The AppKey used in LoRaWAN 1.0x has been mapped to the nwkkey in LoRaWAN 1.1x */
    uint8_t snwksintkey[LORAMAC_SNWKSINTKEY_LEN];   /**< Serving Network session integrity key buffer */
    uint8_t nwksenckey[LORAMAC_NWKSENCKEY_LEN];     /**< Network session encryption key buffer */
    uint8_t jsintkey[LORAMAC_JSINTKEY_LEN];         /**< Join session integrity key buffer */
    uint8_t jsenckey[LORAMAC_JSENCKEY_LEN];         /**< Join session encryption key buffer */
#endif
    uint8_t deveui[LORAMAC_DEVEUI_LEN];             /**< Device EUI buffer */
    uint8_t joineui[LORAMAC_JOINEUI_LEN];           /**< Join EUI buffer */
    gnrc_lorawan_t mac;                             /**< gnrc lorawan mac descriptor */
    ztimer_t timer;                                 /**< General purpose timer */
    ztimer_t backoff_timer;                         /**< Backoff timer */
    uint8_t flags;                                  /**< flags for the LoRaWAN interface */
    uint8_t demod_margin;                           /**< value of last demodulation margin */
    uint8_t num_gateways;                           /**< number of gateways of last link check */
    uint8_t datarate;                               /**< LoRaWAN datarate for the next transmission */
    uint8_t port;                                   /**< LoRaWAN port for the next transmission */
    uint8_t ack_req;                                /**< Request ACK in the next transmission */
    uint8_t otaa;                                   /**< whether the next transmission is OTAA or not */
} gnrc_netif_lorawan_t;

/**
 * @brief Set the app key in the interface descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] lw_netif      pointer to the interface descriptor
 * @param[in] key           pointer to the app key
 * @param[in] len           length of the app key
 * @return 0 on success
 * @return <0 on failure
 */
static inline int gnrc_netif_lorawan_set_appkey(gnrc_netif_lorawan_t *lw_netif, const uint8_t *key,
                                                size_t len)
{
    (void)lw_netif;
    (void)key;
    (void)len;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    if (sizeof(lw_netif->appkey) < len) {
        return -1;
    }
    memcpy(lw_netif->appkey, key, len);
#endif
    return 0;
}

/**
 * @brief Get the app key from the interface descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] lw_netif       pointer to the interface descriptor
 * @return pointer to the app key
 */
static inline uint8_t * gnrc_netif_lorawan_get_appkey(gnrc_netif_lorawan_t *lw_netif)
{
    (void)lw_netif;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    return lw_netif->appkey;
#endif
    return NULL; /* NO-OP */
}

/**
 * @brief Set the serving network session integrity key in the interface descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] lw_netif      pointer to the interface descriptor
 * @param[in] key           pointer to the serving network session integrity key
 * @param[in] len           length of serving network session integrity key
 * @return 0 on success
 * @return <0 on failure
 */
static inline int gnrc_netif_lorawan_set_snwksintkey(gnrc_netif_lorawan_t *lw_netif,
                                                     const uint8_t *key, size_t len)
{
    (void)lw_netif;
    (void)key;
    (void)len;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    if (sizeof(lw_netif->snwksintkey) < len) {
        return -1;
    }
    memcpy(lw_netif->snwksintkey, key, len);
#endif
    return 0;
}

/**
 * @brief Set the network session encryption key in the interface descriptor
 *
 *  This getter function exists to allow if (IS_USED(...)) constructs in the
 *  LoRaWAN code in order to increase code coverage.
 *
 * @param[in] lw_netif      pointer to the interface descriptor
 * @param[in] key           pointer to the network session encryption key
 * @param[in] len           length of network session encryption key
 * @return 0 on success
 * @return <0 on failure
 */
static inline int gnrc_netif_lorawan_set_nwksenckey(gnrc_netif_lorawan_t *lw_netif,
                                                    const uint8_t *key, size_t len)
{
    (void)lw_netif;
    (void)key;
    (void)len;

#if IS_USED(MODULE_GNRC_LORAWAN_1_1)
    if (sizeof(lw_netif->nwksenckey) < len) {
        return -1;
    }
    memcpy(lw_netif->nwksenckey, key, len);
#endif
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_LORAWAN_H */
/** @} */
