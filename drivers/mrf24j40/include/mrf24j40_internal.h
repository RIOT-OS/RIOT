/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup drivers_mrf24j40
 * @{
 *
 * @file
 * @brief Internal interfaces for MRF24J40 drivers
 *
 * @author      Neo Nenaco <neo@nenaco.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef MRF24J40_INTERNAL_H
#define MRF24J40_INTERNAL_H

#include <stdint.h>

#include "mrf24j40.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialization as described in datasheet
 *
 * @param[in] dev device to initialize
 *
 * @return 0 on success, error otherwise
 */
int mrf24j40_init_hw(mrf24j40_t *dev);

/**
 * @brief Read from a register with a  at address `addr` from device `dev`. Register with 8bit address
 *
 * @param[in] dev device to read from
 * @param[in] addr address of the register to read
 *
 * @return the value of the specified register
 */
uint8_t mrf24j40_reg_read_short(mrf24j40_t *dev, const uint8_t addr);

/**
 * @brief Write to a register at address `addr` from device `dev`. Register with 8bit address
 *
 * @param[in] dev device to write to
 * @param[in] addr address of the register to write
 * @param[in] value value to write to the given register
 */
void mrf24j40_reg_write_short(mrf24j40_t *dev, const uint8_t addr, const uint8_t value);

/**
 * @brief Read from a register with a  at address `addr` from device `dev`. Register with 10bit address
 *
 * @param[in] dev device to read from
 * @param[in] addr address of the register to read
 *
 * @return the value of the specified register
 */
uint8_t mrf24j40_reg_read_long(mrf24j40_t *dev, const uint16_t addr);

/**
 * @brief Write to a register at address `addr` from device `dev`. Register with 10bit address
 *
 * @param[in] dev device to write to
 * @param[in] addr address of the register to write
 * @param[in] value value to write to the given register
 */
void mrf24j40_reg_write_long(mrf24j40_t *dev, const uint16_t addr, const uint8_t value);

/**
 * @brief   Write a chunk of data into the TX Normal FIFO area of the given device
 *
 * @param[in] dev       device to write to
 * @param[in] offset    address in the TX Normal FIFO to write to [valid 0x00-0x1ff]
 * @param[in] data      data to copy into FIFO
 * @param[in] len       number of bytes to write to FIFO
 */
void mrf24j40_tx_normal_fifo_write(mrf24j40_t *dev, const uint16_t offset, const uint8_t *data, const size_t len);

/**
 * @brief   Read a chunk of data from the RX_FIFO area of the given device
 *
 * @param[in]  dev      device to read from
 * @param[in]  offset   starting address to read from [valid 0x00-0x1ff]
 * @param[out] data     buffer to read data into
 * @param[in]  len      number of bytes to read from FIFO
 */
void mrf24j40_rx_fifo_read(mrf24j40_t *dev, const uint16_t offset, uint8_t *data, const size_t len);

/**
 * @brief   Reset the pending task list of a device
 *
 * @param[in] dev       device to reset tasks of
 */
void mrf24j40_reset_tasks(mrf24j40_t *dev);

/**
 * @brief   Check for pending interrupts and update task list
 *
 * @param[in] dev       device to read
 */
void mrf24j40_update_tasks(mrf24j40_t *dev);

/**
 * @brief   Trigger a hardware reset
 *
 * @param[in] dev       device to reset
 */
void mrf24j40_hardware_reset(mrf24j40_t *dev);

/**
 * @brief   Enable automatic External Power Amplifier & Low Noise Amplifier control
 *
 * @param[in] dev       device to enable the PA & LNA on
 */
#if IS_ACTIVE(CONFIG_MRF24J40_USE_EXT_PA_LNA)
void mrf24j40_enable_auto_pa_lna(mrf24j40_t *dev);
#else
static inline void mrf24j40_enable_auto_pa_lna(mrf24j40_t *dev) { (void) dev; }
#endif

/**
 * @brief   Disable automatic External Power Amplifier & Low Noise Amplifier control
 *
 * @param[in] dev       device to disable the PA & LNA on
 */
#if IS_ACTIVE(CONFIG_MRF24J40_USE_EXT_PA_LNA)
void mrf24j40_disable_auto_pa_lna(mrf24j40_t *dev);
#else
static inline void mrf24j40_disable_auto_pa_lna(mrf24j40_t *dev) { (void) dev; }
#endif

/**
 * @brief   Enable only the External Low Noise Amplifier
 *
 * @param[in] dev       device enable the LNA on
 */
#if IS_ACTIVE(CONFIG_MRF24J40_USE_EXT_PA_LNA)
void mrf24j40_enable_lna(mrf24j40_t *dev);
#else
static inline void mrf24j40_enable_lna(mrf24j40_t *dev) { (void) dev; }
#endif

/**
 * @brief   Flush the RX FIFO
 *
 * @param[in] dev       device to flush the RX FIFO
 */
void mrf24j40_flush_rx(mrf24j40_t *dev);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MRF24J40_INTERNAL_H */
