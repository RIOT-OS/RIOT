/*
 * Copyright (C) 2019 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_dose Differentially Operated Serial Ethernet
 * @ingroup     drivers_netdev
 * @brief       Driver for connecting RIOT devices using a single bus wire
 *
 * About
 * =====
 *
 * This driver enables RIOT nodes to communicate by Ethernet over a serial bus.
 * This enables them to interact in an easy and cheap manner using a single
 * bus wire with very low hardware requirements: The used microcontrollers just
 * need to feature at least one UART and one optional GPIO that is able to raise
 * interrupts.
 *
 * Wiring
 * ======
 *
 * ![DOSE wiring](dose-wiring.svg)
 *
 * For bus access, you need a CAN transceiver, since the DOSE uses the PHY layer
 * of CAN for the electrical connection of the nodes. Every transceiver IC
 * operating with the right voltage levels should do. (If you are on a 3.3V MCU,
 * you could use an IC such as the SN65HVD233.)
 *
 * Basically, UART TX and RX are connected to respective pins of the
 * transceiver. In addition, the RX pin can also be connected to the sense GPIO
 * if the UART does not implement the `periph_uart_rxstart_irq` feature.
 * In this case, the bus allocation can be detected more precisely and
 * collisions are less likely.
 *
 * How it works
 * ============
 *
 * Some technical details for those interested: The Ethernet frames are sent
 * onto the bus using `uart_write()` while observing the received echo from
 * the bus. This way collisions are detected (received echo != transmitted
 * octet) and retransmissions are scheduled. The frames are appended with a
 * CRC16 to protect the system from transmission errors.
 *
 * @{
 *
 * @file
 * @brief       Driver for the Differentially Operated Serial Ethernet module
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef DOSE_H
#define DOSE_H

#include "periph/uart.h"
#include "periph/gpio.h"
#include "net/netdev.h"
#include "net/ethernet.h"
#include "net/eui48.h"
#include "bitarithm.h"
#include "mutex.h"
#include "xtimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Escape octet definitions
 * @{
 */
#define DOSE_OCTET_PREAMBLE (0x55) /**< Dummy bytes before SOF marker, help sync UART */
#define DOSE_OCTET_SOF      (0xFF) /**< Magic octet indicating the start of frame */
/** @} */

/**
 * @name    State definitions
 * @brief   The drivers internal state that is hold in dose_t.state
 */
typedef enum {
    DOSE_STATE_INIT    = 0x00,     /**< Initial state that will never be reentered */
    DOSE_STATE_BLOCKED = 0x01,     /**< The driver just listens to incoming frames and blocks outgress frames */
    DOSE_STATE_IDLE    = 0x02,     /**< Frames will be received or sent */
    DOSE_STATE_RECV    = 0x03,     /**< Currently receiving a frame */
    DOSE_STATE_SEND    = 0x04,     /**< Currently sending a frame */
    DOSE_STATE_STANDBY = 0x05,     /**< Receiver is turned off, but send will wake it up */
    DOSE_STATE_SLEEP   = 0x06,     /**< Receiver is turned off and send will be discarded */
    DOSE_STATE_ANY     = 0x0F      /**< Special state filter used internally to observe any state transition */
} dose_state_t;

/**
 * @name    Signal definitions
 * @brief   A signal controls the state machine and may cause a state transition
 */
typedef enum {
    DOSE_SIGNAL_NONE   = 0x00,    /**< No signal ... */
    DOSE_SIGNAL_INIT   = 0x10,    /**< Init the state machine */
    DOSE_SIGNAL_GPIO   = 0x20,    /**< Sense GPIO detected a falling edge */
    DOSE_SIGNAL_UART   = 0x30,    /**< Octet has been received */
    DOSE_SIGNAL_XTIMER = 0x40,    /**< Timer timed out */
    DOSE_SIGNAL_SEND   = 0x50,    /**< Enter send state */
    DOSE_SIGNAL_END    = 0x60     /**< Leave send state */
} dose_signal_t;

/**
 * @name    Flag definitions
 * @brief   Hold in dose_t.flags
 * @{
 */
