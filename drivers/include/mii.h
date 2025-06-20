/*
 * SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_mii     Ethernet Media-Independent Interface (MII)
 *                              Utilities
 * @ingroup     drivers_netdev
 *
 * This module contains constants and helpers as library to help working with
 * Ethernet PHYs connected via the Media-Independent Interface (MII) or the
 * Reduced Media-Independent Interface (RMII)
 *
 * @{
 *
 * @file
 * @brief       Interface definition for MII/RMII h
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdbool.h>
#include <stdint.h>

#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Common MII Management Register Set
 *
 * All registers except @ref MII_BMCR and @ref MII_BMSR are extended registers.
 * Support for extended registers is indicated by the @ref MII_BMSR_EXTENDED
 * bit in the @ref MII_BMSR register.
 *
 * @{
 */
#define MII_BMCR            (0x00U) /**< Basic mode control register */
#define MII_BMSR            (0x01U) /**< Basic mode status register */
#define MII_PHYID1          (0x02U) /**< PHY Identifier 1 */
#define MII_PHYID2          (0x03U) /**< PHY Identifier 2 */
#define MII_ADVERTISE       (0x04U) /**< Auto-Negotiation Advertisement */
#define MII_LPA             (0x05U) /**< Link Parter Abilities */
#define MII_EXPANSION       (0x06U) /**< Auto-Negotiation Expansion */
#define MII_ESTATUS         (0x0fU) /**< Extended Status Register */
#define MII_IRQ             (0x1bU) /**< Interrupt Control/Status */
/** @} */

/**
 * @name    Bits in the MII Basic Mode Control Register
 * @{
 */
#define MII_BMCR_RESET      BIT15   /**< Set to perform PHY reset */
#define MII_BMCR_LOOP       BIT14   /**< Set to enable loopback mode */
#define MII_BMCR_AN_ENABLE  BIT12   /**< Set to enable auto-negotiation */
#define MII_BMCR_POWER_DOWN BIT11   /**< Set to power down PHY */
#define MII_BMCR_ISOLATE    BIT10   /**< Set to electrically isolate PHY from
                                         MII (PHY becomes inoperational) */
#define MII_BMCR_AN_RESTART BIT9    /**< Set to restart auto-negotiation */
#define MII_BMCR_FULL_DPLX  BIT8    /**< Set for full duplex */
#define MII_BMCR_HALF_DPLX  (0)     /**< Set for half duplex */
#define MII_BMCR_COLL_TEST  BIT7    /**< Set to enable collision signal test */
#define MII_BMCR_SPEED_10   (0)     /**< Set speed to 10 Mbps */
#define MII_BMCR_SPEED_100  BIT13   /**< Set speed to 100 Mbps */
#define MII_BMCR_SPEED_1000 BIT6    /**< Set speed to 1 Gbps */
/** @} */

/**
 * @name    Bits in the MII Basic Mode Status Register
 * @{
 */
#define MII_BMSR_100_T4     BIT15   /**< PHY supports 100BASE-T4 (half-duplex) */
#define MII_BMSR_100_TX_F   BIT14   /**< PHY supports 100BASE-TX, full duplex */
#define MII_BMSR_100_TX_H   BIT13   /**< PHY supports 100BASE-TX, half duplex */
#define MII_BMSR_10_F       BIT12   /**< PHY supports 10BASE-T, full duplex */
#define MII_BMSR_10_H       BIT11   /**< PHY supports 10BASE-T, half duplex */
#define MII_BMSR_100_T2_F   BIT10   /**< PHY supports 100BASE-T2, full duplex */
#define MII_BMSR_100_T2_H   BIT9    /**< PHY supports 100BASE-T2, half duplex */
#define MII_BMSR_ESTATUS    BIT8    /**< Set, if @ref MII_ESTATUS is
                                         available */
#define MII_BMSR_AN_DONE    BIT5    /**< Set when auto-negotiation is done */
#define MII_BMSR_FAULT      BIT4    /**< Set when remote fault condition
                                         is detected */
#define MII_BMSR_HAS_AN     BIT3    /**< Set if PHY can auto-negotiate */
#define MII_BMSR_LINK       BIT2    /**< Set if link is up */
#define MII_BMSR_JABBER     BIT1    /**< Set when jabber condition detected */
#define MII_BMSR_EXTENDED   BIT0    /**< Extended MII registers available */
/** @} */

/**
 * @name    Bits in the MII Extended Mode Advertisement Register
 * @{
 */
#define MII_ADVERTISE_100_F BIT8    /**< Advertise 100BASE-T, full duplex */
#define MII_ADVERTISE_100_H BIT7    /**< Advertise 100BASE-T, half duplex */
#define MII_ADVERTISE_10_F  BIT6    /**< Advertise 10BASE-T, full duplex */
#define MII_ADVERTISE_10_H  BIT5    /**< Advertise 10BASE-T, half duplex */
#define MII_ADVERTISE_100   (BIT7 | BIT8)   /**< Advertise 100BASE-T */
#define MII_ADVERTISE_10    (BIT5 | BIT6)   /**< Advertise 10BASE-T */
/** @} */

