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
 * @brief   Implementation utility functions for the
 *          NRF24L01P device interface
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <string.h>

#include "nrf24l01p_util.h"

const char *nrf24l01p_state_to_string(nrf24l01p_state_t state)
{
    if (state == NRF24L01P_STATE_POWER_DOWN) {
        return "POWER_DOWN";
    }
    if (state == NRF24L01P_STATE_STANDBY_1) {
        return "STANDBY_1";
    }
    if (state == NRF24L01P_STATE_STANDBY_2) {
        return "STANDBY_2";
    }
    if (state == NRF24L01P_STATE_TX_MODE) {
        return "TX_MODE";
    }
    if (state == NRF24L01P_STATE_RX_MODE) {
        return "RX_MODE";
    }
    return "UNDEFINED";
}

nrf24l01p_state_t nrf24l01p_string_to_state(const char *sstate)
{
    if (!strcmp(sstate, "POWER_DOWN")) {
        return NRF24L01P_STATE_POWER_DOWN;
    }
    if (!strcmp(sstate, "STANDBY_1")) {
        return NRF24L01P_STATE_STANDBY_1;
    }
    if (!strcmp(sstate, "STANDBY_2")) {
        return NRF24L01P_STATE_STANDBY_2;
    }
    if (!strcmp(sstate, "TX_MODE")) {
        return NRF24L01P_STATE_TX_MODE;
    }
    if (!strcmp(sstate, "RX_MODE")) {
        return NRF24L01P_STATE_RX_MODE;
    }
    return NRF24L01P_STATE_UNDEFINED;
}

netopt_state_t nrf24l01p_state_to_netif(nrf24l01p_state_t state)
{
    if (state == NRF24L01P_STATE_POWER_DOWN) {
        return NETOPT_STATE_SLEEP;
    }
    if (state == NRF24L01P_STATE_STANDBY_1) {
        return NETOPT_STATE_STANDBY;
    }
    if (state == NRF24L01P_STATE_STANDBY_2) {
        return NETOPT_STATE_TX;
    }
    if (state == NRF24L01P_STATE_TX_MODE) {
        return NETOPT_STATE_TX;
    }
    if (state == NRF24L01P_STATE_RX_MODE) {
        return NETOPT_STATE_RX;
    }
    return NETOPT_STATE_OFF; /* error */
}

nrf24l01p_state_t nrf24l01p_state_from_netif(netopt_state_t state)
{
    if (state == NETOPT_STATE_SLEEP) {
        return NRF24L01P_STATE_POWER_DOWN;
    }
    if (state == NETOPT_STATE_STANDBY) {
        return NRF24L01P_STATE_STANDBY_1;
    }
    if (state == NETOPT_STATE_TX) {
        return NRF24L01P_STATE_TX_MODE;
    }
    if (state == NETOPT_STATE_RX) {
        return NRF24L01P_STATE_RX_MODE;
    }
    return NRF24L01P_STATE_UNDEFINED;
}
