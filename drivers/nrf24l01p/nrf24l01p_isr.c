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
 * @brief     Implementation of ISR functions for the NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "nrf24l01p_communication.h"
#include "nrf24l01p_isr.h"

netdev_event_t nrf24l01p_isr_max_rt(const nrf24l01p_t *dev)
{
    assert(dev->state == NRF24L01P_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_STATE_RX_MODE   ||
           dev->state == NRF24L01P_STATE_TX_MODE);
    DEBUG("[nrf24l01p] IRS MAX_RT\n");
    netdev_event_t event = NETDEV_EVENT_TX_NOACK;
    nrf24l01p_flush_tx(dev);
    return event;
}

netdev_event_t nrf24l01p_isr_rx_dr(const nrf24l01p_t *dev)
{
    assert(dev->state == NRF24L01P_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_STATE_RX_MODE   ||
           dev->state == NRF24L01P_STATE_TX_MODE);
    DEBUG("[nrf24l01p] IRS RX_DR\n");
    netdev_event_t event = NETDEV_EVENT_RX_COMPLETE;
    return event;
}

netdev_event_t nrf24l01p_isr_tx_ds(const nrf24l01p_t *dev)
{
    assert(dev->state == NRF24L01P_STATE_STANDBY_1 ||
           dev->state == NRF24L01P_STATE_STANDBY_2 ||
           dev->state == NRF24L01P_STATE_RX_MODE   ||
           dev->state == NRF24L01P_STATE_TX_MODE);
    DEBUG("[nrf24l01p] IRS TX_DS\n");
    netdev_event_t event = NETDEV_EVENT_TX_COMPLETE;
    return event;
}
