/*
 * SPDX-FileCopyrightText: 2024 COGIP Robotics association
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_can
 * @defgroup    fdcandev_stm32 STM32 FDCAN controller
 * @brief       STM32 FDCAN controller driver
 *
 * The STM32Gx microcontroller can have an integrated FDCAN controller
 *
 * This driver has been tested with a STM32G4 MCU
 * but should work on others.
 *
 * The default bitrates are set to 1 Mbps for headers and 4 Mbps for data.
 * The default sample point is set to 87.5%.
 * @{
 *
 * @file
 * @brief       FDCAN specific definitions
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "can/candev.h"

/** Number of channels in the device (up to 3) */
#if defined(FDCAN3)
#define FDCANDEV_STM32_CHAN_NUMOF 3
#elif defined(FDCAN2)
#define FDCANDEV_STM32_CHAN_NUMOF 2
#elif defined(FDCAN1) || DOXYGEN
#define FDCANDEV_STM32_CHAN_NUMOF 1
#else
#error "FDCAN STM32: CPU not supported"
#endif

/**
 * @name    ISR functions
 * @{
 */
#define ISR_FDCAN1_IT0  isr_fdcan1_it0  /**< Interrupt line 0 */
#define ISR_FDCAN1_IT1  isr_fdcan1_it1  /**< Interrupt line 1 */
/** @} */

/**
 * @name    Filters
 * @{
 */
#define FDCAN_STM32_NB_STD_FILTER   28U /**< Number of standard filters */
#define FDCAN_STM32_NB_EXT_FILTER   8U  /**< Number of extended filters */
#define FDCAN_STM32_NB_FILTER \
        (FDCAN_STM32_NB_STD_FILTER + FDCAN_STM32_NB_EXT_FILTER) /**< Total number of filters */
/** @} */

/**
 * @name    Birates
 * @{
 */
#ifndef FDCANDEV_STM32_DEFAULT_BITRATE
#define FDCANDEV_STM32_DEFAULT_BITRATE 500000U
    /**< Default bitrate for headers and non-FDCAN messages */
#endif
#ifndef FDCANDEV_STM32_DEFAULT_FD_DATA_BITRATE
#define FDCANDEV_STM32_DEFAULT_FD_DATA_BITRATE 1000000U
    /**< Default FDCAN data bitrate */
#endif
/** @} */

#ifndef FDCANDEV_STM32_DEFAULT_SPT
/** Default sampling-point */
#define FDCANDEV_STM32_DEFAULT_SPT     875
#endif

/** bxCAN device configuration */
typedef struct {
    FDCAN_GlobalTypeDef *can;   /**< CAN device */
    uint32_t rcc_mask;          /**< RCC mask to enable clock */
    gpio_t rx_pin;              /**< RX pin */
    gpio_t tx_pin;              /**< TX pin */
    gpio_af_t af;               /**< Alternate pin function to use */
    bool en_deep_sleep_wake_up; /**< Enable deep-sleep wake-up interrupt */
    uint8_t it0_irqn;           /**< Interrupt line 0 IRQ channel */
    uint8_t it1_irqn;           /**< Interrupt line 1 IRQ channel */
    uint8_t ttcm : 1;           /**< Time triggered communication mode */
    uint8_t abom : 1;           /**< Automatic bus-off management */
    uint8_t awum : 1;           /**< Automatic wakeup mode */
    uint8_t nart : 1;           /**< No automatic retransmission */
    uint8_t rflm : 1;           /**< Receive FIFO locked mode */
    uint8_t txfp : 1;           /**< Transmit FIFO priority */
    uint8_t lbkm : 1;           /**< Loopback mode */
    uint8_t silm : 1;           /**< Silent mode */
} can_conf_t;
/** can_conf_t is re-defined */
#define HAVE_CAN_CONF_T

/**
 * @name    STM32 mailboxes
 * @{
 */
#define FDCAN_STM32_TX_MAILBOXES 3
    /**< Number of frame the driver can transmit simultaneously */
