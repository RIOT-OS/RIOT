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
 * @brief   Implementation of the public NRF24L01P device interface
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <errno.h>
#include <string.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "nrf24l01p_netdev.h"
#include "nrf24l01p_constants.h"
#include "nrf24l01p_channels.h"
#include "nrf24l01p_communication.h"
#include "nrf24l01p_registers.h"
#include "nrf24l01p_states.h"
#include "nrf24l01p_internal.h"
#include "nrf24l01p_custom_header.h"
#include "nrf24l01p_diagnostics.h"

/**
 * @brief   Struct that holds certain register addresses for any pipe
 */
typedef struct {
    uint8_t reg_pipe_addr;  /**< Register that holds the rx address */
    uint8_t reg_pipe_plw;   /**< Register that holds the expected payload width */
} nrf24l01p_pipe_regs_t;

/**
 * @brief   Table that maps data pipe indices to corresponding pipe
 *          register addresses
 */
static const nrf24l01p_pipe_regs_t reg_pipe_info[NRF24L01P_PX_NUM_OF] = {
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P0,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P0
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P1,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P1
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P2,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P2
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P3,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P3
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P4,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P4
    },
    {
        .reg_pipe_addr = NRF24L01P_REG_RX_ADDR_P5,
        .reg_pipe_plw = NRF24L01P_REG_RX_PW_P5
    }
};

static int nrf24l01p_set_payload_width(nrf24l01p_t *dev, uint8_t width,
                                       nrf24l01p_pipe_t pipe)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_ESB) {
        return 0;
    }
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    if (!width || width > NRF24L01P_MAX_PAYLOAD_WIDTH) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8_rx_pw_px_t rx_pw_px = NRF24L01P_FLG_RX_PW_PX(width);
    nrf24l01p_acquire(dev);
    nrf24l01p_write_reg(dev, reg_pipe_info[pipe].reg_pipe_plw, &rx_pw_px, 1);
    nrf24l01p_release(dev);
    if (pipe == NRF24L01P_P0) {
        dev->params.config.cfg_plw_padd_p0 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P1) {
        dev->params.config.cfg_plw_padd_p1 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P2) {
        dev->params.config.cfg_plw_padd_p2 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P3) {
        dev->params.config.cfg_plw_padd_p3 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P4) {
        dev->params.config.cfg_plw_padd_p4 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    else if (pipe == NRF24L01P_P5) {
        dev->params.config.cfg_plw_padd_p5 = NRF24L01P_MAX_PAYLOAD_WIDTH -
                                             width;
    }
    return 0;
}

static int nrf24l01p_get_payload_width(const nrf24l01p_t *dev,
                                       nrf24l01p_pipe_t pipe)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_ESB) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH;
    }
    else if (pipe == NRF24L01P_P0) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p0;
    }
    else if (pipe == NRF24L01P_P1) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p1;
    }
    else if (pipe == NRF24L01P_P2) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p2;
    }
    else if (pipe == NRF24L01P_P3) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p3;
    }
    else if (pipe == NRF24L01P_P4) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p4;
    }
    else if (pipe == NRF24L01P_P5) {
        return NRF24L01P_MAX_PAYLOAD_WIDTH - dev->params.config.cfg_plw_padd_p5;
    }
    return -ERANGE;
}

int nrf24l01p_setup(nrf24l01p_t *dev, const nrf24l01p_params_t *params)
{
    assert(dev);
    assert(params);
    if (params->config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        if (params->config.cfg_max_retr != 0) {
            return -ENOTSUP;
        }
    }
    /* Zero out everything but RIOT's driver interface, which should be
     * managed by RIOT
     */
    memset((char *)dev + sizeof(netdev_t), 0x00,
           sizeof(nrf24l01p_t) - sizeof(netdev_t));
    dev->state = NRF24L01P_STATE_UNDEFINED;
    dev->idle_state = NRF24L01P_STATE_RX_MODE;
#ifndef NDEBUG
    dev->transitions = NRF24L01P_TRANSITIONS_FROM_UNDEFINED;
#endif
    dev->params = *params;
    dev->netdev.driver = &nrf24l01p_driver;
    nrf24l01p_power_on(dev);
    return 0;
}

int nrf24l01p_set_air_data_rate(nrf24l01p_t *dev, nrf24l01p_rfdr_t data_rate)
{
    assert(dev);
    if (data_rate >= NRF24L01P_RF_DR_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8_rf_setup_t rf_setup = NRF24L01P_FLG_RF_DR(data_rate);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG_RF_SETUP, NRF24L01P_MSK_RF_DR,
                       &rf_setup);
    nrf24l01p_release(dev);
    dev->params.config.cfg_data_rate = data_rate;
    return 0;
}

