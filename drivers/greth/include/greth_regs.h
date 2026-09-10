/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_greth
 * @{
 *
 * @file
 * @brief       Register definitions for the Gaisler GRETH Ethernet MAC
 *
 * @author      Matvii Ivashchenko
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GRETH APB register map
 *
 * All registers are 32-bit wide. Base address from grmon: APB 0xff984000.
 */
typedef struct {
    volatile uint32_t ctrl;     /**< 0x00 Control */
    volatile uint32_t status;   /**< 0x04 Status / Interrupt flags */
    volatile uint32_t mac_msb;  /**< 0x08 MAC address bytes [0:1] */
    volatile uint32_t mac_lsb;  /**< 0x0C MAC address bytes [2:5] */
    volatile uint32_t mdio;     /**< 0x10 MDIO control and data */
    volatile uint32_t tx_desc;  /**< 0x14 TX descriptor table base address */
    volatile uint32_t rx_desc;  /**< 0x18 RX descriptor table base address */
    volatile uint32_t edcl_ip;  /**< 0x1C EDCL IP address (read-only) */
} greth_regs_t;

/**
 * @name    Control register bits (offset 0x00)
 * @{
 */
#define GRETH_CTRL_TXEN     (1u << 0)   /**< TX enable: start/continue DMA TX */
#define GRETH_CTRL_RXEN     (1u << 1)   /**< RX enable: start/continue DMA RX */
#define GRETH_CTRL_TXIRQEN  (1u << 2)   /**< TX interrupt enable */
#define GRETH_CTRL_RXIRQEN  (1u << 3)   /**< RX interrupt enable */
#define GRETH_CTRL_FD       (1u << 4)   /**< Full duplex mode */
#define GRETH_CTRL_PRO      (1u << 5)   /**< Promiscuous mode */
#define GRETH_CTRL_RST      (1u << 6)   /**< Software reset (self-clearing) */
#define GRETH_CTRL_SPD      (1u << 7)   /**< Speed: 1=100 Mbps, 0=10 Mbps */
#define GRETH_CTRL_GB       (1u << 8)   /**< Gigabit mode (1000 Mbps) */
#define GRETH_CTRL_EDCLDIS  (1u << 14)  /**< EDCL disable */
#define GRETH_CTRL_GBIT_CAP (1u << 27)  /**< Gigabit capable (read-only) */
#define GRETH_CTRL_EDCL_CAP (1u << 26)  /**< EDCL present (read-only, bit 26 not 31) */
/** @} */

/**
 * @name    Status register bits (offset 0x04)
 *
 * Write 1 to clear each bit.
 * @{
 */
#define GRETH_STATUS_RXERR  (1u << 0)   /**< RX error occurred */
#define GRETH_STATUS_TXERR  (1u << 1)   /**< TX error occurred */
#define GRETH_STATUS_RXIRQ  (1u << 2)   /**< RX frame received (DMA done) */
#define GRETH_STATUS_TXIRQ  (1u << 3)   /**< TX frame sent (DMA done) */
#define GRETH_STATUS_PHYIRQ (1u << 4)   /**< PHY link status change */
/** @} */

/**
 * @name    MDIO register bit fields (offset 0x10)
 * @{
 */
#define GRETH_MDIO_OP_WR    (1u)                /**< Write operation */
#define GRETH_MDIO_OP_RD    (2u)                /**< Read operation */
#define GRETH_MDIO_BUSY     (1u << 3)           /**< Transaction in progress */
#define GRETH_MDIO_NVALID   (1u << 4)           /**< Read data not valid */
#define GRETH_MDIO_REGSHIFT (6u)                /**< PHY register address shift */
#define GRETH_MDIO_PHYSHIFT (11u)               /**< PHY device address shift */
#define GRETH_MDIO_DATASHIFT (16u)              /**< Read/write data shift */
/** @} */

/**
 * @name    Buffer descriptor control word bits
 *
 * TX and RX descriptors share the same ctrl layout in bits 12:0.
 * Error bits in 18:14 differ between TX and RX.
 * @{
 */
#define GRETH_BD_LEN_MASK   (0x7FFu)    /**< bits[10:0]: frame byte length */
#define GRETH_BD_EN         (1u << 11)  /**< Enable: descriptor owned by DMA */
#define GRETH_BD_WR         (1u << 12)  /**< Wrap: last descriptor, next = first */
#define GRETH_BD_IE         (1u << 13)  /**< Interrupt enable after processing */
/** @} */

/**
 * @name    TX descriptor error bits (set by DMA on error)
 * @{
 */
#define GRETH_TXBD_ERR_UE   (1u << 14)  /**< Underrun error */
#define GRETH_TXBD_ERR_AL   (1u << 15)  /**< Attempt limit exceeded */
#define GRETH_TXBD_ERR_LC   (1u << 16)  /**< Late collision */
/** @brief All TX descriptor error bits combined */
#define GRETH_TXBD_ERR_MASK (GRETH_TXBD_ERR_UE | GRETH_TXBD_ERR_AL | \
                              GRETH_TXBD_ERR_LC)
