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
 * @brief   Implementation of the public NRF24L01+ (NG) device interface
 *
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"
#include "kernel_defines.h"

#include "nrf24l01p_ng_netdev.h"
#include "nrf24l01p_ng_constants.h"
#include "nrf24l01p_ng_communication.h"
#include "nrf24l01p_ng_registers.h"
#include "nrf24l01p_ng_states.h"
#if IS_USED(MODULE_NRF24L01P_NG_DIAGNOSTICS)
#include "nrf24l01p_ng_diagnostics.h"
#endif

#define NRF24L01P_NG_REG_RX_ADDR_PX(x)  (NRF24L01P_NG_REG_RX_ADDR_P0 + (x))
#define NRF24L01P_NG_REG_RX_PW_PX(x)    (NRF24L01P_NG_REG_RX_PW_P0 + (x))

int nrf24l01p_ng_setup(nrf24l01p_ng_t *dev,
                       const nrf24l01p_ng_params_t *params,
                       uint8_t index)
{
    assert(dev);
    assert(params);
    memset((char *)dev + sizeof(netdev_t), 0x00,
           sizeof(nrf24l01p_ng_t) - sizeof(netdev_t));
    dev->state = NRF24L01P_NG_STATE_UNDEFINED;
    dev->idle_state = NRF24L01P_NG_STATE_RX_MODE;
    dev->params = *params;
    dev->netdev.driver = &nrf24l01p_ng_driver;
    netdev_register(&dev->netdev, NETDEV_NRF24L01P_NG, index);
    return 0;
}

int nrf24l01p_ng_set_enable_pipe(nrf24l01p_ng_t *dev, nrf24l01p_ng_pipe_t pipe,
                                 bool enable)
{
    assert(dev);
    if (pipe >= NRF24L01P_NG_PX_NUM_OF) {
        return -EINVAL;
    }
    uint8_t en_aa = (1 << pipe);
    uint8_t dynpd = (1 << pipe);
    uint8_t en_rx_addr = (1 << pipe);
    nrf24l01p_ng_acquire(dev);
    if (enable) {
        nrf24l01p_ng_reg8_set(dev, NRF24L01P_NG_REG_EN_AA, &en_aa);
        nrf24l01p_ng_reg8_set(dev, NRF24L01P_NG_REG_DYNPD, &dynpd);
        nrf24l01p_ng_reg8_set(dev, NRF24L01P_NG_REG_EN_RXADDR, &en_rx_addr);
    }
    else {
        nrf24l01p_ng_reg8_clear(dev, NRF24L01P_NG_REG_EN_AA, &en_aa);
        nrf24l01p_ng_reg8_clear(dev, NRF24L01P_NG_REG_DYNPD, &dynpd);
        nrf24l01p_ng_reg8_clear(dev, NRF24L01P_NG_REG_EN_RXADDR, &en_rx_addr);
    }
    nrf24l01p_ng_release(dev);
    return 0;
}

int nrf24l01p_ng_get_enable_pipe(nrf24l01p_ng_t *dev, nrf24l01p_ng_pipe_t pipe,
                                 bool* enable)
{
    assert(dev);
    if (pipe >= NRF24L01P_NG_PX_NUM_OF) {
        return -EINVAL;
    }
    uint8_t en_rx_addr;
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_read_reg(dev, NRF24L01P_NG_REG_EN_RXADDR, &en_rx_addr, 1);
    nrf24l01p_ng_release(dev);
    *enable = !!(en_rx_addr & (1 << pipe));
    return 0;
}