#define FDCAN_STM32_RX_MAILBOXES (FDCANDEV_STM32_CHAN_NUMOF * 6)
        /**< Maximum number of frame the driver can receive simultaneously.
             There are 3 buffers per FIFO and 2 FIFO per channel. */
/** @} */

/** FDCAN candev descriptor */
typedef struct can can_t;
/** can_t is re-defined */
#define HAVE_CAN_T

#define FDCAN_SRAM_MESSAGE_RAM_SIZE     0x350   /**< FDCAN SRAM message size */

/**
 * @name Message RAM addresses - 32 bits aligned
 * @{
 */
#define FDCAN_SRAM_FLESA                0x1CU   /**< Filter List Extended Start Address */
#define FDCAN_SRAM_F0SA                 0x2CU   /**< Rx FIFO0 Start Address */
#define FDCAN_SRAM_F1SA                 0x62U   /**< Rx FIFO1 Start Address */
#define FDCAN_SRAM_EFSA                 0x98U   /**< Event FIFO Start Address */
#define FDCAN_SRAM_TBSA                 0x9EU   /**< Tx Buffer Start Address */
/** @} */

/**
 * @name Standard filter bit definition
 * @{
 */
#define FDCAN_SRAM_FLS_SFID1_Pos        (16U)
    /**< Standard filter ID 1 position */
#define FDCAN_SRAM_FLS_SFID1_Msk        (0x7FFU << FDCAN_SRAM_FLS_SFID1_Pos)
    /**< Standard filter ID 1 mask */
#define FDCAN_SRAM_FLS_SFID1            FDCAN_SRAM_FLS_SFID1_Msk
    /**< Standard filter ID 1 */
#define FDCAN_SRAM_FLS_SFID2_Msk        (0x7FFU)
    /**< Standard filter ID 2 mask */
#define FDCAN_SRAM_FLS_SFID2            FDCAN_SRAM_FLS_SFID2_Msk
    /**< Standard filter ID 2 */
#define FDCAN_SRAM_FLS_SFT_Pos          (30U)
    /**< Standard filter type position */
#define FDCAN_SRAM_FLS_SFT_Msk          (0x3U << FDCAN_SRAM_FLS_SFT_Pos)
    /**< Standard filter type mask */
#define FDCAN_SRAM_FLS_SFT              FDCAN_SRAM_FLS_SFT_Msk
    /**< Standard filter type */
#define FDCAN_SRAM_FLS_SFEC_Pos         (27U)
    /**< Standard filter element configuration position */
#define FDCAN_SRAM_FLS_SFEC_Msk         (0x7U << FDCAN_SRAM_FLS_SFEC_Pos)
    /**< Standard filter element configuration mask */
#define FDCAN_SRAM_FLS_SFEC             FDCAN_SRAM_FLS_SFEC_Msk
    /**< Standard filter element configuration */
/** @} */

/**
 * @name Standard filter configuration
 * @{
 */
#define FDCAN_SRAM_FLS_FILTER_SIZE      1U
    /**< Standard filter size */
#define FDCAN_SRAM_FLS_SFT_DISABLED     (0x3U << FDCAN_SRAM_FLS_SFT_Pos)
    /**< Filter element disabled */
#define FDCAN_SRAM_FLS_SFT_CLASSIC      (0x2U << FDCAN_SRAM_FLS_SFT_Pos)
    /**< Classic filter */
#define FDCAN_SRAM_FLS_SFEC_DISABLED    (0x0U << FDCAN_SRAM_FLS_SFEC_Pos)
    /**< Filter element disabled */
#define FDCAN_SRAM_FLS_SFEC_FIFO0       (0x1U << FDCAN_SRAM_FLS_SFEC_Pos)
    /**< Use FIFO0 if filter matches */
#define FDCAN_SRAM_FLS_SFEC_FIFO1       (0x2U << FDCAN_SRAM_FLS_SFEC_Pos)
    /**< Use FIFO1 if filter matches*/
/** @} */

/**
 * @name Extended filter bit definition
 * @{
 */