uint16_t nrf24l01p_get_air_data_rate(const nrf24l01p_t *dev,
                                     nrf24l01p_rfdr_t *data_rate)
{
    assert(dev);
    if (data_rate) {
        *data_rate = dev->params.config.cfg_data_rate;
    }
    return nrf24l01p_etoval_rfdr(dev->params.config.cfg_data_rate);
}

int nrf24l01p_set_crc(nrf24l01p_t *dev, nrf24l01p_crc_t crc)
{
    assert(dev);
    if (crc >= NRF24L01P_CRC_NUM_OF) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_ESB) {
        if (crc == NRF24L01P_CRC_0BYTE) {
            return -ENOTSUP;
        }
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8_config_t config =
        (((crc & 2) ? NRF24L01P_FLG_EN_CRC : 0) |
         ((crc & 1) ? NRF24L01P_FLG_CRCO_2_BYTE : NRF24L01P_FLG_CRCO_1_BYTE));
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG_CONFIG, NRF24L01P_MSK_CRC,
                       &config);
    nrf24l01p_release(dev);
    dev->params.config.cfg_crc = crc;
    return 0;
}

uint8_t nrf24l01p_get_crc(const nrf24l01p_t *dev, nrf24l01p_crc_t *crc)
{
    assert(dev);
    if (crc) {
        *crc = dev->params.config.cfg_crc;
    }
    return nrf24l01p_etoval_crc(dev->params.config.cfg_crc);
}

int nrf24l01p_set_tx_power(nrf24l01p_t *dev, nrf24l01p_tx_power_t power)
{
    assert(dev);
    if (power >= NRF24L01P_TX_POWER_NUM_OF) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8_rf_setup_t rf_setup = NRF24L01P_FLG_RF_PWR(power);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG_RF_SETUP, NRF24L01P_MSK_RF_PWR,
                       &rf_setup);
    nrf24l01p_release(dev);
    dev->params.config.cfg_tx_power = power;
    return 0;
}

int8_t nrf24l01p_get_tx_power(const nrf24l01p_t *dev, nrf24l01p_tx_power_t *power)
{
    assert(dev);
    if (power) {
        *power = dev->params.config.cfg_tx_power;
    }
    return nrf24l01p_etoval_tx_power(dev->params.config.cfg_tx_power);
}

int nrf24l01p_set_channel(nrf24l01p_t *dev, uint8_t channel)
{
    assert(dev);
    if (channel >= NRF24L01P_NUM_CHANNELS) {
        return -EINVAL;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8_rf_ch_t rf_ch = NRF24L01P_FLG_RF_CH(vchanmap[channel]);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG_RF_CH, NRF24L01P_MSK_RF_CH,
                       &rf_ch);
    nrf24l01p_release(dev);
    dev->params.config.cfg_channel = channel;
    return 0;
}

uint8_t nrf24l01p_get_channel(const nrf24l01p_t *dev)
{
    assert(dev);
    return dev->params.config.cfg_channel;
}

int nrf24l01p_set_mtu(nrf24l01p_t *dev, uint8_t mtu, nrf24l01p_pipe_t pipe)
{
# if NRF24L01P_CUSTOM_HEADER
    return nrf24l01p_set_payload_width(dev,
                                       mtu + NRF24L01P_MAX_ADDR_WIDTH + 1,
                                       pipe);
#else
    return nrf24l01p_set_payload_width(dev, mtu, pipe);
#endif
}

int nrf24l01p_get_mtu(const nrf24l01p_t *dev, nrf24l01p_pipe_t pipe)
{
# if NRF24L01P_CUSTOM_HEADER
    return nrf24l01p_get_payload_width(dev, pipe)
                                        - (NRF24L01P_MAX_ADDR_WIDTH + 1);
#else
    return nrf24l01p_get_payload_width(dev, pipe);
#endif
}

int nrf24l01p_set_rx_address(nrf24l01p_t *dev, const uint8_t *addr,
                             size_t addr_len, nrf24l01p_pipe_t pipe)
{
    assert(dev);
    assert(addr);
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    if (pipe == NRF24L01P_P0 || pipe == NRF24L01P_P1) {
        if (addr_len !=
            nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width)) {
            return -EINVAL;
        }
        nrf24l01p_acquire(dev);
        nrf24l01p_write_reg(dev, reg_pipe_info[pipe].reg_pipe_addr, addr,
                            addr_len);
        nrf24l01p_release(dev);
        memcpy(dev->params.urxaddr.arxaddr.rx_addr_long[pipe], addr, addr_len);
    }
    else {
        if (addr_len > 1) {
            return -EINVAL;
        }
        nrf24l01p_acquire(dev);
        nrf24l01p_write_reg(dev, reg_pipe_info[pipe].reg_pipe_addr, addr,
                            addr_len);
        nrf24l01p_release(dev);
        dev->params.urxaddr.arxaddr.rx_addr_short[pipe - 2] = *addr;
    }
    return 0;
}

