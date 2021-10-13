/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at25xxx
 * @{
 *
 * @file
 * @brief       Commands for the AT25xxx family of SPI-EEPROMs
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#ifndef AT25XXX_CONSTANTS_H
#define AT25XXX_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

#define CMD_WREN    (0x6)   /**< Write Enable             */
#define CMD_WRDI    (0x4)   /**< Write Disable            */
#define CMD_RDSR    (0x5)   /**< Read Status Register     */
#define CMD_WRSR    (0x1)   /**< Write Status Register    */
#define CMD_READ    (0x3)   /**< Read from Memory Array   */
#define CMD_WRITE   (0x2)   /**< Write to Memory Array    */

#define SR_WIP      (0x01)  /**< Write In Progress        */
#define SR_WEL      (0x02)  /**< Write Enable Latch       */
#define SR_BP0      (0x04)  /**< Block Protect 0          */
#define SR_BP1      (0x08)  /**< Block Protect 1          */
#define SR_SRWD     (0x80)  /**< Status Register Write Disable */

#ifdef __cplusplus
}
#endif

#endif /* AT25XXX_CONSTANTS_H */