#define FDCAN_SRAM_FLE_F0_EFID1_Msk     0x1FFFFFFFU
    /**< Extended filter ID 1 mask */
#define FDCAN_SRAM_FLE_F0_EFID1         FDCAN_SRAM_FLE_F0_EFID1_Msk
    /**< Extended filter ID 1 */
#define FDCAN_SRAM_FLE_F1_EFID2_Msk     0x1FFFFFFFU
    /**< Extended filter ID 2 mask */
#define FDCAN_SRAM_FLE_F1_EFID2         FDCAN_SRAM_FLE_F1_EFID2_Msk
    /**< Extended filter ID 2 */
#define FDCAN_SRAM_FLE_F1_EFT_Pos       30U
    /**< Extended filter type position */
#define FDCAN_SRAM_FLE_F1_EFT_Msk       (0x3U << FDCAN_SRAM_FLE_F1_EFT_Pos)
    /**< Extended filter type mask */
#define FDCAN_SRAM_FLE_F1_EFT           FDCAN_SRAM_FLE_F1_EFT_Msk
    /**< Extended filter type */
#define FDCAN_SRAM_FLE_F0_EFEC_Pos      29U
    /**< Extended filter element configuration position */
#define FDCAN_SRAM_FLE_F0_EFEC_Msk      (0x7U << FDCAN_SRAM_FLE_F0_EFEC_Pos)
    /**< Extended filter element configuration mask */
#define FDCAN_SRAM_FLE_F0_EFEC          FDCAN_SRAM_FLE_F0_EFEC_Msk
    /**< Extended filter element configuration */
/** @} */

/**
 * @name Extended filter configuration
 * @{
 */
#define FDCAN_SRAM_FLE_FILTER_SIZE      2U
    /**< Extended filter size */
#define FDCAN_SRAM_FLE_F1_EFT_CLASSIC   (0x2U << FDCAN_SRAM_FLE_F1_EFT_Pos)
    /**< Classic filter */
#define FDCAN_SRAM_FLE_F0_EFEC_DISABLED (0x0U)
    /**< Disabled filter */
#define FDCAN_SRAM_FLE_F0_EFEC_FIFO0    (0x1U << FDCAN_SRAM_FLE_F0_EFEC_Pos)
    /**< Use FIFO0 if filter matches */
#define FDCAN_SRAM_FLE_F0_EFEC_FIFO1    (0x2U << FDCAN_SRAM_FLE_F0_EFEC_Pos)
    /**< Use FIFI1 if filter matches */
/** @} */

/**
 * @name Tx Buffer bits definition
 * @{
 */
#define FDCAN_SRAM_TXBUFFER_T0_ESI_Pos  31U
    /**< Error State Indicator position */
#define FDCAN_SRAM_TXBUFFER_T0_ESI_Msk  (0x1U << FDCAN_SRAM_TXBUFFER_T0_ESI_Pos)
    /**< Error State Indicator mask */
#define FDCAN_SRAM_TXBUFFER_T0_ESI      FDCAN_SRAM_TXBUFFER_T0_ESI_Msk
    /**< Error State Indicator */
#define FDCAN_SRAM_TXBUFFER_T0_XTD_Pos  30U
    /**< Extended Identifier position */
#define FDCAN_SRAM_TXBUFFER_T0_XTD_Msk  (0x1U << FDCAN_SRAM_TXBUFFER_T0_XTD_Pos)
    /**< Extended Identifier mask */
#define FDCAN_SRAM_TXBUFFER_T0_XTD      FDCAN_SRAM_TXBUFFER_T0_XTD_Msk
    /**< Extended Identifier */
#define FDCAN_SRAM_TXBUFFER_T0_RTR_Pos  29U
    /**< Remote transmission request position*/
#define FDCAN_SRAM_TXBUFFER_T0_RTR_Msk  (0x1U << FDCAN_SRAM_TXBUFFER_T0_RTR_Pos)
    /**< Remote transmission request mask */
#define FDCAN_SRAM_TXBUFFER_T0_RTR      FDCAN_SRAM_TXBUFFER_T0_RTR_Msk
    /**< Remote transmission request */
