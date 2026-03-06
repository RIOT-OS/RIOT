/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
