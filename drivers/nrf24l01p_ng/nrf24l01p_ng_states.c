/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup drivers_nrf24l01p_ng
 * @{
 *
 * @file
 * @brief   Implementation of state transition procedures
 *          for the NRF24L01+ (NG) transceiver
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <assert.h>
#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#include "xtimer.h"
#include "net/netopt.h"

#include "nrf24l01p_ng_registers.h"
#include "nrf24l01p_ng_communication.h"
#include "nrf24l01p_ng_states.h"

static void _restore_address_p0(const nrf24l01p_ng_t *dev)
{
    uint8_t addr_buffer[NRF24L01P_NG_ADDR_WIDTH];
    memcpy(addr_buffer, NRF24L01P_NG_ADDR_P0(dev), NRF24L01P_NG_ADDR_WIDTH);
    nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RX_ADDR_P0, addr_buffer,
                           NRF24L01P_NG_ADDR_WIDTH);
    nrf24l01p_ng_reg8_write(dev, NRF24L01P_NG_REG_SETUP_AW,
        NRF24L01P_NG_FLG_AW(nrf24l01p_ng_valtoe_aw(NRF24L01P_NG_ADDR_WIDTH)));
}

void nrf24l01p_ng_transition_to_power_down(nrf24l01p_ng_t *dev)
{
    DEBUG_PUTS("[nrf24l01p_ng] transition to POWER_DOWN");
    assert(dev->state & NRF24L01P_NG_TRANSITION_TO_POWER_DOWN);
    uint8_t config = NRF24L01P_NG_FLG_PWR_UP;
    nrf24l01p_ng_reg8_clear(dev, NRF24L01P_NG_REG_CONFIG, &config);
    dev->state = NRF24L01P_NG_STATE_POWER_DOWN;
}

void nrf24l01p_ng_transition_to_standby_1(nrf24l01p_ng_t *dev)
{
    DEBUG_PUTS("[nrf24l01p_ng] transition to STANDBY_1");
    assert(dev->state & NRF24L01P_NG_TRANSITION_TO_STANDBY_1);
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
            gpio_clear(dev->params.pin_ce);
            uint8_t config = NRF24L01P_NG_FLG_PWR_UP;
            nrf24l01p_ng_reg8_set(dev, NRF24L01P_NG_REG_CONFIG, &config);
            xtimer_usleep(NRF24L01P_NG_DELAY_US_START_UP);
            break;
        case NRF24L01P_NG_STATE_RX_MODE:
            gpio_clear(dev->params.pin_ce);
            break;
        case NRF24L01P_NG_STATE_STANDBY_2:
        case NRF24L01P_NG_STATE_TX_MODE:
            gpio_clear(dev->params.pin_ce);
            _restore_address_p0(dev);
            /* TX finished with one packet */
            break;
        default:
            return;
    }
    dev->state = NRF24L01P_NG_STATE_STANDBY_1;
}

void nrf24l01p_ng_transition_to_standby_2(nrf24l01p_ng_t *dev)
{
    DEBUG_PUTS("[nrf24l01p_ng] transition to STANDBY_2");
    assert(dev->state & NRF24L01P_NG_TRANSITION_TO_STANDBY_2);
    switch (dev->state) {
        case NRF24L01P_NG_STATE_STANDBY_1:;
            /* TX FIFO empty */
            uint8_t config = NRF24L01P_NG_FLG_PRIM_RX;
            nrf24l01p_ng_reg8_clear(dev, NRF24L01P_NG_REG_CONFIG, &config);
            gpio_set(dev->params.pin_ce);
            break;
        case NRF24L01P_NG_STATE_TX_MODE:
            gpio_set(dev->params.pin_ce);
            break;
        default:
            return;
    }
    dev->state = NRF24L01P_NG_STATE_STANDBY_2;
}

void nrf24l01p_ng_transition_to_rx_mode(nrf24l01p_ng_t *dev)
{
    DEBUG_PUTS("[nrf24l01p_ng] transition to RX_MODE");
    assert(dev->state & NRF24L01P_NG_TRANSITION_TO_RX_MODE);
    if (nrf24l01p_ng_reg8_read(dev, NRF24L01P_NG_REG_FIFO_STATUS) &
        NRF24L01P_NG_FLG_RX_FULL) {
        nrf24l01p_ng_flush_rx(dev);
    }
    uint8_t config = NRF24L01P_NG_FLG_PRIM_RX;
    nrf24l01p_ng_reg8_set(dev, NRF24L01P_NG_REG_CONFIG, &config);
    gpio_set(dev->params.pin_ce);
    xtimer_usleep(NRF24L01P_NG_DELAY_US_RX_SETTLING);
    dev->state = NRF24L01P_NG_STATE_RX_MODE;
}

void nrf24l01p_ng_transition_to_tx_mode(nrf24l01p_ng_t *dev)
{
    DEBUG_PUTS("[nrf24l01p_ng] transition to TX_MODE");
    assert(dev->state & NRF24L01P_NG_TRANSITION_TO_TX_MODE);
    /* TX FIFI not empty */
    uint8_t config = NRF24L01P_NG_FLG_PRIM_RX;
    nrf24l01p_ng_reg8_clear(dev, NRF24L01P_NG_REG_CONFIG, &config);
    dev->state = NRF24L01P_NG_STATE_TX_MODE;
}