#define FDCAN_SRAM_TXBUFFER_T0_ID_Pos   18U
    /**< Standard Identifier position */
#define FDCAN_SRAM_TXBUFFER_T1_EFC_Pos  23U
    /**< Event FIFO Control position */
#define FDCAN_SRAM_TXBUFFER_T1_EFC_Msk  (0x1U << FDCAN_SRAM_TXBUFFER_T1_EFC_Pos)
    /**< Event FIFO Control mask */
#define FDCAN_SRAM_TXBUFFER_T1_EFC      FDCAN_SRAM_TXBUFFER_T1_EFC_Msk
    /**< Event FIFO Control */
#define FDCAN_SRAM_TXBUFFER_T1_FDF_Pos  21U
    /**< FD Format position */
#define FDCAN_SRAM_TXBUFFER_T1_FDF_Msk  (0x1U << FDCAN_SRAM_TXBUFFER_T1_FDF_Pos)
    /**< FD Format mask */
#define FDCAN_SRAM_TXBUFFER_T1_FDF      FDCAN_SRAM_TXBUFFER_T1_FDF_Msk
    /**< FD Format */
#define FDCAN_SRAM_TXBUFFER_T1_BRS_Pos  20U
    /**< Bit Rate Switching position */
#define FDCAN_SRAM_TXBUFFER_T1_BRS_Msk  (0x1U << FDCAN_SRAM_TXBUFFER_T1_BRS_Pos)
    /**< Bit Rate Switching mask */
#define FDCAN_SRAM_TXBUFFER_T1_BRS      FDCAN_SRAM_TXBUFFER_T1_BRS_Msk
    /**< Bit Rate Switching */
#define FDCAN_SRAM_TXBUFFER_T1_DLC_Pos  16U
    /**< Data Length Code position */
#define FDCAN_SRAM_TXBUFFER_T1_DLC_Msk  (0xFU << FDCAN_SRAM_TXBUFFER_T1_DLC_Pos)
    /**< Data Length Code mask */
#define FDCAN_SRAM_TXBUFFER_T1_DLC      FDCAN_SRAM_TXBUFFER_T1_DLC_Msk
    /**< Data Length Code */
/** @} */

/**
 * @name Tx buffers configuration
 * @{
 */
#define FDCAN_SRAM_TXBUFFER_SIZE                18U
    /**< Tx buffer size */
#define FDCAN_SRAM_TXBUFFER_T0_ESI_PASSIVE_FLAG (0x0U << FDCAN_SRAM_TXBUFFER_T0_ESI_Pos)
    /**< ESI bit in CAN FD format depends only on error passive flag */
#define FDCAN_SRAM_TXBUFFER_T0_ESI_RECESSIVE    (0x1U << FDCAN_SRAM_TXBUFFER_T0_ESI_Pos)
    /**< ESI bit in CAN FD format transmitted recessive */
#define FDCAN_SRAM_TXBUFFER_T1_EFC_DISABLE      (0x0U << FDCAN_SRAM_TXBUFFER_T1_EFC_Pos)
    /**< Do not store Tx events */
#define FDCAN_SRAM_TXBUFFER_T1_EFC_STORE_EVENTS (0x1U << FDCAN_SRAM_TXBUFFER_T1_EFC_Pos)
    /**< Store Tx events */
#define FDCAN_SRAM_TXBUFFER_T1_FDF_CLASSIC      (0x0U << FDCAN_SRAM_TXBUFFER_T1_FDF_Pos)
    /**< Classic CAN format */
#define FDCAN_SRAM_TXBUFFER_T1_FDF_FD           (0x1U << FDCAN_SRAM_TXBUFFER_T1_FDF_Pos)
    /**< CAN FD format */
#define FDCAN_SRAM_TXBUFFER_T1_BRS_OFF          (0x0U << FDCAN_SRAM_TXBUFFER_T1_BRS_Pos)
    /**< Disable CAN FD bit rate switching */