/** @} */

/**
 * @name    RX descriptor error bits (set by DMA on error)
 * @{
 */
#define GRETH_RXBD_ERR_AE   (1u << 14)  /**< Alignment error */
#define GRETH_RXBD_ERR_FT   (1u << 15)  /**< Frame too long */
#define GRETH_RXBD_ERR_CRC  (1u << 16)  /**< CRC error */
#define GRETH_RXBD_ERR_OE   (1u << 17)  /**< FIFO overrun */
#define GRETH_RXBD_ERR_LE   (1u << 18)  /**< Length mismatch */
/** @brief All RX descriptor error bits combined */
#define GRETH_RXBD_ERR_MASK (GRETH_RXBD_ERR_AE | GRETH_RXBD_ERR_FT | \
                              GRETH_RXBD_ERR_CRC | GRETH_RXBD_ERR_OE | \
                              GRETH_RXBD_ERR_LE)
/** @} */

/**
 * @name    PHY MII register numbers (IEEE 802.3)
 * @{
 */
#define GRETH_MII_CTRL      (0)     /**< Basic Control */
#define GRETH_MII_STATUS    (1)     /**< Basic Status */
#define GRETH_MII_ADV       (4)     /**< Auto-Negotiation Advertisement */
#define GRETH_MII_LPA       (5)     /**< Link Partner Ability */

#define GRETH_MII_CTRL_RST      (1u << 15)  /**< Software reset */
#define GRETH_MII_CTRL_ANEG     (1u << 12)  /**< Auto-negotiation enable */
#define GRETH_MII_CTRL_SPD100   (1u << 13)  /**< 100 Mbps (if no aneg) */
#define GRETH_MII_CTRL_FD       (1u << 8)   /**< Full duplex (if no aneg) */
#define GRETH_MII_STATUS_ANEG_DONE (1u << 5) /**< Auto-negotiation complete */
#define GRETH_MII_STATUS_LINK   (1u << 2)   /**< Link status */
/** @} */

/**
 * @name    Auto-negotiation Advertisement / Link Partner Ability bits (regs 4/5)
 *
 * These are the capability bits in the standard MII AN advertisement register
 * (reg 4) and the link partner ability register (reg 5).  AND both registers
 * together to find the highest common link mode.
 * @{
 */
#define GRETH_MII_LPA_10_HD     (1u << 5)   /**< 10BASE-T Half Duplex */
#define GRETH_MII_LPA_10_FD     (1u << 6)   /**< 10BASE-T Full Duplex */
#define GRETH_MII_LPA_100_HD    (1u << 7)   /**< 100BASE-TX Half Duplex */
#define GRETH_MII_LPA_100_FD    (1u << 8)   /**< 100BASE-TX Full Duplex */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
