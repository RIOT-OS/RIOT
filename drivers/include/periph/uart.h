/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_uart UART
 * @ingroup     drivers_periph
 * @brief       Low-level UART peripheral driver
 *
 * This is a basic UART (Universal Asynchronous Receiver Transmitter) interface
 * to allow platform independent access to the MCU's serial communication
 * abilities. This interface is intentionally designed to be as simple as
 * possible, to allow for easy implementation and maximum portability.
 *
 * The simple interface provides capabilities to initialize and configure
 * the serial communication module, which automatically enables for receiving
 * data, as well as writing data to the UART port, which means transmitting
 * data. The UART device and the corresponding pins need to be mapped in
 * `RIOT/boards/ * /include/periph_conf.h`. Furthermore, you need to select the
 * symbol rate for initialization which is typically {9600, 19200, 38400, 57600,
 * 115200} baud. Additionally, you should register a callback function that is
 * executed in interrupt context when data is being received. The driver will
 * then read the received data byte, call the registered callback function and
 * pass the received data to it via its argument. The interface enforces the
 * receiving to be implemented in an interrupt driven mode. Thus, you never know
 * how many bytes are going to be received and might want to handle that in your
 * specific callback function. The transmit function can be implemented in any
 * way. You can also configure parity, the number of data and stop bits, i.e.
 * such combinations as 8-E-1, 7-N-2 etc. 8-N-1 mode is set by default.
 *
 * By default the @p UART_DEV(0) device of each board is initialized and mapped
 * to STDIO in RIOT which is used for standard input/output functions like
 * `printf()` or `puts()`.
 *
 * # (Low-) Power Implications
 *
 * After initialization, the UART peripheral **MUST** be powered on and
 * active. By calling @ref uart_poweroff the device can be powered off again.
 *
 * While the UART is active, the implementation might need to block certain
 * power states.
 *
 * @{
 *
 * @file
 * @brief       Low-level UART peripheral driver interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_UART_H
#define PERIPH_UART_H

#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Define default UART type identifier
 */
#ifndef HAVE_UART_T
typedef unsigned int uart_t;
#endif

/**
 * @brief   Default UART undefined value
 */
#ifndef UART_UNDEF
#define UART_UNDEF          (UINT_MAX)
#endif

/**
 * @brief   Default UART device access macro
 */
#ifndef UART_DEV
#define UART_DEV(x)         (x)
#endif

/**
 * @brief   Signature for receive interrupt callback
 *
 * @param[in] arg           context to the callback (optional)
 * @param[in] data          the byte that was received
 */
typedef void(*uart_rx_cb_t)(void *arg, uint8_t data);

/**
 * @brief   Interrupt context for a UART device
 */
#ifndef HAVE_UART_ISR_CTX_T
typedef struct {
    uart_rx_cb_t rx_cb;     /**< data received interrupt callback */
    void *arg;              /**< argument to both callback routines */
} uart_isr_ctx_t;
#endif

/**
 * @brief       Possible UART return values
 *
 * @deprecated  Use the errno constants directly instead.
 */
enum {
    UART_OK         =  0,           /**< everything in order */
    UART_NODEV      = -ENODEV,      /**< invalid UART device given */
    UART_NOBAUD     = -ENOTSUP,     /**< given symbol rate is not applicable */
    UART_NOMODE     = -ENOTSUP,     /**< given mode is not applicable */
    UART_INTERR     = -EIO,         /**< all other internal errors */
};

/**
 * @brief   Definition of possible parity modes
 */
#ifndef HAVE_UART_PARITY_T
typedef enum {
   UART_PARITY_NONE,   /**< no parity */
   UART_PARITY_EVEN,   /**< even parity */
   UART_PARITY_ODD,    /**< odd parity */
   UART_PARITY_MARK,   /**< mark parity */
   UART_PARITY_SPACE   /**< space parity */
} uart_parity_t;
#endif

/**
 * @brief   Definition of possible data bits lengths in a UART frame
 */
#ifndef HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5,   /**< 5 data bits */
    UART_DATA_BITS_6,   /**< 6 data bits */
    UART_DATA_BITS_7,   /**< 7 data bits */
    UART_DATA_BITS_8,   /**< 8 data bits */
} uart_data_bits_t;
#endif