int nrf24l01p_get_rx_address(const nrf24l01p_t *dev, uint8_t *addr,
                             nrf24l01p_pipe_t pipe)
{
    assert(dev);
    assert(addr);
    if (pipe >= NRF24L01P_PX_NUM_OF) {
        return -ERANGE;
    }
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);
    if (pipe == NRF24L01P_P0 || pipe == NRF24L01P_P1) {
        memcpy(addr, dev->params.urxaddr.arxaddr.rx_addr_long[pipe], aw);
    }
    else {
        memcpy(addr, dev->params.urxaddr.arxaddr.rx_addr_long[NRF24L01P_P1], aw);
        addr[aw - 1] = dev->params.urxaddr.arxaddr.rx_addr_short[pipe - 2];
    }
    return aw;
}

int nrf24l01p_set_max_retransm(nrf24l01p_t *dev, uint8_t max_rt)
{
    assert(dev);
    if (max_rt > NRF24L01P_MAX_RETRANSMISSIONS) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return -ENOTSUP;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8_setup_retr_t setup_retr = NRF24L01P_FLG_ARC(max_rt);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG_SETUP_RETR, NRF24L01P_MSK_ARC,
                       &setup_retr);
    nrf24l01p_release(dev);
    dev->params.config.cfg_max_retr = max_rt;
    return 0;
}

uint8_t nrf24l01p_get_max_retransm(const nrf24l01p_t *dev)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return 0;
    }
    return dev->params.config.cfg_max_retr;
}

int nrf24l01p_set_retransm_delay(nrf24l01p_t *dev, nrf24l01p_ard_t rt_delay)
{
    assert(dev);
    if (rt_delay >= NRF24L01P_ARD_NUM_OF) {
        return -EINVAL;
    }
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return -ENOTSUP;
    }
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_reg8_setup_retr_t setup_retr = NRF24L01P_FLG_ARD(rt_delay);
    nrf24l01p_acquire(dev);
    nrf24l01p_reg8_mod(dev, NRF24L01P_REG_SETUP_RETR, NRF24L01P_MSK_ARD,
                       &setup_retr);
    nrf24l01p_release(dev);
    dev->params.config.cfg_retr_delay = rt_delay;
    return 0;
}

uint16_t nrf24l01p_get_retransm_delay(const nrf24l01p_t *dev,
                                      nrf24l01p_ard_t *rt_delay)
{
    assert(dev);
    if (dev->params.config.cfg_protocol == NRF24L01P_PROTOCOL_SB) {
        return 0;
    }
    if (rt_delay) {
        *rt_delay = dev->params.config.cfg_retr_delay;
    }
    return nrf24l01p_etoval_ard(dev->params.config.cfg_retr_delay);
}

int nrf24l01p_set_state(nrf24l01p_t *dev, nrf24l01p_state_t state)
{
    switch (dev->state) {
        case NRF24L01P_STATE_POWER_DOWN:
        case NRF24L01P_STATE_STANDBY_1:
        case NRF24L01P_STATE_RX_MODE:
            break;
        default:
            return -EAGAIN;
    }
    nrf24l01p_state_t old = dev->state;
    nrf24l01p_acquire(dev);
    if (state == NRF24L01P_STATE_POWER_DOWN) {
        if (dev->state != NRF24L01P_STATE_POWER_DOWN) {
            nrf24l01p_transition_to_power_down(dev);
        }
    }
    else if (state == NRF24L01P_STATE_STANDBY_1) {
        if (dev->state != NRF24L01P_STATE_STANDBY_1) {
            nrf24l01p_transition_to_standby_1(dev);
        }
    }
    else if (state == NRF24L01P_STATE_RX_MODE) {
        if (dev->state != NRF24L01P_STATE_RX_MODE) {
            if (state != NRF24L01P_STATE_STANDBY_1) {
                nrf24l01p_transition_to_standby_1(dev);
            }
            nrf24l01p_transition_to_rx_mode(dev);
        }
    }
    else {
        nrf24l01p_release(dev);
        return -ENOTSUP;
    }
    nrf24l01p_release(dev);
    return (int)old;
}

nrf24l01p_state_t nrf24l01p_get_state(const nrf24l01p_t *dev)
{
    assert(dev);
    return dev->state;
}

void nrf24l01p_print_all_regs(nrf24l01p_t *dev)
{
    nrf24l01p_acquire(dev);
    nrf24l01p_diagnostics_print_all_regs(dev);
    nrf24l01p_release(dev);
}

void nrf24l01p_print_dev_info(const nrf24l01p_t *dev)
{
    nrf24l01p_diagnostics_print_dev_info(dev);
}
