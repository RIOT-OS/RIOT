/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_dali DALI protocol
 * @brief       DALI lowlevel protocol implementation
 *
 * This module provides a software implemented master DALI controller
 *
 * @{
 *
 * @file
 * @brief       DALI interface descriptor definition
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef DALI_H
#define DALI_H

#include "mutex.h"
#include "periph/gpio.h"
#include "ztimer.h"
#include "ztimer/periodic.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DALI logic '1' is a rising edge so the binary
 *        representation is 0b01
 */
#define DALI_RISING_EDGE  0x01

/**
 * @brief DALI logic '1' is a rising edge so the binary
 *        representation is 0b10
 */
#define DALI_FALLING_EDGE 0x02

/**
 * @brief Size of internal buffer for transmission for a
 *        peripheral node
 */
#define DALI_PERIPH2HOST_BUFFER_SIZE 3

/**
 * @brief Size of internal buffer for transmission for a
 *        host node
 */
#define DALI_HOST2PERIPH_BUFFER_SIZE 5

/**
 * @brief Number of bits for a standard transfer
 *        between a peripheral to host
 *
 * @note  1 Start bit + 8 data bits + 2 stop bits
 */
#define DALI_PERIPH2HOST_XFER_BITS_SIZE 11

/**
 * @brief Number of bits for a standard transfer
 *        between a host to a peripheral
 *
 * @note  1 Start bit + 8 addr bits + 8 data bits + 2 stop bits
 */
#define DALI_HOST2PERIPH_XFER_BITS_SIZE 19

/**
 * @brief Internal state machine for reception
 */
typedef enum {
    WAIT_FOR_START,
    SAMPLING,
    HALF_BIT,
    WAIT_FOR_IDLE,
    WAIT_FOR_STOP,
} rx_state_t;

/**
 * @brief DALI device configuration
 */
typedef struct {
    gpio_t rx_pin;  /**< DALI RX pin */
    gpio_t tx_pin;  /**< DALI TX pin */
} dali_conf_t;

/**
 * @brief DALI device type definition
 */
typedef struct {
    const dali_conf_t* cfg;             /**< DALI Configuration struct */
    ztimer_periodic_t timer_tx;         /**< Timer for bits transmission */
    ztimer_t timeout_xfer;              /**< Timeout for receiving slaves data */
    ztimer_t timeout_stop;              /**< Timeout for STOP transition detection */
    ztimer_now_t prev_time;             /**< Synchronisation mechanism for reception */
    rx_state_t rx_state;                /**< RX internal state machine */
    uint8_t* txbuf_ptr;                 /**< Pointer to tx buffer */
    uint32_t rx_frame;                  /**< RX frame buffer */
    uint8_t tx_idx;                     /**< Internal counter for transmission */
    uint8_t rx_idx;                     /**< Internal counter for reception */
    mutex_t xfer_pending;               /**< Mutex for transfer */
    mutex_t timeout;                    /**< Mutex for timeout */
} dali_intf_t;

/**
 * @brief DALI peripheral device type definition
 */
typedef struct {
    dali_intf_t intf;                               /**< DALI interface definition */
    uint8_t addr;                                   /**< DALI Slave address */
    uint8_t tx_buf[DALI_PERIPH2HOST_BUFFER_SIZE];   /**< internal buffer for transmission */
} dali_peripheral_t;

/**
 * @brief DALI host device type definition
 */
typedef struct {
    dali_intf_t intf;                                /**< DALI interface definition */
    uint8_t tx_buf[DALI_HOST2PERIPH_BUFFER_SIZE];    /**< internal buffer for transmission */
} dali_host_t;

/**
 * @brief   Initialize the DALI interface driver.
 *
 * @param[out] dev          device descriptor of DALI interface to initialize
 * @param[in]  cfg          configuration parameters
 *
 * @return                  0 on success
 * @return                  -EINVAL if GPIO configuration is missing
 */
int dali_intf_init(dali_intf_t* dev, const dali_conf_t* cfg);

/**
 * @brief   Setup reception of DALI interface.
 *
 * @note    This function will enable RX IRQ and set TX as input
 *
 * @param[out] dev          device descriptor of DALI interface to setup
 *
 * @return                  0 on success
 */
void dali_intf_setup_reception(dali_intf_t* dev);

/**
 * @brief   Setup transmission of DALI interface.
 *
 * @note    This function will disable RX IRQ and set TX as output
 *
 * @param[out] dev          device descriptor of DALI interface to setup
 *
 * @return                  0 on success
 */
void dali_intf_setup_transmission(dali_intf_t* dev);

#ifdef MODULE_DALI_PERIPHERAL
/**
 * @brief   Initialize the DALI peripheral node.
 *
 * @param[out] dev          device descriptor of DALI interface to initialize
 * @param[in]  cfg          configuration parameters
 * @param[in]  addr         DALI device address
 *
 * @return                  0 on success
 * @return                  -EINVAL if GPIO configuration is missing
 */
int dali_peripheral_init(dali_peripheral_t* dev, const dali_conf_t* cfg, uint8_t addr);

/**
 * @brief   Blocking DALI reception for a node.
 *
 * @note    Should be called from DALI node thread
 *
 * @param[in]  dev           device descriptor of DALI interface
 * @param[out] addr          1st byte of DALI transmission
 * @param[out] data          2nd byte of DALI transmission
 *
 * @return                  0 on success
 */
int dali_peripheral_recv(dali_peripheral_t* dev, uint8_t* addr, uint8_t* data);

/**
 * @brief   DALI reply for a peripheral.
 *
 * @note    Should be called from DALI peripheral thread
 *
 * @param[in]  dev           device descriptor of DALI interface
 * @param[out] data          1st byte of DALI transmission
 *
 * @return                  0 on success
 */
int dali_peripheral_reply(dali_peripheral_t* dev, uint8_t data);
#endif /* MODULE_DALI_PERIPHERAL */

#ifdef MODULE_DALI_HOST
/**
 * @brief   Initialize the DALI host driver.
 *
 * @param[out] dev          device descriptor of DALI host to initialize
 * @param[in]  cfg          configuration parameters
 *
 * @return                  0 on success
 * @return                  -EINVAL if GPIO configuration is missing
 */
int dali_host_init(dali_host_t* dev, const dali_conf_t* cfg);

/**
 * @brief   Setup a DALI transfer from the host to the device.
 *
 * @param[in] dev           device descriptor of DALI interface to use
 * @param[in]  addr         DALI address to send data to
 * @param[in]  cmd          DALI command to send
 * @param[out] data         Received byte from peripheral
 *
 * @return                  0 on success
 * @return                  -EINVAL on decoding error
 * @return                  -ENXIO if device doesn't answer
 */
int dali_host_xfer(dali_host_t* dev, uint8_t addr, uint8_t cmd, uint8_t* data);
#endif /* MODULE_DALI_HOST */

#ifdef __cplusplus
}
#endif

#endif /* DALI_H */
/** @} */