#define DOSE_FLAG_RECV_BUF_DIRTY (BIT0)     /**< Receive buffer contains a complete unhandled frame */
#define DOSE_FLAG_END_RECEIVED   (BIT1)     /**< END octet has been received */
#define DOSE_FLAG_SOF_RECEIVED   (BIT2)     /**< Start of Frame marker has been received */
/** @} */

/**
 * @name    Opt definitions
 * @brief   Hold in dose_t.opts
 * @{
 */
#define DOSE_OPT_PROMISCUOUS     (BIT0)     /**< Don't check the destination MAC - pass every frame to upper layers */
/** @} */

/**
 * @defgroup drivers_dose_config    Differentially Operated Serial Ethernet (DOSE) driver compile configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief Timeout that brings the driver back into idle state expressed as bytes.
 *
 *  Fallback to idle if the remote side died within a transaction.
 */
#ifndef CONFIG_DOSE_TIMEOUT_BYTES
#define CONFIG_DOSE_TIMEOUT_BYTES       (50)
#endif
/** @} */

/**
 * @brief   DOSE on-line header
 *          This precedes the DOSE Ethernet frame
 */
typedef struct __attribute__((packed)) {
    uint8_t sof_marker;                     /**< Start of Frame marker */
    uint8_t parity;                         /**< xor of the two length bytes */
    uint16_t frame_len;                     /**< length of the subsequent DOSE frame */
} dose_header_t;

#define DOSE_FRAME_HEADER_LEN       (sizeof(dose_header_t))      /**< size of DOSE frame header */
#define DOSE_FRAME_CRC_LEN          (2)     /**< CRC16 is used */
#define DOSE_FRAME_LEN (ETHERNET_FRAME_LEN + DOSE_FRAME_CRC_LEN) /**< dose frame length */

/**
 * @brief   DOSE netdev device
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;                        /**< Extended netdev structure */
    eui48_t mac_addr;                       /**< This device's MAC address */
    uint8_t opts;                           /**< Driver options */
    dose_state_t state;                     /**< Current state of the driver's state machine */
    mutex_t state_mtx;                      /**< Is unlocked every time a state is (re)entered */
    uint8_t header_recv_buf[DOSE_FRAME_HEADER_LEN]; /**< Buffer to hold the DOSE header */
    uint8_t recv_buf[DOSE_FRAME_LEN];       /**< Receive buffer for incoming frames */
    uint16_t recv_buf_ptr;                  /**< Index of the next empty octet of the recveive buffer */
#if !defined(MODULE_PERIPH_UART_RXSTART_IRQ) || DOXYGEN
    gpio_t sense_pin;                       /**< GPIO to sense for start bits on the UART's rx line */
#endif
    gpio_t standby_pin;                     /**< GPIO to put the CAN transceiver in standby mode */
    xtimer_t timeout;                       /**< Timeout timer ensuring always to get back to IDLE state */
    uint32_t timeout_base;                  /**< Base timeout in us */
    uart_t uart;                            /**< UART device to use */
    uint8_t uart_octet;                     /**< Last received octet */
    uint8_t flags;                          /**< Several flags */
    uint8_t header_buf_ptr;                 /**< Index of the next header byte */
} dose_t;

/**
 * @brief   Struct containing the required configuration
 */
typedef struct {
    uart_t uart;                            /**< UART device to use */
#if !defined(MODULE_PERIPH_UART_RXSTART_IRQ) || DOXYGEN
    gpio_t sense_pin;                       /**< GPIO to sense for start bits on the UART's rx line */
#endif
    gpio_t standby_pin;                     /**< GPIO to put the CAN transceiver in standby mode */
    uint32_t baudrate;                      /**< Baudrate to UART device */
} dose_params_t;

/**
 * @brief   Setup a DOSE based device state
 * @param[out]  dev         Handle of the device to initialize
 * @param[in]   params      Parameters for device initialization
 * @param[in]   index       Index of @p params in a global parameter struct array.
 *                          If initialized manually, pass a unique identifier instead.
 */
void dose_setup(dose_t *dev, const dose_params_t *params, uint8_t index);

#ifdef __cplusplus
}
#endif
#endif /* DOSE_H */
/** @} */
