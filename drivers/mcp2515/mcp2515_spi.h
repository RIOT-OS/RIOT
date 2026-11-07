/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mcp2515
 * @brief       Driver for the Microchip MCP2515 can controller.
 *
 * @{
 *
 * @file
 * @brief       Definition of the MCP2515 SPI driver.
 *
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#include <stdint.h>
#include "candev_mcp2515.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize SPI interface
 *
 * The device descriptor contains all information related to the SPI interface.
 *
 * @param[out] dev          device descriptor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_init(const candev_mcp2515_t *dev);

/**
 * @brief Reset MCP2515 device though SPI interface
 *
 * The MCP2515 device is reset by sending the right message over SPI interface.
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_reset(const candev_mcp2515_t *dev);

/**
 * @brief Read the register value corresponding to @p addr
 *
 * @param[in]  dev          device descriptor
 * @param[in]  addr         register addr
 * @param[out] buf          buffer to receive register value
 * @param[in]  len          length of register value
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_read(const candev_mcp2515_t *dev, uint8_t addr, uint8_t *buf,
                     unsigned int len);

/**
 * @brief Read the can data received in the rx @p mailbox
 *
 * @param[in]  dev          device descriptor
 * @param[in]  mailbox      rx mailbox
 * @param[out] buf          buffer to receive can data
 * @param[in]  len          length of can data
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_read_rxbuf(const candev_mcp2515_t *dev, uint8_t mailbox,
                           void *buf, uint8_t len);

/**
 * @brief Send the register value corresponding to @p addr
 *
 * @param[in]  dev          device descriptor
 * @param[in]  addr         address to write
 * @param[in]  buf          buffer containing register data
 * @param[in]  len          buffer length
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_write(const candev_mcp2515_t *dev, uint8_t addr, uint8_t *buf,
                      unsigned int len);

/**
 * @brief Send the can data to the tx @p mailbox
 *
 * @param[in]  dev          device descriptor
 * @param[in]  mailbox      tx mailbox
 * @param[in]  buf          buffer containing can data
 * @param[in]  len          buffer length
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_write_txbuf(const candev_mcp2515_t *dev, uint8_t mailbox,
                            void *buf, uint8_t len);

/**
 * @brief Initiate message transmission.
 *
 * The RTS command can be used to initiate message transmission for one or more
 * of the transmit buffers.
 *
 * @param[in]  dev          device descriptor
 * @param[in]  mailbox      mailbox to enable transmission
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_rts(const candev_mcp2515_t *dev, uint8_t mailbox);

/**
 * @brief Read MCP2515 status over SPI interface.
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  the read status
 */
uint8_t mcp2515_spi_read_status(const candev_mcp2515_t *dev);

/**
 * @brief Read MCP2515 receive status over SPI interface.
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_rx_status(const candev_mcp2515_t *dev);

/**
 * @brief Bit modify instruction.
 *
 * The Bit Modify instruction provides a means for setting or clearing
 * individual bits in specific status and control registers.
 * This command is not available for all registers. Executing this command on
 * registers that are not bit-modifiable will force the mask to FFh.
 *
 * @param[in]  dev          device descriptor
 * @param[in]  addr         register address
 * @param[in]  mask         mask to modify individual bit
 * @param[in]  buf          register value
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_spi_bitmod(const candev_mcp2515_t *dev, uint8_t addr, uint8_t mask,
                       uint8_t buf);

#ifdef __cplusplus
}
#endif

/** @} */
