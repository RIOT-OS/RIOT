/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_can
 * @defgroup    candev_stm32 STM32 CAN controller
 * @brief       STM32 CAN controller driver (bxCAN)
 *
 * The STM32Fx microcontroller can have an integrated CAN controller (bxCAN)
 *
 * This driver has been tested with a STM32F0,STM32F2 and STM32F4 MCU
 * but should work on others.
 *
 * The default bitrate is set to 500 kbps and the default sample point is set to
 * 87.5%.
 * @{
 *
 * @file
 * @brief       bxCAN specific definitions
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "can/candev.h"

/** Number of channels in the device (up to 3) */
#if defined(CAN3)
#define CANDEV_STM32_CHAN_NUMOF 3
#elif defined(CAN2)
#define CANDEV_STM32_CHAN_NUMOF 2
#elif defined(CAN1) || defined(CAN) || DOXYGEN
#define CANDEV_STM32_CHAN_NUMOF 1
#else
#error "CAN STM32: CPU not supported"
#endif

/**
 * @name    ISR functions
 * @{
 */
#if defined(CPU_FAM_STM32F1)
#define ISR_CAN1_TX     isr_usb_hp_can1_tx
#define ISR_CAN1_RX0    isr_usb_lp_can1_rx0
#define ISR_CAN1_RX1    isr_can1_rx1
#define ISR_CAN1_SCE    isr_can1_sce
#else
#define ISR_CAN1_TX     isr_can1_tx
#define ISR_CAN1_RX0    isr_can1_rx0
#define ISR_CAN1_RX1    isr_can1_rx1
#define ISR_CAN1_SCE    isr_can1_sce
#define ISR_CAN2_TX     isr_can2_tx
#define ISR_CAN2_RX0    isr_can2_rx0
#define ISR_CAN2_RX1    isr_can2_rx1
#define ISR_CAN2_SCE    isr_can2_sce
#define ISR_CAN3_TX     isr_can3_tx
#define ISR_CAN3_RX0    isr_can3_rx0
#define ISR_CAN3_RX1    isr_can3_rx1
#define ISR_CAN3_SCE    isr_can3_sce
#endif
/** @} */

#if CANDEV_STM32_CHAN_NUMOF > 1 || DOXYGEN
/** The maximum number of filters: 28 for dual channel, 14 for single channel */
#define CAN_STM32_NB_FILTER     28
#else
#define CAN_STM32_NB_FILTER     14
#endif

#ifndef CANDEV_STM32_DEFAULT_BITRATE
/** Default bitrate */
#define CANDEV_STM32_DEFAULT_BITRATE 500000U
#endif

#ifndef CANDEV_STM32_DEFAULT_SPT
/** Default sampling-point */
#define CANDEV_STM32_DEFAULT_SPT     875
#endif

/** bxCAN device configuration */
typedef struct {
    CAN_TypeDef *can;           /**< CAN device */
    uint32_t rcc_mask;          /**< RCC mask to enable clock */
    gpio_t rx_pin;              /**< RX pin */
    gpio_t tx_pin;              /**< TX pin */
#ifndef CPU_FAM_STM32F1
    gpio_af_t af;               /**< Alternate pin function to use */
#endif
    bool en_deep_sleep_wake_up; /**< Enable deep-sleep wake-up interrupt */
#if CANDEV_STM32_CHAN_NUMOF > 1 || defined(DOXYGEN)
    CAN_TypeDef *can_master;    /**< Master CAN device */
    uint32_t master_rcc_mask;   /**< Master device RCC mask */
    /** First filter in the bank. For a master channel it must be 0.
     *  For a slave channel, it is used without checking with the master channel,
     *  beware bot to overwrite the master config. */
    uint8_t first_filter;
    /** Number of filters to use. Must be less or equal
     *  to CAN_STM32_NB_FILTER - first_filter */
    uint8_t nb_filters;
#endif
#if defined(CPU_FAM_STM32F0)
    uint8_t irqn;                       /**< CAN common IRQ channel */
#else
    uint8_t tx_irqn;                    /**< TX IRQ channel */
    uint8_t rx0_irqn;                   /**< RX0 IRQ channel */
    uint8_t rx1_irqn;                   /**< RX1 IRQ channel */
    uint8_t sce_irqn;                   /**< SCE IRQ channel */
#endif
    uint8_t ttcm : 1;                   /**< Time triggered communication mode */
    uint8_t abom : 1;                   /**< Automatic bus-off management */
    uint8_t awum : 1;                   /**< Automatic wakeup mode */
    uint8_t nart : 1;                   /**< No automatic retransmission */
    uint8_t rflm : 1;                   /**< Receive FIFO locked mode */
    uint8_t txfp : 1;                   /**< Transmit FIFO priority */
    uint8_t lbkm : 1;                   /**< Loopback mode */
    uint8_t silm : 1;                   /**< Silent mode */
} can_conf_t;
/** can_conf_t is re-defined */
#define HAVE_CAN_CONF_T

/** The number of transmit mailboxes */
#define CAN_STM32_TX_MAILBOXES 3
/** The number of receive FIFO */
#define CAN_STM32_RX_MAILBOXES 2

#ifndef CAN_STM32_RX_MAIL_FIFO
/** This is the maximum number of frame the driver can receive simultaneously */
#define CAN_STM32_RX_MAIL_FIFO 12
#endif

/** bxCAN candev descriptor */
typedef struct can can_t;
/** can_t is re-defined */
#define HAVE_CAN_T

/** This structure holds anything related to the receive part */
typedef struct candev_stm32_rx_fifo {
    struct can_frame frame[CAN_STM32_RX_MAIL_FIFO]; /**< Receive FIFO */
    int write_idx;                                  /**< Write index in the receive FIFO */
    int read_idx;                                   /**< Read index in the receive FIFO*/
    int is_full;                                    /**< Flag set when the FIFO is full */
} candev_stm32_rx_fifo_t;

/** Internal interrupt flags */
typedef struct candev_stm32_isr {
    int isr_tx : 3;     /**< Tx mailboxes interrupt */
    int isr_rx : 2;     /**< Rx FIFO interrupt */
    int isr_wkup : 1;   /**< Wake up interrupt */
} candev_stm32_isr_t;

/** STM32 CAN device descriptor */
struct can {
    candev_t candev;                    /**< Common candev struct */
    const can_conf_t *conf;    /**< Configuration */
    gpio_t rx_pin;                      /**< RX pin */
    gpio_t tx_pin;                      /**< TX pin */
    gpio_af_t af;                       /**< Alternate pin function to use */
    /** Tx mailboxes */
    const struct can_frame *tx_mailbox[CAN_STM32_TX_MAILBOXES];
    candev_stm32_rx_fifo_t rx_fifo;     /**< Rx FIFOs */
    candev_stm32_isr_t isr_flags;       /**< ISR flags */
};

#ifndef CPU_FAM_STM32F1
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
#else
/**
 * @brief Set the pins of an stm32 CAN device
 *
 * @param[in,out] dev     the device to set pins
 * @param[in] tx_pin      tx pin
 * @param[in] rx_pin      rx pin
 */
void candev_stm32_set_pins(can_t *dev, gpio_t tx_pin, gpio_t rx_pin);
#endif

#ifdef __cplusplus
}
#endif
