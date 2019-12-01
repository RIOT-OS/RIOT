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
 * @brief   Implementation of register modification functions
 *          for the NRF24L01P transceiver
 *
 * @author Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */
#include "nrf24l01p_communication.h"
#include "nrf24l01p_registers.h"

uint8_t nrf24l01p_reg8_set(const nrf24l01p_t *dev, uint8_t reg_addr, uint8_t *reg_val)
{
    uint8_t reg_val_old;

    nrf24l01p_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    *reg_val = reg_val_old | *reg_val;
    return nrf24l01p_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}

uint8_t nrf24l01p_reg8_clear(const nrf24l01p_t *dev, uint8_t reg_addr,
                             uint8_t *reg_val)
{
    uint8_t reg_val_old;

    nrf24l01p_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    *reg_val = reg_val_old &= ~(*reg_val);
    return nrf24l01p_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}

uint8_t nrf24l01p_reg8_mod(const nrf24l01p_t *dev, uint8_t reg_addr,
                           uint8_t mask, uint8_t *reg_val)
{
    uint8_t reg_val_old;

    nrf24l01p_read_reg(dev, reg_addr, &reg_val_old, sizeof(reg_val_old));
    reg_val_old &= ~mask;
    *reg_val = reg_val_old | *reg_val;
    return nrf24l01p_write_reg(dev, reg_addr, reg_val, sizeof(*reg_val));
}