int nrf24l01p_ng_set_air_data_rate(nrf24l01p_ng_t *dev,
                                   nrf24l01p_ng_rfdr_t data_rate)
{
    assert(dev);
    if (data_rate >= NRF24L01P_NG_RF_DR_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    uint8_t rf_setup = NRF24L01P_NG_FLG_RF_DR(data_rate);
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_reg8_mod(dev, NRF24L01P_NG_REG_RF_SETUP,
                          NRF24L01P_NG_MSK_RF_DR, &rf_setup);
    nrf24l01p_ng_release(dev);
    dev->params.config.cfg_data_rate = data_rate;
    return 0;
}

uint16_t nrf24l01p_ng_get_air_data_rate(const nrf24l01p_ng_t *dev,
                                        nrf24l01p_ng_rfdr_t *data_rate)
{
    assert(dev);
    if (data_rate) {
        *data_rate = dev->params.config.cfg_data_rate;
    }
    return nrf24l01p_ng_etoval_rfdr(dev->params.config.cfg_data_rate);
}

int nrf24l01p_ng_set_crc(nrf24l01p_ng_t *dev, nrf24l01p_ng_crc_t crc)
{
    assert(dev);
    if (crc > NRF24L01P_NG_CRC_2BYTE) {
        return -EINVAL;
    }
    if (crc == NRF24L01P_NG_CRC_0BYTE) {
        return -ENOTSUP;
    }
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    uint8_t config = NRF24L01P_NG_FLG_CRCO(crc);
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_reg8_mod(dev, NRF24L01P_NG_REG_CONFIG, NRF24L01P_NG_MSK_CRC,
                       &config);
    nrf24l01p_ng_release(dev);
    dev->params.config.cfg_crc = crc;
    return 0;
}

uint8_t nrf24l01p_ng_get_crc(const nrf24l01p_ng_t *dev,
                             nrf24l01p_ng_crc_t *crc)
{
    assert(dev);
    if (crc) {
        *crc = dev->params.config.cfg_crc;
    }
    return nrf24l01p_ng_etoval_crc(dev->params.config.cfg_crc);
}

int nrf24l01p_ng_set_tx_power(nrf24l01p_ng_t *dev,
                              nrf24l01p_ng_tx_power_t power)
{
    assert(dev);
    if (power >= NRF24L01P_NG_TX_POWER_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    uint8_t rf_setup = NRF24L01P_NG_FLG_RF_PWR(power);
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_reg8_mod(dev, NRF24L01P_NG_REG_RF_SETUP,
                          NRF24L01P_NG_MSK_RF_PWR, &rf_setup);
    nrf24l01p_ng_release(dev);
    dev->params.config.cfg_tx_power = power;
    return 0;
}

int8_t nrf24l01p_ng_get_tx_power(const nrf24l01p_ng_t *dev,
                                 nrf24l01p_ng_tx_power_t *power)
{
    assert(dev);
    if (power) {
        *power = dev->params.config.cfg_tx_power;
    }
    return nrf24l01p_ng_etoval_tx_power(dev->params.config.cfg_tx_power);
}

int nrf24l01p_ng_set_channel(nrf24l01p_ng_t *dev, uint8_t channel)
{
    assert(dev);
    if (channel >= NRF24L01P_NG_NUM_CHANNELS) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    uint8_t rf_ch = NRF24L01P_NG_FLG_RF_CH(channel);
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_reg8_mod(dev, NRF24L01P_NG_REG_RF_CH,
                          NRF24L01P_NG_MSK_RF_CH, &rf_ch);
    nrf24l01p_ng_release(dev);
    dev->params.config.cfg_channel = channel;
    return 0;
}

uint8_t nrf24l01p_ng_get_channel(const nrf24l01p_ng_t *dev)
{
    assert(dev);
    return dev->params.config.cfg_channel;
}

int nrf24l01p_ng_set_rx_address(nrf24l01p_ng_t *dev, const uint8_t *addr,
                                nrf24l01p_ng_pipe_t pipe)
{
    assert(dev);
    assert(addr);
    if (pipe >= NRF24L01P_NG_PX_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_ng_acquire(dev);
    if (pipe == NRF24L01P_NG_P0) {
        nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RX_ADDR_PX(pipe),
                               addr, NRF24L01P_NG_ADDR_WIDTH);
        memcpy(NRF24L01P_NG_ADDR_P0(dev), addr, NRF24L01P_NG_ADDR_WIDTH);
    }
    else if (pipe == NRF24L01P_NG_P1) {
        nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RX_ADDR_PX(pipe),
                               addr, NRF24L01P_NG_ADDR_WIDTH);
        memcpy(NRF24L01P_NG_ADDR_P1(dev), addr, NRF24L01P_NG_ADDR_WIDTH);
    }
    else {
        nrf24l01p_ng_write_reg(dev, NRF24L01P_NG_REG_RX_ADDR_PX(pipe),
                               addr, 1);
        NRF24L01P_NG_ADDR_PX_LSB(dev, pipe) = *addr;
    }
    nrf24l01p_ng_release(dev);
    return 0;
}

int nrf24l01p_ng_get_rx_address(const nrf24l01p_ng_t *dev, uint8_t *addr,
                                nrf24l01p_ng_pipe_t pipe)
{
    assert(dev);
    assert(addr);
    if (pipe >= NRF24L01P_NG_PX_NUM_OF) {
        return -EINVAL;
    }
    if (pipe == NRF24L01P_NG_P0) {
        memcpy(addr, NRF24L01P_NG_ADDR_P0(dev), NRF24L01P_NG_ADDR_WIDTH);
    }
    else {
        memcpy(addr, NRF24L01P_NG_ADDR_P1(dev), NRF24L01P_NG_ADDR_WIDTH);
        if (pipe > NRF24L01P_NG_P1) {
            addr[NRF24L01P_NG_ADDR_WIDTH - 1] =
                NRF24L01P_NG_ADDR_PX_LSB(dev, pipe);
        }
    }
    return NRF24L01P_NG_ADDR_WIDTH;
}

int nrf24l01p_ng_set_max_retransm(nrf24l01p_ng_t *dev, uint8_t max_rt)
{
    assert(dev);
    if (max_rt > NRF24L01P_NG_MAX_RETRANSMISSIONS) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    uint8_t setup_retr = NRF24L01P_NG_FLG_ARC(max_rt);
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_reg8_mod(dev, NRF24L01P_NG_REG_SETUP_RETR,
                          NRF24L01P_NG_MSK_ARC, &setup_retr);
    nrf24l01p_ng_release(dev);
    dev->params.config.cfg_max_retr = max_rt;
    return 0;
}

uint8_t nrf24l01p_ng_get_max_retransm(const nrf24l01p_ng_t *dev)
{
    assert(dev);
    return dev->params.config.cfg_max_retr;
}

int nrf24l01p_ng_set_retransm_delay(nrf24l01p_ng_t *dev,
                                    nrf24l01p_ng_ard_t rt_delay)
{
    assert(dev);
    if (rt_delay >= NRF24L01P_NG_ARD_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    uint8_t setup_retr = NRF24L01P_NG_FLG_ARD(rt_delay);
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_reg8_mod(dev, NRF24L01P_NG_REG_SETUP_RETR,
                          NRF24L01P_NG_MSK_ARD, &setup_retr);
    nrf24l01p_ng_release(dev);
    dev->params.config.cfg_retr_delay = rt_delay;
    return 0;
}

uint16_t nrf24l01p_ng_get_retransm_delay(const nrf24l01p_ng_t *dev,
                                         nrf24l01p_ng_ard_t *rt_delay)
{
    assert(dev);
    if (rt_delay) {
        *rt_delay = dev->params.config.cfg_retr_delay;
    }
    return nrf24l01p_ng_etoval_ard(dev->params.config.cfg_retr_delay);
}

int nrf24l01p_ng_set_state(nrf24l01p_ng_t *dev, nrf24l01p_ng_state_t state)
{
    switch (dev->state) {
        case NRF24L01P_NG_STATE_POWER_DOWN:
        case NRF24L01P_NG_STATE_STANDBY_1:
        case NRF24L01P_NG_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_ng_state_t old = dev->state;
    nrf24l01p_ng_acquire(dev);
    if (state == NRF24L01P_NG_STATE_POWER_DOWN) {
        if (dev->state != NRF24L01P_NG_STATE_POWER_DOWN) {
            nrf24l01p_ng_transition_to_power_down(dev);
        }
    }
    else if (state == NRF24L01P_NG_STATE_STANDBY_1) {
        if (dev->state != NRF24L01P_NG_STATE_STANDBY_1) {
            nrf24l01p_ng_transition_to_standby_1(dev);
        }
    }
    else if (state == NRF24L01P_NG_STATE_RX_MODE) {
        if (dev->state != NRF24L01P_NG_STATE_RX_MODE) {
            if (state != NRF24L01P_NG_STATE_STANDBY_1) {
                nrf24l01p_ng_transition_to_standby_1(dev);
            }
            nrf24l01p_ng_transition_to_rx_mode(dev);
        }
    }
    else {
        nrf24l01p_ng_release(dev);
        return -ENOTSUP;
    }
    nrf24l01p_ng_release(dev);
    return (int)old;
}

nrf24l01p_ng_state_t nrf24l01p_ng_get_state(const nrf24l01p_ng_t *dev)
{
    assert(dev);
    return dev->state;
}

#if IS_USED(MODULE_NRF24L01P_NG_DIAGNOSTICS)
void nrf24l01p_ng_print_all_regs(nrf24l01p_ng_t *dev)
{
    nrf24l01p_ng_acquire(dev);
    nrf24l01p_ng_diagnostics_print_all_regs(dev);
    nrf24l01p_ng_release(dev);
}

void nrf24l01p_ng_print_dev_info(const nrf24l01p_ng_t *dev)
{
    nrf24l01p_ng_diagnostics_print_dev_info(dev);
}
#endif