/**
 * @brief   Definition of possible stop bits lengths in a UART frame
 */
#ifndef HAVE_UART_STOP_BITS_T
typedef enum {
   UART_STOP_BITS_1,   /**< 1 stop bit */
   UART_STOP_BITS_2,   /**< 2 stop bits */
} uart_stop_bits_t;
#endif

/**
 * @brief   Initialize a given UART device
 *
 * The UART device will be initialized with the following configuration:
 * - 8 data bits
 * - no parity
 * - 1 stop bit
 * - symbol rate as given
 *
 * If no callback parameter is given (rx_cb := NULL), the UART will be
 * initialized in TX only mode.
 *
 * @param[in] uart          UART device to initialize
 * @param[in] baud          desired symbol rate in baud
 * @param[in] rx_cb         receive callback, executed in interrupt context once
 *                          for every byte that is received (RX buffer filled),
 *                          set to NULL for TX only mode
 * @param[in] arg           optional context passed to the callback functions
 *
 * @post    The UART interface is configured, powered on, and active
 *
 * @note    The implementation must assume that the UART and the GPIOs used for TXD/RXD are in
 *          any possible state prior to the call. A full reset of the UART peripheral to bring
 *          the device in a known state is typically part of the initialization process.
 * @note    The implementation should keep the TXD pin high during initialization if supported by
 *          the hardware, so that no garbage is detected at the receiver during initialization.
 *          Typically configuring the peripheral before routing the GPIOs to it does the trick.
 *
 * @retval  0               Success
 * @retval  -ENODEV         Invalid UART device
 * @retval  -ENOTSUP        Unsupported symbol rate
 * @retval  <0              On other errors
 */
int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg);

#if defined(MODULE_PERIPH_UART_RECONFIGURE) || DOXYGEN
/**
 * @brief   Get the RX pin of the given UART.
 *
 * @param[in] uart      The device to query
 *
 * @note Until this is implemented on all platforms, this requires the
 *       periph_uart_reconfigure feature to be used.
 *
 * @return              The GPIO used for the UART RX line.
 */
gpio_t uart_pin_rx(uart_t uart);

/**
 * @brief   Get the TX pin of the given UART.
 *
 * @param[in] uart      The device to query
 *
 * @note Until this is implemented on all platforms, this requires the
 *       periph_uart_reconfigure feature to be used.
 *
 * @return              The GPIO used for the UART TX line.
 */
gpio_t uart_pin_tx(uart_t uart);

#endif /* MODULE_PERIPH_UART_RECONFIGURE */

/**
 * @brief   Setup parity, data and stop bits for a given UART device
 *
 * @param[in] uart          UART device to configure
 * @param[in] data_bits     number of data bits in a UART frame
 * @param[in] parity        parity mode
 * @param[in] stop_bits     number of stop bits in a UART frame
 *
 * @retval  0               Success
 * @retval  -ENOTSUP        Given configuration not supported
 * @retval  <0              Other error
 */
int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits);

/**
 * @brief   Write data from the given buffer to the specified UART device
 *
 * This function is blocking, as it will only return after @p len bytes from the
 * given buffer have been send. The way this data is send is up to the
 * implementation: active waiting, interrupt driven, DMA, etc.
 *
 * @param[in] uart          UART device to use for transmission
 * @param[in] data          data buffer to send
 * @param[in] len           number of bytes to send
 *
 */
void uart_write(uart_t uart, const uint8_t *data, size_t len);

/**
 * @brief Power off the given UART device
 *
 * @param[in] uart          the UART device to power off
 *
 * @post    The UART interface is powered off. After a call to @ref gpio_init the pins
 *          previously used by the UART peripheral must be usable as ordinary GPIOs.
 *
 * @note    If @ref gpio_init disconnects GPIOs from any peripherals they might be
 *          routed to anyway, the implementation does not need to pay any attention.
 *          Otherwise, it must disconnect the GPIOs so that they can be used as
 *          regular GPIOs again.
 */
void uart_poweroff(uart_t uart);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_UART_H */
/** @} */
