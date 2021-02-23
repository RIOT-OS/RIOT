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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A Link Check request was scheduled
 */
#define GNRC_NETIF_LORAWAN_FLAGS_LINK_CHECK                (0x1U)

/**
 * @brief Encode LoRaWAN port in GNRC netif header.
 *
 * When set, GNRC netif will interpret the destination address of the
 * GNRC netif header as the LoRaWAN port. For downlinks, a GNRC netif header
 * with the received port in destination field will be included in the
 * first snip.
 *
 * @deprecated From Release 2021.10 all GNRC LoRaWAN packets will include
 * the GNRC Netif header. Therefore this parameter will be removed
 */
#if defined(DOXYGEN)
#define CONFIG_GNRC_NETIF_LORAWAN_NETIF_HDR
#endif

/**
 * @brief   GNRC LoRaWAN interface descriptor
 */
typedef struct {
    uint8_t nwkskey[LORAMAC_NWKSKEY_LEN];   /**< network SKey buffer */
    uint8_t appskey[LORAMAC_APPSKEY_LEN];   /**< App SKey buffer */
    uint8_t appkey[LORAMAC_APPKEY_LEN];     /**< App Key buffer */
    uint8_t deveui[LORAMAC_DEVEUI_LEN];     /**< Device EUI buffer */
    uint8_t appeui[LORAMAC_APPEUI_LEN];     /**< App EUI buffer */
    gnrc_lorawan_t mac;                     /**< gnrc lorawan mac descriptor */
    uint8_t flags;                          /**< flags for the LoRaWAN interface */
    uint8_t demod_margin;                   /**< value of last demodulation margin */
    uint8_t num_gateways;                   /**< number of gateways of last link check */
    uint8_t datarate;                       /**< LoRaWAN datarate for the next transmission */
    uint8_t port;                           /**< LoRaWAN port for the next transmission */
    uint8_t ack_req;                        /**< Request ACK in the next transmission */
    uint8_t otaa;                           /**< whether the next transmission is OTAA or not */
} gnrc_netif_lorawan_t;

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETIF_LORAWAN_H */
/** @} */