/**
 * @name    Bits in the MII Extended Mode Advertisement Register
 * @{
 */
#define MII_LPA_100_F       BIT8    /**< Partner can 100BASE-T, full duplex */
#define MII_LPA_100_H       BIT7    /**< Partner can 100BASE-T, half duplex */
#define MII_LPA_10_F        BIT6    /**< Partner can 10BASE-T, full duplex */
#define MII_LPA_10_H        BIT5    /**< Partner can 10BASE-T, half duplex */
#define MII_LPA_100         (BIT7 | BIT8)   /**< Partner can 100BASE-T */
#define MII_LPA_10          (BIT5 | BIT6)   /**< Partner can 10BASE-T */
/** @} */

/**
 * @name    Bits in the MII Auto-Negotiation Expansion Register
 * @{
 */
#define MII_LPA_HAS_AN      BIT0    /**< Partner can auto-negotiate */
/** @} */

/**
 * @name    Bits in the MII Interrupt Control/Status Register
 * @{
 */
#define MII_IRQ_LINK_UP     BIT0    /**< Link-up occurred */
#define MII_IRQ_RMT_FAULT   BIT1    /**< Remote fault occurred */
#define MII_IRQ_LINK_DOWN   BIT2    /**< Link-down occurred */
#define MII_IRQ_LPA_ACK     BIT3    /**< Link partner acknowledge occurred */
#define MII_IRQ_PD_FAULT    BIT4    /**< Parallel detect fault occurred */
#define MII_IRQ_PAGE_RX     BIT5    /**< Page receive occurred */
#define MII_IRQ_RX_ERROR    BIT6    /**< Receive error occurred */
#define MII_IRQ_JABBER      BIT7    /**< Jabber occurred */
#define MII_IRQ_EN_LINK_UP     BIT8    /**< Enable Link-up occurred IRQ */
#define MII_IRQ_EN_RMT_FAULT   BIT9    /**< Enable Remote fault occurred IRQ */
#define MII_IRQ_EN_LINK_DOWN   BIT10   /**< Enable Link-down occurred IRQ */
#define MII_IRQ_EN_LPA_ACK     BIT11    /**< Enable Link partner acknowledge occurred IRQ */
#define MII_IRQ_EN_PD_FAULT    BIT12    /**< Enable Parallel detect fault occurred IRQ */
#define MII_IRQ_EN_PAGE_RX     BIT13    /**< Enable Page receive occurred IRQ */
#define MII_IRQ_EN_RX_ERROR    BIT14    /**< Enable Receive error occurred IRQ */
#define MII_IRQ_EN_JABBER      BIT15    /**< Enable Jabber occurred IRQ */
/** @} */

/**
 * @brief   Check if an Ethernet PHY supports 100 Mbps at full duplex
 *
 * @param[in]   bmsr    Value of the MII basic mode status register
 *
 * @retval      true    PHY supports 100 Mbps at full duplex
 * @retval      false   PHY does not support 100 Mbps at full duplex
 */
static inline bool mii_can_100_mbps_full_dp(uint16_t bmsr)
{
    return (bmsr & (MII_BMSR_100_TX_F | MII_BMSR_100_T2_F));
}

/**
 * @brief   Check if an Ethernet PHY supports 100 Mbps at half duplex
 *
 * @param[in]   bmsr    Value of the MII basic mode status register
 *
 * @retval      true    PHY supports 100 Mbps at half duplex
 * @retval      false   PHY does not support 100 Mbps at half duplex
 */
static inline bool mii_can_100_mbps_half_dp(uint16_t bmsr)
{
    return (bmsr & (MII_BMSR_100_T4 | MII_BMSR_100_TX_H | MII_BMSR_100_T2_H));
}

/**
 * @brief   Check if an Ethernet PHY supports 10 Mbps at full duplex
 *
 * @param[in]   bmsr    Value of the MII basic mode status register
 *
 * @retval      true    PHY supports 10 Mbps at full duplex
 * @retval      false   PHY does not support 10 Mbps at full duplex
 */
static inline bool mii_can_10_mbps_full_dp(uint16_t bmsr)
{
    return (bmsr & MII_BMSR_10_F);
}

/**
 * @brief   Check if an Ethernet PHY supports 10 Mbps at half duplex
 *
 * @param[in]   bmsr    Value of the MII basic mode status register
 *
 * @retval      true    PHY supports 10 Mbps at half duplex
 * @retval      false   PHY does not support 10 Mbps at half duplex
 */
static inline bool mii_can_10_mbps_half_dp(uint16_t bmsr)
{
    return (bmsr & MII_BMSR_10_H);
}

#ifdef __cplusplus
}
#endif

/** @} */
