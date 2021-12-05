/*
 * Copyright (C) 2021 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_mcp23x17
 * @brief       Register definitions for Microchip MCP23x17 I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Register addresses (IOCON.BANK = 0)
 * @{
 */
#define MCP23X17_REG_IODIR      (0x00)  /**< I/O direction register pair */
#define MCP23X17_REG_IODIRA     (0x00)  /**< I/O direction register port A */
#define MCP23X17_REG_IODIRB     (0x01)  /**< I/O direction register port B */
#define MCP23X17_REG_IPOL       (0x02)  /**< Input polarity port register pair */
#define MCP23X17_REG_IPOLA      (0x02)  /**< Input polarity port register port A */
#define MCP23X17_REG_IPOLB      (0x03)  /**< Input polarity port register port B */
#define MCP23X17_REG_GPINTEN    (0x04)  /**< Interupt-on-change enabled register pair */
#define MCP23X17_REG_GPINTENA   (0x04)  /**< Interupt-on-change enabled register port A */
#define MCP23X17_REG_GPINTENB   (0x05)  /**< Interupt-on-change enabled register port B */
#define MCP23X17_REG_DEFVAL     (0x06)  /**< Default value register pair */
#define MCP23X17_REG_DEFVALA    (0x06)  /**< Default value register port A */
#define MCP23X17_REG_DEFVALB    (0x07)  /**< Default value register port B */
#define MCP23X17_REG_INTCON     (0x08)  /**< Interupt-on-change config register pair */
#define MCP23X17_REG_INTCONA    (0x08)  /**< Interupt-on-change config register port A */
#define MCP23X17_REG_INTCONB    (0x09)  /**< Interupt-on-change config register port B */
#define MCP23X17_REG_IOCON      (0x0A)  /**< I/O expander config register pair */
#define MCP23X17_REG_IOCONA     (0x0A)  /**< I/O expander config register port A */
#define MCP23X17_REG_IOCONB     (0x0B)  /**< I/O expander config register port B */
#define MCP23X17_REG_GPPU       (0x0C)  /**< GPIO pull-up resistor register pair */
#define MCP23X17_REG_GPPUA      (0x0C)  /**< GPIO pull-up resistor register port A */
#define MCP23X17_REG_GPPUB      (0x0D)  /**< GPIO pull-up resistor register port B */
#define MCP23X17_REG_INTF       (0x0E)  /**< Interrupt flag register pair */
#define MCP23X17_REG_INTFA      (0x0E)  /**< Interrupt flag register port A */
#define MCP23X17_REG_INTFB      (0x0F)  /**< Interrupt flag register port B */
#define MCP23X17_REG_INTCAP     (0x10)  /**< Interrupt capture register pair */
#define MCP23X17_REG_INTCAPA    (0x10)  /**< Interrupt capture register port A */
#define MCP23X17_REG_INTCAPB    (0x11)  /**< Interrupt capture register port B */
#define MCP23X17_REG_GPIO       (0x12)  /**< GPIO port register pair */
#define MCP23X17_REG_GPIOA      (0x12)  /**< GPIO port register port A */
#define MCP23X17_REG_GPIOB      (0x13)  /**< GPIO port register port B */
#define MCP23X17_REG_OLAT       (0x14)  /**< Output latch register pair */
#define MCP23X17_REG_OLATA      (0x14)  /**< Output latch register port A */
#define MCP23X17_REG_OLATB      (0x15)  /**< Output latch register port B */
/** @} */

/**
 * @name Register structure definitions
 * @{
 */
/* IOCONA/IOCONB registers */
#define MCP23X17_IOCON_BANK     (0x80)  /**< Registered separated into different banks */
#define MCP23X17_IOCON_MIRROR   (0x40)  /**< INT pins internally connected */
#define MCP23X17_IOCON_SEQOP    (0x20)  /**< Sequential option disabled */
#define MCP23X17_IOCON_DISSLW   (0x10)  /**< Slew rate control for SDA disabled */
#define MCP23X17_IOCON_HAEN     (0x08)  /**< Hardware address enabled */
#define MCP23X17_IOCON_ODR      (0x04)  /**< Open-drain output (overrides INTPOL) */
#define MCP23X17_IOCON_INTPOL   (0x02)  /**< INT signals are active high */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
