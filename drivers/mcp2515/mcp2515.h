/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp2515
 * @brief       Driver for the Microchip MCP2515 can controller.
 *
 * @{
 *
 * @file
 * @brief       Definition of the MCP2515 CAN controller driver.
 *
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 */
#ifndef MCP2515_H
#define MCP2515_H

#include "mcp2515_defines.h"
#include "candev_mcp2515.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MCP2515 mode
 */
enum mcp2515_mode {
    MODE_NORMAL         = MCP2515_CANSTAT_OPMOD_NORMAL,
    MODE_SLEEP          = MCP2515_CANSTAT_OPMOD_SLEEP,
    MODE_LOOPBACK       = MCP2515_CANSTAT_OPMOD_LOOPBACK,
    MODE_LISTEN_ONLY    = MCP2515_CANSTAT_OPMOD_LISTEN_ONLY,
    MODE_CONFIG         = MCP2515_CANSTAT_OPMOD_CONFIGURATION,
    MODE_UNKNOWN        = -1
};

/**
 * @brief MCP2515 interrupt
 */
enum mcp2515_interrupt {
    INT_RX0             = MCP2515_CANINTF_RX0IF,
    INT_RX1             = MCP2515_CANINTF_RX1IF,
    INT_TX0             = MCP2515_CANINTF_TX0IF,
    INT_TX1             = MCP2515_CANINTF_TX1IF,
    INT_TX2             = MCP2515_CANINTF_TX2IF,
    INT_ERROR           = MCP2515_CANINTF_ERRIF,
    INT_WAKEUP          = MCP2515_CANINTF_WAKIF,
    INT_MESSAGE_ERROR   = MCP2515_CANINTF_MERRF,
};

/**
 * @brief MCP2515 error
 */
enum mcp2515_error {
    ERR_WARNING         = MCP2515_EFLG_EWARN,
    ERR_RX_WARNING      = MCP2515_EFLG_RXWAR,
    ERR_TX_WARNING      = MCP2515_EFLG_TXWAR,
    ERR_RX_PASSIVE      = MCP2515_EFLG_RXEP,
    ERR_TX_PASSIVE      = MCP2515_EFLG_TXEP,
    ERR_TX_BUS_OFF      = MCP2515_EFLG_TXBO,
    ERR_RX_0_OVERFLOW   = MCP2515_EFLG_RX0OVR,
    ERR_RX_1_OVERFLOW   = MCP2515_EFLG_RX1OVR,
};

/** Wake up source */
#define MCP2515_WKUP_SRC_INTERNAL 1

/** Acceptance mode (enable/disable filtering) */
#ifndef MCP2515_RECV_FILTER_EN
#define MCP2515_RECV_FILTER_EN  0
#endif

/**
 * @brief Initialize pins and SPI interface
 *
 * The device descriptor contains all information related to pins and SPI
 * interface. This function initialize all corresponding fields and relies
 * the @p irq_cb callback function to the pin interruption. The pin
 * interruption should be configured in the device descriptor.
 *
 * @param[out] dev          device descriptor
 * @param[in]  irq_cb       callback function called when an interrupt is raised
 *                          from the MCP2515. The MCP2515 makes the interruption
 *                          through the interruption pin.
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_init(candev_mcp2515_t *dev, void (*irq_cb)(void *));

/**
 * @brief Reset MCP2515 device with dedicated pin
 *
 * The MCP2515 device is reset by toggling the rst pin.
 *
 * @param[in]  dev          device descriptor
 */
void mcp2515_reset(candev_mcp2515_t *dev);

/**
 * @brief Initialize MCP2515 interrupts
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_init_irqs(candev_mcp2515_t *dev);

/**
 * @brief Send frame through the corresponding tx @p mailbox.
 *
 * @param[in]  dev          device descriptor
 * @param[in]  frame        the frame to send
 * @param[in]  mailbox      tx mailbox
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_send(candev_mcp2515_t *dev, const struct can_frame *frame,
                 int mailbox);

/**
 * @brief Receive frame from the corresponding rx @p mailbox.
 *
 * @param[in]  dev          device descriptor
 * @param[out] frame        the receive frame
 * @param[in]  mailbox      rx mailbox
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_receive(candev_mcp2515_t *dev, struct can_frame *frame,
                    int mailbox);

/**
 * @brief Abort communication.
 *
 * @param[in]  dev          device descriptor
 * @param[in]  mailbox      mailbox
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_abort(candev_mcp2515_t *dev, int mailbox);

/**
 * @brief Get MCP2515 mode of operation.
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  mcp2515_mode enum
 */
enum mcp2515_mode mcp2515_get_mode(candev_mcp2515_t *dev);

/**
 * @brief Set MCP2515 mode of operation.
 *
 * @param[in]  dev          device descriptor
 * @param[in]  mode         mode of operation to set
 *
 * @return                  The mode actually set
 */
enum mcp2515_mode mcp2515_set_mode(candev_mcp2515_t *dev,
                                   enum mcp2515_mode mode);

/**
 * @brief Wake up MCP2515
 *
 * @param[in] dev           device descriptor
 */
void mcp2515_wake_up(candev_mcp2515_t *dev);

/**
 * @brief Get MCP2515 interrupt type.
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  mcp2515_interrupt enum
 */
enum mcp2515_interrupt mcp2515_get_irq(candev_mcp2515_t *dev);

/**
 * @brief Clear MCP2515 interrupt.
 *
 * @param[in]  dev          device descriptor
 * @param[in]  irq          interrupt to clear
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_clear_irq(candev_mcp2515_t *dev, enum mcp2515_interrupt irq);

/**
 * @brief Get if an tx error occurred on MCP2515.
 *
 * @param[in]  dev          device descriptor
 * @param[in]  mailbox      tx mailbox
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_tx_err_occurred(candev_mcp2515_t *dev, int mailbox);

/**
 * @brief Configure the bit timing of the MCP2515.
 *
 * The information about the bit timing should be contained in dev descriptor.
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_configure_bittiming(candev_mcp2515_t *dev);

/**
 * @brief Get the error flags.
 *
 * @param[in]  dev          device descriptor
 *
 * @return                  EFLG error flags
 */
uint8_t mcp2515_get_errors(candev_mcp2515_t *dev);

/**
 * @brief Set the @ filter_id to the position in the mailbox.
 *
 * @p filter_id corresponds to the position in the MCP2515 mailbox as follow:
 *  - [0; 1]: mailbox RXB0
 *  - [2; 5]: mailbox RXB1
 *
 * The MCP2515 managed 6 acceptance filters in 2 rx mailboxes:
 *  - MB0 contains 2 acceptance filters in relation with 1 acceptance mask
 *  - MB1 contains 4 acceptance filters in relation with 1 acceptance mask
 *
 *            MB0          MB1
 *          +------+    +------+
 *  mask 0  | RXM0 |    | RXM1 | mask 1
 *          +======+    +======+
 * filter 0 | RXF0 |    | RXF2 | filter 2
 *          +------+    +------+
 * filter 1 | RXF1 |    | RXF3 | filter 3
 *          +------+    +------+
 *                      | RXF4 | filter 4
 *                      +------+
 *                      | RXF5 | filter 5
 *                      +------+
 *
 * @param[in]  dev          device descriptor
 * @param[in]  filter_id    filter identifier in the MCP2515 mailbox
 * @param[in]  filter       acceptance filter can identifier
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_set_filter(candev_mcp2515_t *dev, int filter_id, uint32_t filter);

/**
 * @brief Set the @ mask to the mailbox.
 *
 * The MCP2515 managed 6 acceptance filters in 2 rx mailboxes:
 *  - MB0 contains 2 acceptance filters in relation with 1 acceptance mask
 *  - MB1 contains 4 acceptance filters in relation with 1 acceptance mask
 *
 *            MB0          MB1
 *          +------+    +------+
 *  mask 0  | RXM0 |    | RXM1 | mask 1
 *          +======+    +======+
 * filter 0 | RXF0 |    | RXF2 | filter 2
 *          +------+    +------+
 * filter 1 | RXF1 |    | RXF3 | filter 3
 *          +------+    +------+
 *                      | RXF4 | filter 4
 *                      +------+
 *                      | RXF5 | filter 5
 *                      +------+
 *
 * @param[in]  dev          device descriptor
 * @param[in]  mailbox      rx mailbox
 * @param[in]  mask         acceptance mask
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int mcp2515_set_mask(candev_mcp2515_t *dev, int mailbox, uint32_t mask);

#ifdef __cplusplus
}
#endif

#endif /* MCP2515_H */
/** @} */
