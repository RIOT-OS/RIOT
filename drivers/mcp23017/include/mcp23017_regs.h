/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp23017
 * @{
 *
 * @file
 * @brief       Register and bit definitions for the MCP23017
 *
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 */

#ifndef MCP23017_REGS_H
#define MCP23017_REGS_H

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Register addresses (IOCON.BANK = 0)
 * @{
 */
#define MCP23017_REG_IODIRA     (0x00)
#define MCP23017_REG_IODIRB     (0x01)
#define MCP23017_REG_IPOLA      (0x02)
#define MCP23017_REG_IPOLB      (0x03)
#define MCP23017_REG_GPINTENA   (0x04)
#define MCP23017_REG_GPINTENB   (0x05)
#define MCP23017_REG_DEFVALA    (0x06)
#define MCP23017_REG_DEFVALB    (0x07)
#define MCP23017_REG_INTCONA    (0x08)
#define MCP23017_REG_INTCONB    (0x09)
#define MCP23017_REG_IOCONA     (0x0A)
#define MCP23017_REG_IOCONB     (0x0B)
#define MCP23017_REG_GPPUA      (0x0C)
#define MCP23017_REG_GPPUB      (0x0D)
#define MCP23017_REG_INTFA      (0x0E)
#define MCP23017_REG_INTFB      (0x0F)
#define MCP23017_REG_INTCAPA    (0x10)
#define MCP23017_REG_INTCAPB    (0x11)
#define MCP23017_REG_GPIOA      (0x12)
#define MCP23017_REG_GPIOB      (0x13)
#define MCP23017_REG_OLATA      (0x14)
#define MCP23017_REG_OLATB      (0x15)
/** @} */

/**
 * @name Bit definition for register
 * @{
 */
#define PIN_0                   (1 << 0)
#define PIN_1                   (1 << 1)
#define PIN_2                   (1 << 2)
#define PIN_3                   (1 << 3)
#define PIN_4                   (1 << 4)
#define PIN_5                   (1 << 5)
#define PIN_6                   (1 << 6)
#define PIN_7                   (1 << 7)
/** @} */

/**
 * @name Port definition of MCP23017
 * @{
 */
#define PORT_A                  (0)
#define PORT_B                  (1)
/** @} */

/**
 * @name Pin direction definition of MCP23017
 * @{
 */
#define INPUT                   (1)
#define OUTPUT                  (0)
/** @} */

/**
 * @name Pin pull-up definition of MCP23017
 * @{
 */
#define PULL_UP_EN              (1)
#define PULL_UP_DIS             (0)
/** @} */

/**
 * @name Pin output definition of MCP23017
 * @{
 */
#define HIGH                    (1)
#define LOW                     (0)
/** @} */

/**
 * @name Pin interrupt definition of MCP23017
 * @{
 */
#define ENABLE                  (1)
#define DISABLE                 (0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MCP23017_REGS_H */
/** @} */