#define FDCAN_SRAM_TXBUFFER_T1_BRS_ON           (0x1U << FDCAN_SRAM_TXBUFFER_T1_BRS_Pos)
    /**< Enable CAN FD bit rate switching */
/** @} */

/**
 * @name Rx Buffer bits definition
 * @{
 */
#define FDCAN_SRAM_RXFIFO_R0_ESI_Pos    31U
    /**< Error State Indicator position */
#define FDCAN_SRAM_RXFIFO_R0_ESI_Msk    (0x1U << FDCAN_SRAM_RXFIFO_R0_ESI_Pos)
    /**< Error State Indicator mask */
#define FDCAN_SRAM_RXFIFO_R0_ESI        FDCAN_SRAM_RXFIFO_R0_ESI_Msk
    /**< Error State Indicator */
#define FDCAN_SRAM_RXFIFO_R0_XTD_Pos    30U
    /**< Extended Identifier position */
#define FDCAN_SRAM_RXFIFO_R0_XTD_Msk    (0x1U << FDCAN_SRAM_RXFIFO_R0_XTD_Pos)
    /**< Extended Identifier mask */
#define FDCAN_SRAM_RXFIFO_R0_XTD        FDCAN_SRAM_RXFIFO_R0_XTD_Msk
    /**< Extended Identifier */
#define FDCAN_SRAM_RXFIFO_R0_RTR_Pos    29U
    /**< Remote transmission request position*/
#define FDCAN_SRAM_RXFIFO_R0_RTR_Msk    (0x1U << FDCAN_SRAM_RXFIFO_R0_RTR_Pos)
    /**< Remote transmission request mask */
#define FDCAN_SRAM_RXFIFO_R0_RTR        FDCAN_SRAM_RXFIFO_R0_RTR_Msk
    /**< Remote transmission request */
#define FDCAN_SRAM_RXFIFO_R0_ID_Pos     18U
    /**< Standard Identifier position */
#define FDCAN_SRAM_RXFIFO_R0_ID_Msk     0x1FFFFFFFU
    /**< Identifier mask */
#define FDCAN_SRAM_RXFIFO_R0_ID         FDCAN_SRAM_RXFIFO_R0_ID_Msk
    /**< Identifier */
#define FDCAN_SRAM_RXFIFO_R1_EFC_Pos    23U
    /**< Event FIFO Control position */
#define FDCAN_SRAM_RXFIFO_R1_EFC_Msk    (0x1U << FDCAN_SRAM_RXFIFO_R1_EFC_Pos)
    /**< Event FIFO Control mask */
#define FDCAN_SRAM_RXFIFO_R1_EFC        FDCAN_SRAM_RXFIFO_R1_EFC_Msk
    /**< Event FIFO Control */
#define FDCAN_SRAM_RXFIFO_R1_FDF_Pos    21U
    /**< FD Format position */
#define FDCAN_SRAM_RXFIFO_R1_FDF_Msk    (0x1U << FDCAN_SRAM_RXFIFO_R1_FDF_Pos)
    /**< FD Format mask */
#define FDCAN_SRAM_RXFIFO_R1_FDF        FDCAN_SRAM_RXFIFO_R1_FDF_Msk
    /**< FD Format */
#define FDCAN_SRAM_RXFIFO_R1_BRS_Pos    20U
    /**< Bit Rate Switching position */
#define FDCAN_SRAM_RXFIFO_R1_BRS_Msk    (0x1U << FDCAN_SRAM_RXFIFO_R1_BRS_Pos)
    /**< Bit Rate Switching mask */
#define FDCAN_SRAM_RXFIFO_R1_BRS        FDCAN_SRAM_RXFIFO_R1_BRS_Msk
    /**< Bit Rate Switching */
#define FDCAN_SRAM_RXFIFO_R1_DLC_Pos    16U
    /**< Data Length Code position */
#define FDCAN_SRAM_RXFIFO_R1_DLC_Msk    (0xFU << FDCAN_SRAM_RXFIFO_R1_DLC_Pos)
    /**< Data Length Code mask */
