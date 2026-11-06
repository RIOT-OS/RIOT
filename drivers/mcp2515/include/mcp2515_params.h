/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_mcp2515
 * @{
 *
 * @file
 * @brief       Parameters for the CAN driver implementation
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Wouter Symons <wosym@airsantelmo.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "can/device.h"
#include "candev_mcp2515.h"

#include "board.h"

/**
 * @name    Set default configuration parameters for the MCP2515
 * @{
 */
#ifndef MCP2515_PARAM_SPI
#define MCP2515_PARAM_SPI SPI_DEV(0)
#endif

#ifndef MCP2515_PARAM_SPI_MODE
#define MCP2515_PARAM_SPI_MODE SPI_MODE_0
#endif

#ifndef MCP2515_PARAM_SPI_CLK
#define MCP2515_PARAM_SPI_CLK SPI_CLK_10MHZ
#endif

#ifndef MCP2515_PARAM_CS
#define MCP2515_PARAM_CS GPIO_PIN(1, 9)
#endif

#ifndef MCP2515_PARAM_RST
#define MCP2515_PARAM_RST GPIO_UNDEF
#endif

#ifndef MCP2515_PARAM_INT
#define MCP2515_PARAM_INT GPIO_PIN(1, 8)
#endif

#ifndef MCP2515_PARAM_CLK
#define MCP2515_PARAM_CLK (8000000ul)       /**< External clock frequency */

#endif

#define MCP2515_DEFAULT_CONFIG \
{ \
    .spi = MCP2515_PARAM_SPI, \
    .spi_mode = MCP2515_PARAM_SPI_MODE, \
    .spi_clk =MCP2515_PARAM_SPI_CLK, \
    .cs_pin = MCP2515_PARAM_CS, \
    .rst_pin = MCP2515_PARAM_RST, \
    .int_pin = MCP2515_PARAM_INT, \
    .clk = MCP2515_PARAM_CLK, \
}
/** @} */

/**
 * @brief   Set default configuration
 */
static const candev_mcp2515_conf_t candev_mcp2515_conf[] = {
    MCP2515_DEFAULT_CONFIG
};

/**
 * @brief   set candev parameters
 */
static const candev_params_t candev_mcp2515_params[] = {
    {
        .name = "can_mcp2515_0",
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
