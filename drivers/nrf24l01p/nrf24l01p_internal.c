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
 * @brief   Implementation of internal helper functions for
 *          the NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include <string.h>

#include "nrf24l01p_registers.h"
#include "nrf24l01p_communication.h"
#include "nrf24l01p_conversion.h"
#include "nrf24l01p_internal.h"

int nrf24l01p_assert_awake(const nrf24l01p_t *dev)
{
    nrf24l01p_reg8_config_t config = 0;

    nrf24l01p_read_reg(dev, NRF24L01P_REG_CONFIG, &config, 1);
    return config & NRF24L01P_FLG_PWR_UP;
}

void nrf24l01p_restore_address(const nrf24l01p_t *dev)
{
    nrf24l01p_reg8_setup_aw_t setup_aw =
        NRF24L01P_FLG_AW(dev->params.config.cfg_addr_width);
    uint8_t addr_buffer[NRF24L01P_MAX_ADDR_WIDTH];
    uint8_t aw = nrf24l01p_etoval_aw(dev->params.config.cfg_addr_width);

    memcpy(addr_buffer, dev->params.urxaddr.rxaddrpx.rx_pipe_0_addr, aw);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_RX_ADDR_P0, addr_buffer, aw);
    nrf24l01p_write_reg(dev, NRF24L01P_REG_SETUP_AW, &setup_aw, 1);
}
