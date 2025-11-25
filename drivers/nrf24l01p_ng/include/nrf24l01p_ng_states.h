/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Operation states of NRF24L01+ (NG) devices
 *
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_NG_STATES_H
#define NRF24L01P_NG_STATES_H

#include "nrf24l01p_ng.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wakeup delay
 */
#define NRF24L01P_NG_DELAY_US_START_UP      (1500)

/**
 * @brief   Transition delay to RX mode
 */
#define NRF24L01P_NG_DELAY_US_RX_SETTLING   (130)

/**
 * @brief   Transition delay to TX mode
 */
#define NRF24L01P_NG_DELAY_US_TX_SETTLING   (130)

/**
 * @brief   CE gpio pin pull-low to trigger a transmission
 */
#define NRF24L01P_NG_DELAY_US_CE_HIGH_PULSE (10)

/**
 * @brief   Put device to sleep
 *
 * @param[in] dev       NRF24L01+ device handle
 */
void nrf24l01p_ng_transition_to_power_down(nrf24l01p_ng_t *dev);

/**
 * @brief   Go to idle state, wake up device
 *
 * @param[in] dev       NRF24L01+ device handle
 */
void nrf24l01p_ng_transition_to_standby_1(nrf24l01p_ng_t *dev);

/**
 * @brief   Go to "be ready to transmit" state
 *
 * @param[in] dev   NRF24L01+ device handle
 */
void nrf24l01p_ng_transition_to_standby_2(nrf24l01p_ng_t *dev);

/**
 * @brief   Go to Rx mode
 *
 * @param[in] dev   NRF24L01+ device handle
 */
void nrf24l01p_ng_transition_to_rx_mode(nrf24l01p_ng_t *dev);

/**
 * @brief   Go to Tx mode
 *
 * @param[in] dev   NRF24L01+ device handle
 */
void nrf24l01p_ng_transition_to_tx_mode(nrf24l01p_ng_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_NG_STATES_H */
/** @} */
