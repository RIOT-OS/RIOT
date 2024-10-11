/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    drivers_netdev_lora LoRa drivers
 * @ingroup     drivers_netdev_api
 * @{
 *
 * @file
 * @brief       Definitions for netdev common LoRa code
 *
 * @author      José Ignacio Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef NET_NETDEV_LORA_H
#define NET_NETDEV_LORA_H

#include <stdint.h>

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Received LoRa packet status information
 */
typedef struct {
    int16_t rssi;           /**< RSSI of a received packet */
    int8_t snr;             /**< S/N ratio */
} netdev_lora_rx_info_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_NETDEV_LORA_H */
