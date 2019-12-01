/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup drivers_nrf24l01p
 * @{
 *
 * @file
 * @brief   Operation states of NRF24L01P devices
 *
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author  Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author  Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef NRF24L01P_STATES_H
#define NRF24L01P_STATES_H

#include "nrf24l01p.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Transition delay to POWER_ON state
 */
#define NRF24L01P_DELAY_US_POWER_ON_RESET   (100000)

/**
 * @brief   Wakeup delay
 */
#define NRF24L01P_DELAY_US_START_UP         (1500)

/**
 * @brief   Transition delay to RX mode
 */
#define NRF24L01P_DELAY_US_RX_SETTLING      (130)

/**
 * @brief   Transition delay to TX mode
 */
#define NRF24L01P_DELAY_US_TX_SETTLING      (130)

/**
 * @brief   CE gpio pin pull-low time to trigger the transmission of a frame
 */
#define NRF24L01P_DELAY_US_CE_HIGH_PULSE    (10)

#if !defined (NDEBUG) || defined(DOXYGEN)
/**
 * @name NRF24L01P Transitions
 *
 * Decode possible state transitions
 * @{
 */
/**
 * @brief Transitions that can be made from state UNDEFINED
 */
#define NRF24L01P_TRANSITIONS_FROM_UNDEFINED    (NRF24L01P_STATE_POWER_DOWN)

/**
 * @brief Transitions that can be made from state POWER_DOWN
 */
#define NRF24L01P_TRANSITIONS_FROM_POWER_DOWN   (NRF24L01P_STATE_POWER_DOWN  | \
                                                 NRF24L01P_STATE_STANDBY_1)
/**
 * @brief Transitions that can be made from state STANDBY_1
 */
#define NRF24L01P_TRANSITIONS_FROM_STANDBY_1    (NRF24L01P_STATE_STANDBY_1   | \
                                                 NRF24L01P_STATE_POWER_DOWN  | \
                                                 NRF24L01P_STATE_STANDBY_2   | \
                                                 NRF24L01P_STATE_RX_MODE     | \
                                                 NRF24L01P_STATE_TX_MODE)
/**
 * @brief Transitions that can be made from state STANDBY_2
 */
#define NRF24L01P_TRANSITIONS_FROM_STANDBY_2    (NRF24L01P_STATE_STANDBY_2   | \
                                                 NRF24L01P_STATE_TX_MODE     | \
                                                 NRF24L01P_STATE_POWER_DOWN)
/**
 * @brief Transitions that can be made from state RX_MODE
 */
#define NRF24L01P_TRANSITIONS_FROM_RX_MODE      (NRF24L01P_STATE_RX_MODE     | \
                                                 NRF24L01P_STATE_POWER_DOWN  | \
                                                 NRF24L01P_STATE_STANDBY_1)
/**
 * @brief Transitions that can be made from state TX_MODE
 */
#define NRF24L01P_TRANSITIONS_FROM_TX_MODE      (NRF24L01P_STATE_TX_MODE     | \
                                                 NRF24L01P_STATE_POWER_DOWN  | \
                                                 NRF24L01P_STATE_STANDBY_1   | \
                                                 NRF24L01P_STATE_STANDBY_2)
/** @} */
#endif

/**
 * @brief   Wait until device is powered on
 *
 * @post    If there is a device, it is able to operate
 *
 * @param[in] dev       NRF24L01P device handle
 */
void nrf24l01p_power_on(nrf24l01p_t *dev);

/**
 * @brief   Put device to sleep
 *
 * @param[in] dev       NRF24L01P device handle
 */
void nrf24l01p_transition_to_power_down(nrf24l01p_t *dev);

/**
 * @brief   Go to idle state, wake up device
 *
 * @param[in] dev       NRF24L01P device handle
 */
void nrf24l01p_transition_to_standby_1(nrf24l01p_t *dev);

/**
 * @brief   Go to "be ready to transmit" state
 *
 * @param[in] dev   NRF24L01P device handle
 */
void nrf24l01p_transition_to_standby_2(nrf24l01p_t *dev);

/**
 * @brief   Go to Rx mode
 *
 * @param[in] dev   NRF24L01P device handle
 */
void nrf24l01p_transition_to_rx_mode(nrf24l01p_t *dev);

/**
 * @brief   Go to Tx mode
 *
 * @param[in] dev   NRF24L01P device handle
 */
void nrf24l01p_transition_to_tx_mode(nrf24l01p_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* NRF24L01P_STATES_H */
/** @} */
