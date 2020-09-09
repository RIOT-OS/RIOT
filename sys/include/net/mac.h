/*
 * Copyright (C) 2017 Fundación Inria Chile
 *               2019 HAW Hamburg
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_MAC_H
#define NET_MAC_H

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MAC parameters descriptor */
typedef struct {
    uint8_t placeholder;
} mac_params_t;

/**
 * @brief MAC types
 */
typedef struct {
    mutex_t lock;
    mac_params_t* params;
} mac_t;

/**
 * @brief Indicate the MAC layer there was a timeout event
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void mac_event_timeout(mac_t *mac);

/**
 * @brief Indicate the MAC layer when the transmission finished
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void mac_event_tx_complete(mac_t *mac);

/**
 * @brief Init GNRC LoRaWAN
 *
 * @param[in] mac pointer to the MAC descriptor
 * @param[in] nwkskey buffer to store the NwkSKey. Should be at least 16 bytes long
 * @param[in] appskey buffer to store the AppsKey. Should be at least 16 bytes long
 */
void mac_init(mac_t *mac, mac_params_t* params);

/**
 * @brief Fetch a LoRaWAN packet from the radio.
 *
 *        To be called on radio RX done event.
 *
 * @param[in] mac pointer to the MAC descriptor
 */
void mac_recv(mac_t *mac);

/**
 * @brief Acquire the MAC layer
 *
 * @param[in] mac pointer to the MAC descriptor
 *
 * @return true on success
 * @return false if MAC is already acquired
 */
static inline int mac_acquire(mac_t *mac)
{
    return mutex_trylock(&mac->lock);
}

/**
 * @brief Release the MAC layer
 *
 * @param[in] mac pointer to the MAC descriptor
 */
static inline void mac_release(mac_t *mac)
{
     mutex_unlock(&mac->lock);
}


#ifdef __cplusplus
}
#endif

#endif /* NET_MAC_H */
/** @} */
