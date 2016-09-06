/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    ethernet_phy Ethernet PHY
 * @brief       Provides a Ethernet PHY abstraction
 * @{
 *
 * @file
 * @brief       Definitions for Ethernet PHY devices
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 */

#ifndef ETH_PHY_H
#define ETH_PHY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Ethernet PHY Common Registers */
#define PHY_BMCR                           (0x00)
#define PHY_BSMR                           (0x01)
#define PHY_PHYIDR1                        (0x02)
#define PHY_PHYIDR2                        (0x03)
#define PHY_ANAR                           (0x04)
#define PHY_ANLPAR                         (0x05)
#define PHY_ANER                           (0x06)
#define PHY_ANNPTR                         (0x07)

/* Ethernet PHY BMCR Fields */
#define BMCR_RESET                         (0x8000)
#define BMCR_LOOPBACK                      (0x4000)
#define BMCR_SPEED_SELECT                  (0x2000)
#define BMCR_AN                            (0x1000)
#define BMCR_POWER_DOWN                    (0x0800)
#define BMCR_ISOLATE                       (0x0400)
#define BMCR_RESTART_AN                    (0x0200)
#define BMCR_DUPLEX_MODE                   (0x0100)
#define BMCR_COLLISION_TEST                (0x0080)

/* Ethernet PHY BSMR Fields */
#define BSMR_100BASE_T4                    (0x8000)
#define BSMR_100BASE_TX_FDUPLEX            (0x4000)
#define BSMR_100BASE_TX_HDUPLEX            (0x2000)
#define BSMR_10BASE_T_FDUPLEX              (0x1000)
#define BSMR_10BASE_T_HDUPLEX              (0x0800)
#define BSMR_NO_PREAMBLE                   (0x0040)
#define BSMR_AN_COMPLETE                   (0x0020)
#define BSMR_REMOTE_FAULT                  (0x0010)
#define BSMR_AN_ABILITY                    (0x0008)
#define BSMR_LINK_STATUS                   (0x0004)
#define BSMR_JABBER_DETECT                 (0x0002)
#define BSMR_EXTENDED_CAP                  (0x0001)

/* Ethernet PHY PHYIDR1 Fields */
#define PHYIDR1_OUI                        (0xffff)

/* Ethernet PHY PHYIDR2 Fields */
#define PHYIDR2_OUI                        (0xfe00)
#define PHYIDR2_MODEL                      (0x01f0)
#define PHYIDR2_REV                        (0x0007)

/* Ethernet PHY ANAR Fields */
#define ANAR_NEXT_PAGE                     (0x8000)
#define ANAR_REMOTE_FAULT                  (0x2000)
#define ANAR_PAUSE                         (0x0600)
#define ANAR_100BASE_T4                    (0x0200)
#define ANAR_100BASE_TX_FDUPLEX            (0x0100)
#define ANAR_100BASE_TX_HDUPLEX            (0x0080)
#define ANAR_10BASE_T_FDUPLEX              (0x0040)
#define ANAR_10BASE_T_HDUPLEX              (0x0020)
#define ANAR_SELECTOR                      (0x000f)

/* Ethernet PHY ANLPAR Fields */
#define ANLPAR_NEXT_PAGE                   (0x8000)
#define ANLPAR_ACK                         (0x4000)
#define ANLPAR_REMOTE_FAULT                (0x2000)
#define ANLPAR_PAUSE                       (0x0600)
#define ANLPAR_100BASE_T4                  (0x0200)
#define ANLPAR_100BASE_TX_FDUPLEX          (0x0100)
#define ANLPAR_100BASE_TX_HDUPLEX          (0x0080)
#define ANLPAR_10BASE_T_FDUPLEX            (0x0040)
#define ANLPAR_10BASE_T_HDUPLEX            (0x0020)
#define ANLPAR_SELECTOR                    (0x000f)

/* Ethernet PHY ANNPTR Fields */
#define ANNPTR_NEXT_PAGE                   (0x8000)
#define ANNPTR_MSG_PAGE                    (0x2000)
#define ANNPTR_ACK2                        (0x1000)
#define ANNPTR_TOGGLE_TX                   (0x0800)
#define ANNPTR_CODE                        (0x03ff)

/* Ethernet PHY ANER Fields */
#define ANER_PDF                           (0x0010)
#define ANER_LP_NEXT_PAGE_ABLE             (0x0008)
#define ANER_NEXT_PAGE_ABLE                (0x0004)
#define ANER_PAGE_RX                       (0x0002)
#define ANER_LP_AN_ABLE                    (0x0001)

/**
 * @brief Read a PHY register
 *
 * @param[in] addr      address of the PHY to read
 * @param[in] reg       register to be read
 *
 * @return value in the register, or <=0 on error
 */
int32_t eth_phy_read(uint16_t addr, uint8_t reg);

/**
 * @brief Write a PHY register
 *
 * @param[in] addr      address of the PHY to write
 * @param[in] reg       register to be written
 * @param[in] value     value to write into the register
 *
 * @return 0 in case of success or <=0 on error
 */
int32_t eth_phy_write(uint16_t addr, uint8_t reg, uint16_t value);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* ETH_PHY_H */