#define FDCAN_SRAM_RXFIFO_R1_DLC        FDCAN_SRAM_RXFIFO_R1_DLC_Msk
    /**< Data Length Code */
/** @} */

/**
 * @name Rx buffers configuration
 * @{
 */
#define FDCAN_SRAM_RXFIFO_SIZE          54U
    /**< Rx FIFO size */
#define FDCAN_SRAM_RXFIFO_ELEMENT_SIZE  18U
    /**< Rx FIFO element size */
#define FDCAN_SRAM_RXFIFO_R0_ESI_PASSIVE_FLAG   (0x0U << FDCAN_SRAM_RXFIFO_R0_ESI_Pos)
    /**< ESI bit in CAN FD format depends only on error passive flag */
#define FDCAN_SRAM_RXFIFO_R0_ESI_RECESSIVE      (0x1U << FDCAN_SRAM_RXFIFO_R0_ESI_Pos)
    /**< ESI bit in CAN FD format transmitted recessive */
#define FDCAN_SRAM_RXFIFO_R1_EFC_DISABLE        (0x0U << FDCAN_SRAM_RXFIFO_R1_EFC_Pos)
    /**< Do not store Rx events */
#define FDCAN_SRAM_RXFIFO_R1_EFC_STORE_EVENTS   (0x1U << FDCAN_SRAM_RXFIFO_R1_EFC_Pos)
    /**< Store Rx events */
#define FDCAN_SRAM_RXFIFO_R1_FDF_CLASSIC        (0x0U << FDCAN_SRAM_RXFIFO_R1_FDF_Pos)
    /**< Classic CAN format */
#define FDCAN_SRAM_RXFIFO_R1_FDF_FD             (0x1U << FDCAN_SRAM_RXFIFO_R1_FDF_Pos)
    /**< CAN FD format */
#define FDCAN_SRAM_RXFIFO_R1_BRS_OFF            (0x0U << FDCAN_SRAM_RXFIFO_R1_BRS_Pos)
    /**< CAN FD bit rate switching enabled */
#define FDCAN_SRAM_RXFIFO_R1_BRS_ON             (0x1U << FDCAN_SRAM_RXFIFO_R1_BRS_Pos)
    /**< CAN FD bit rate switching disabled */
/** @} */

/** This structure holds anything related to the receive part */
typedef struct candev_stm32_rx_mailbox {
    can_frame_t frame[FDCAN_STM32_RX_MAILBOXES];
        /**< Receive FIFO */
    int write_idx;
        /**< Write index in the receive FIFO */
    int read_idx;
        /**< Read index in the receive FIFO*/
    int is_full;
        /**< Flag set when the FIFO is full */
} candev_stm32_rx_mailbox_t;

/** Internal interrupt flags */
typedef struct candev_stm32_isr {
    int isr_tx : 3;     /**< Tx mailboxes interrupt */
    int isr_rx : 2;     /**< Rx FIFO interrupt */
    int isr_wkup : 1;   /**< Wake up interrupt */
} candev_stm32_isr_t;

/** STM32 CAN device descriptor */
struct can {
    candev_t candev;                    /**< Common candev struct */
    const can_conf_t *conf;             /**< Configuration */
    gpio_t rx_pin;                      /**< RX pin */
    gpio_t tx_pin;                      /**< TX pin */
    gpio_af_t af;                       /**< Alternate pin function to use */
    const can_frame_t
        *tx_mailbox[FDCAN_STM32_TX_MAILBOXES];  /**< Tx mailboxes*/
    candev_stm32_rx_mailbox_t rx_mailbox;       /**< Rx mailboxes */
    candev_stm32_isr_t isr_flags;               /**< ISR flags */
};

/**
 * @brief Set the pins of an stm32 CAN device
 *
 * @param[in,out] dev     the device to set pins
 * @param[in] tx_pin      tx pin
 * @param[in] rx_pin      rx pin
 * @param[in] af          alternate function
 */
void candev_stm32_set_pins(can_t *dev, gpio_t tx_pin, gpio_t rx_pin,
                           gpio_af_t af);
#ifdef __cplusplus
}
#endif
