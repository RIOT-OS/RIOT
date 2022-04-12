/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mcp2515 MCP2515
 * @ingroup     drivers_can
 * @brief       Driver for the Microchip MCP2515 can controller.
 *
 * @{
 *
 * @file
 * @brief       Definition of the implementation of the CAN controller driver.
 *
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Wouter Symons <wosym@airsantelmo.com>
 */

#ifndef CANDEV_MCP2515_H
#define CANDEV_MCP2515_H

#include <stdbool.h>

#include "can/candev.h"
#include "cpu_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "mutex.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Default CAN bitrate
 */
#ifndef CANDEV_MCP2515_DEFAULT_BITRATE
#define CANDEV_MCP2515_DEFAULT_BITRATE 500000
#endif

/**
 * Default sampling point setup
 */
#ifndef CANDEV_MCP2515_DEFAULT_SPT
#define CANDEV_MCP2515_DEFAULT_SPT 875
#endif

/**
 * Number of transmit mailboxes
 */
#define MCP2515_TX_MAILBOXES 3

/**
 * @name Receive mailboxes and filters number
 * @{
 * for RX buffers: the MCP2515 managed 6 acceptance filters in 2 mailboxes:
 *  - MB0 contains 2 acceptance filters in relation with 1 acceptance mask
 *  - MB1 contains 4 acceptance filters in relation with 1 acceptance mask
 *
 *                    MB0          MB1
 *                  +------+    +------+
 *          mask 0  | RXM0 |    | RXM1 | mask 1
 *                  +======+    +======+
 *         filter 0 | RXF0 |    | RXF2 | filter 2
 *                  +------+    +------+
 *         filter 1 | RXF1 |    | RXF3 | filter 3
 *                  +------+    +------+
 *                              | RXF4 | filter 4
 *                              +------+
 *                              | RXF5 | filter 5
 *                              +------+
 */
#define MCP2515_RX_MAILBOXES 2
#define MCP2515_FILTERS_MB0 2
#define MCP2515_FILTERS_MB1 4
#define MCP2515_FILTERS (MCP2515_FILTERS_MB0 + MCP2515_FILTERS_MB1)
/** @} */

/** MCP2515 candev descriptor */
typedef struct candev_mcp2515 candev_mcp2515_t;

/**
 * @brief MCP2515 configuration descriptor
 */
typedef struct candev_mcp2515_conf {
    spi_t spi;                  /**< SPI bus */
    spi_mode_t spi_mode;        /**< SPI mode */
    spi_clk_t spi_clk;          /**< SPI clock speed */
    gpio_t cs_pin;              /**< Slave select pin */
    gpio_t rst_pin;             /**< Reset pin */
    gpio_t int_pin;             /**< Interrupt pin */
    uint32_t clk;               /**< External clock frequency */
} candev_mcp2515_conf_t;

/**
 * @brief MCP2515 device descriptor
 */
struct candev_mcp2515 {
    /** candev driver */
    candev_t candev;
    /** driver configuration */
    const candev_mcp2515_conf_t *conf;
    /** tx mailboxes local copy */
    const struct can_frame *tx_mailbox[MCP2515_TX_MAILBOXES];
    /** rx mailboxes local copy */
    struct can_frame rx_buf[MCP2515_RX_MAILBOXES];
    /** masks list */
    uint32_t masks[MCP2515_RX_MAILBOXES];
    /** filters list */
    canid_t filter_ids[MCP2515_RX_MAILBOXES][MCP2515_FILTERS_MB1];
    /** wakeup source */
    int wakeup_src;
};

/**
 * @brief Initialize a mcp2515 device by assigning a @p timing and an SPI
 *        configuration @p conf.
 *
 * @param[out] dev      mcp2515 device descriptor
 * @param[in]  conf     mcp2515 configuration
 */
void candev_mcp2515_init(candev_mcp2515_t *dev,
                         const candev_mcp2515_conf_t *conf);

#ifdef __cplusplus
}
#endif

#endif /* CANDEV_MCP2515_H */
/** @} */
