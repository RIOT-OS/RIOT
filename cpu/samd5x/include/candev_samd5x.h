/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd5x
 * @brief           CPU specific definitions for CAN controllers
 * @{
 *
 * @file
 * @brief           SAMD5x CAN controller (M_CAN Bosch) driver
 *
 * @author          Firas Hamdi <firas.hamdi@ml-pa.com>
 */

#ifndef CANDEV_SAMD5X_H
#define CANDEV_SAMD5X_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CAN_INST_NUM)
#include "can/candev.h"

#ifndef CANDEV_SAMD5X_DEFAULT_BITRATE
/**
 * @brief   Bitrate to use on init if @ref can_conf_t::bitrate is zero
 */
#define CANDEV_SAMD5X_DEFAULT_BITRATE 500000U
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_SPT
/** Default sampling-point */
#define CANDEV_SAMD5X_DEFAULT_SPT 875
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_STD_FILTER_NUM
#define CANDEV_SAMD5X_DEFAULT_STD_FILTER_NUM 3
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_EXT_FILTER_NUM
#define CANDEV_SAMD5X_DEFAULT_EXT_FILTER_NUM 3
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_RX_FIFO_0_ELTS_NUM
#define CANDEV_SAMD5X_DEFAULT_RX_FIFO_0_ELTS_NUM 32
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_RX_FIFO_1_ELTS_NUM
#define CANDEV_SAMD5X_DEFAULT_RX_FIFO_1_ELTS_NUM 32
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_TX_EVT_FIFO_ELTS_NUM
#define CANDEV_SAMD5X_DEFAULT_TX_EVT_FIFO_ELTS_NUM 16
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_TX_BUFFER_NUM
#define CANDEV_SAMD5X_DEFAULT_TX_BUFFER_NUM 16
#endif

#ifndef CANDEV_SAMD5X_DEFAULT_TX_BUFFER_FIFO_QUEUE_NUM
#define CANDEV_SAMD5X_DEFAULT_TX_BUFFER_FIFO_QUEUE_NUM 16
#endif

/* unit: elements */
#define CANDEV_SAMD5X_MAX_STD_FILTER 128
#define CANDEV_SAMD5X_MAX_EXT_FILTER 64
#define CANDEV_SAMD5X_MAX_RX_FIFO_0_ELTS 64
#define CANDEV_SAMD5X_MAX_RX_FIFO_1_ELTS 64
#define CANDEV_SAMD5X_MAX_RX_BUFFER 64
#define CANDEV_SAMD5X_MAX_TX_EVT_FIFO_ELTS 32
#define CANDEV_SAMD5X_MAX_TX_BUFFER 32
#define CANDEV_SAMD5X_MSG_RAM_MAX_SIZE 448

/* SAMD5x CAN controller error codes (values from datasheet section 39.8.14) */
#define CANDEV_SAMD5X_NO_ERROR 0
#define CANDEV_SAMD5X_STUFF_ERROR 1
#define CANDEV_SAMD5X_FORM_ERROR 2
#define CANDEV_SAMD5X_ACK_ERROR 3
#define CANDEV_SAMD5X_BIT1_ERROR 4
#define CANDEV_SAMD5X_BIT0_ERROR 5
#define CANDEV_SAMD5X_CRC_ERROR 6
#define CANDEV_SAMD5X_NO_CHANGE_ERROR 7

/**
 * @brief CAN device configuration descriptor
 */
typedef struct {
    /** CAN device handler */
    Can *can;
    uint32_t bitrate; /**< initial bitrate after init */
    /** CAN Rx pin */
    gpio_t rx_pin;
    /** CAN Tx pin */
    gpio_t tx_pin;
    /** CAN transceiver enable pin */
    gpio_t enable_pin;
    /** GPIO mode to use for @ref can_conf_t::enable_pin */
    gpio_mode_t enable_pin_mode;
    /** GCLK source supplying the CAN controller */
    uint8_t gclk_src;
    /**
     * @brief Whether @ref can_conf_t::enable_pin is active-low
     *
     * If `true`, @ref can_conf_t::enable_pin is active-low will be set to
     * LOW when active and HIGH when inactive. If `false`,
     *  @ref can_conf_t::enable_pin will be set HIGH when active and LOW when
     * inactive.
     */
    bool enable_pin_active_low            : 1;
    /**
     * @brief Whether to disable automatic retransmission
     *
     * When set to `true`, a CAN frame will not be retransmitted on transmission
     * failure (e.g. on missing ACK). Otherwise the frame will be transmitted
     * again until it succeeds or the CAN controller goes into an error state.
     */
    bool disable_automatic_retransmission : 1;
    /**
     * @brief Whether to enable the transmit pause feature
     *
     * When set to `true`, the CAN controller will insert a delay of two bit
     * times between two subsequent frames. This decreases the maximum
     * throughput, but it ensures that other nodes with a lower priority
     * (according to the CAN ID) will not starve (in other words it prevents
     "Babbling Idiot Syndrome").
     */
    bool enable_transmit_pause            : 1;
    /**
     * @brief Whether to start in monitor mode after initialization
     *
     * When set to `true`, the CAN controller will start in monitor mode instead
     * of regular mode after initialization.
     */
    bool start_in_monitor_mode            : 1;
} can_conf_t;
#define HAVE_CAN_CONF_T

/**
 * @brief CAN message RAM accessible to the CAN controller
 */
typedef struct {
    /** Standard filters space in the CAN message RAM */
    CanMramSidfe std_filter[CANDEV_SAMD5X_DEFAULT_STD_FILTER_NUM];
    /** Extended filters space in the CAN message RAM */
    CanMramXifde ext_filter[CANDEV_SAMD5X_DEFAULT_EXT_FILTER_NUM];
    /** Reception FIFO_0 space in the CAN message RAM */
    CanMramRxf0e rx_fifo_0[CANDEV_SAMD5X_DEFAULT_RX_FIFO_0_ELTS_NUM];
    /** Reception FIFO_1 space in the CAN message RAM */
    CanMramRxf1e rx_fifo_1[CANDEV_SAMD5X_DEFAULT_RX_FIFO_1_ELTS_NUM];
    /** Reception buffers space in the CAN message RAM */
    CanMramRxbe rx_buffer[CANDEV_SAMD5X_MAX_RX_BUFFER];
    /** Transmission events FIFO space in the CAN message RAM */
    CanMramTxefe tx_event_fifo[CANDEV_SAMD5X_DEFAULT_TX_EVT_FIFO_ELTS_NUM];
    /** Transmission FIFO space in the CAN message RAM */
    CanMramTxbe tx_fifo_queue[CANDEV_SAMD5X_DEFAULT_TX_BUFFER_FIFO_QUEUE_NUM];
} msg_ram_conf_t;

/**
 * @brief CAN device descriptor
 */
typedef struct {
    /** Structure to hold driver state */
    candev_t candev;
    /** CAN device configuration descriptor */
    const can_conf_t *conf;
    /** CAN message RAM accessible to the CAN controller */
    msg_ram_conf_t msg_ram_conf;
    /** Enable/Disable Transceiver Delay Compensation */
    bool tdc_ctrl;
    /** False to use Tx FIFO operation, True to use Tx Queue operation */
    bool tx_fifo_queue_ctrl;
} can_t;
#define HAVE_CAN_T

/**
 * @brief   Enable/Disable the transmitter delay compensation
 *
 * @param[in] dev   device descriptor
 *
 */
void candev_samd5x_tdc_control(can_t *dev);

/**
 * @brief   Enter the device into sleep mode
 *
 * @param[in] candev   candev driver
 *
 */
void candev_samd5x_enter_sleep_mode(candev_t *candev);

/**
 * @brief   Wake up the device from sleep mode
 *
 * @param[in] candev   candev driver
 *
 */
void candev_samd5x_exit_sleep_mode(candev_t *candev);

#endif /* CAN_INST_NUM */

#ifdef __cplusplus
}
#endif

#endif /* CANDEV_SAMD5X_H */
/** @} */
