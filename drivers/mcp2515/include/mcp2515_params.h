#ifndef MCP2515_PARAMS_H
#define MCP2515_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "can/device.h"
#include "candev_mcp2515.h"

#include "board.h"

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
#define MCP2515_PARAM_CS SPI_HWCS(0)
#endif

#ifndef MCP2515_PARAM_RST
#define MCP2515_PARAM_RST GPIO_PIN(0, 0)
#endif

#ifndef MCP2515_PARAM_INT
#define MCP2515_PARAM_INT GPIO_PIN(0, 1)
#endif

#ifndef MCP2515_PARAM_CLK
#define MCP2515_PARAM_CLK (8000000ul)
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

const static candev_mcp2515_conf_t candev_mcp2515_conf[] = {
    MCP2515_DEFAULT_CONFIG
};

const static candev_params_t candev_mcp2515_params[] = {
    {
        .name = "can_cmp2515_0",
    },
};

#ifdef __cplusplus
}
#endif

#endif /* MCP2515_PARAMS_H */
