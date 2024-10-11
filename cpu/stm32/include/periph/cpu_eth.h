/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Ethernet CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_ETH_H
#define PERIPH_CPU_ETH_H

#include <stdint.h>

#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief STM32 Ethernet configuration mode
 */
typedef enum {
    MII  = 18,                  /**< Configuration for MII */
    RMII = 9,                   /**< Configuration for RMII */
    SMI  = 2,                   /**< Configuration for SMI */
} eth_mode_t;

/**
 * @brief   Ethernet Peripheral configuration
 */
typedef struct {
    eth_mode_t mode;            /**< Select configuration mode */
    uint16_t speed;             /**< Speed selection */
    uint8_t dma;                /**< Locical CMA Descriptor used for TX */
    uint8_t dma_chan;           /**< DMA channel used for TX */
    uint8_t phy_addr;           /**< PHY address */
    gpio_t pins[];              /**< Pins to use. MII requires 18 pins,
                                     RMII 9 and SMI 9. Not all speeds are
                                     supported by all modes. */
} eth_conf_t;

/**
 * @brief   Layout of enhanced RX/TX DMA descriptor
 *
 * @note    Don't confuse this with the normal RX/TX descriptor format.
 * @warning The content of the status and control bits is different for RX and
 *          TX DMA descriptors
 */
typedef struct eth_dma_desc {
    volatile uint32_t status;                   /**< Mostly status bits, some control bits */
    volatile uint32_t control;                  /**< Control bits */
    char * volatile buffer_addr;                /**< RX/TX buffer */
    struct eth_dma_desc * volatile desc_next;   /**< Address of next DMA descriptor */
    volatile uint32_t reserved1_ext;            /**< RX: Extended status, TX: reserved */
    volatile uint32_t reserved2;                /**< Reserved for future use */
    /**
     * @brief   Sub-second part of PTP timestamp of transmitted / sent frame
     *
     * For TX: If PTP timestamping is enabled and the TTSE bit in the
     * transmit descriptor word 0 (struct eth_dma_desc::status) is set, the
     * MAC will store the PTP timestamp of when the Start of Frame Delimiter
     * was sent. The TTSS bit is send by the hardware if the timestamp was
     * correctly set.
     *
     * For RX: If PTP timestamping is enabled, the timestamp of all received
     * frames is captured.
     */
    volatile uint32_t ts_low;
    volatile uint32_t ts_high;                  /**< Second part of PTP timestamp */
} edma_desc_t;

/**
 * @name    Flags in the status word of the Ethernet enhanced RX DMA descriptor
 * @{
 */
#define RX_DESC_STAT_LS         (BIT8)  /**< If set, descriptor is the last of a frame */
#define RX_DESC_STAT_FS         (BIT9)  /**< If set, descriptor is the first of a frame */
/**
 * @brief   Frame length
 *
 * The length of the frame in host memory order including CRC. Only valid if
 * @ref RX_DESC_STAT_LS is set and @ref RX_DESC_STAT_DE is not set.
 */
#define RX_DESC_STAT_FL         (0x3FFF0000) /* bits 16-29 */
#define RX_DESC_STAT_DE         (BIT14) /**< If set, a frame too large to fit buffers given by descriptors was received */
#define RX_DESC_STAT_ES         (BIT14) /**< If set, an error occurred during RX */
#define RX_DESC_STAT_OWN        (BIT31) /**< If set, descriptor is owned by DMA, otherwise by CPU */
/** @} */
/**
 * @name    Flags in the control word of the Ethernet enhanced RX DMA descriptor
 * @{
 */
/**
 * @brief   Indicates if RDES3 points to the next DMA descriptor (1), or to a second buffer (0)
 *
 * If the bit is set, RDES3 (@ref edma_desc_t::desc_next) will point to the
 * next DMA descriptor rather than to a second frame-segment buffer. This is
 * always set by the driver
 */
#define RX_DESC_CTRL_RCH        (BIT14)
/** @} */
/**
 * @name    Flags in the status word of the Ethernet enhanced TX DMA descriptor
 * @{
 */
#define TX_DESC_STAT_UF         (BIT1)  /**< If set, an underflow occurred while sending */
#define TX_DESC_STAT_EC         (BIT8)  /**< If set, TX was aborted due to excessive collisions (half-duplex only) */
#define TX_DESC_STAT_NC         (BIT10) /**< If set, no carrier was detected (TX aborted) */
#define TX_DESC_STAT_ES         (BIT15) /**< If set, one or more error occurred */
#define TX_DESC_STAT_TTSS       (BIT17) /**< If set, the descriptor contains a valid PTP timestamp */
/**
 * @brief   Indicates if TDES3 points to the next DMA descriptor (1), or to a second buffer (0)
 *
 * If the bit is set, TDES3 (@ref edma_desc_t::desc_next) will point to the
 * next DMA descriptor rather than to a second frame-segment buffer. This is
 * always set by the driver
 */
#define TX_DESC_STAT_TCH        (BIT20)
#define TX_DESC_STAT_TER        (BIT21) /**< If set, DMA will return to first descriptor in ring afterwards */
/**
 * @brief   Checksum insertion control
 *
 * | Value  | Meaning                                                                       |
 * |:------ |:----------------------------------------------------------------------------- |
 * | `0b00` | Checksum insertion disabled                                                   |
 * | `0b01` | Calculate and insert checksum in IPv4 header                                  |
 * | `0b10` | Calculate and insert IPv4 checksum, insert pre-calculated payload checksum    |
 * | `0b11  | Calculated and insert both IPv4 and payload checksum                          |
 */
#define TX_DESC_STAT_CIC        (BIT22 | BIT23)
#define TX_DESC_STAT_TTSE       (BIT25) /**< If set, an PTP timestamp is added to the descriptor after TX completed */
#define TX_DESC_STAT_FS         (BIT28) /**< If set, buffer contains first segment of frame to transmit */
#define TX_DESC_STAT_LS         (BIT29) /**< If set, buffer contains last segment of frame to transmit */
#define TX_DESC_STAT_IC         (BIT30) /**< If set, trigger IRQ on completion */
#define TX_DESC_STAT_OWN        (BIT31) /**< If set, descriptor is owned by DMA, otherwise by CPU */
/** @} */

#ifdef MODULE_PERIPH_ETH_COMMON
/**
 * @brief   Perform ETH initialization common to periph_stm32_eth and
 *          periph_ptp_clock
 */
void stm32_eth_common_init(void);
#endif /* MODULE_PERIPH_ETH_COMMON */


#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_ETH_H */
