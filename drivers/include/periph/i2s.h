/*
 * Copyright (C) 2023 Koen Zandberg <koen@bergzand.net>
 *                    Freie Universität Berlin
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */
/**
 * @defgroup    drivers_periph_i2s I2S
 * @ingroup     drivers_periph
 * @brief       Low level I2S audio transport api
 * @{
 *
 * # About
 *
 * # Bus format and terminology
 *
 * I2S terminology is sometimes a bit vague. The bus format as specified by
 * Philips consists of at least:
 *
 * - Bit clock line, often indicated as BCLK, but SCK is also possible. Supplies
 *   the clock for the synchronous bit data, similar to the clock line in SPI.
 * - Word clock line. Officially "word select", or WS. Often indicated as LRCLK
 *   (left-right clock), but FS (frame sync) is also possible. Indicates to
 *   which channel the current word belongs. Low means left channel, high is
 *   right channel.
 * - At least one Data line. Often indicated with SDI/SDO (Serial Data In/Out),
 *   but variants are possible.
 *
 * Optionally:
 *
 * - Master clock line, usually 256 times the word clock frequency, used to
 *   drive internal operations in the DAC/ADC. Not included in the spec, but
 *   often included by peripherals.
 *
 * # Details
 *
 * ## Configuration
 *
 * I2S requires a mix between compile-time configuration and runtime
 * reconfiguration. The compile-time configuration consists mostly of gpio
 * configuration.
 *
 * At runtime a number of settings must be reconfigurable to facilitate the
 * necessary flexibility of the interface.
 *
 * - Mode, switch between the MCU and the attached device providing the clocks.
 * - Direction, to switch between receiving and transmitting audio.
 * - Frame clock frequency to switch between different audio sample rates.
 * - Word size, e.g. switch between 16 bit and 24 bit samples.
 * - Number of words in a frame, to switch between single channel, stereo or arbitrary number of
 *   channels.
 * - Transaction buffer sizes, configure the size of the buffers that are going to be provided.
 *
 * ## Sample transactions
 *
 * The I2S peripheral API must be able to continuously transfer audio samples to
 * the device. The continuous stream of samples can be separated into
 * transactions. A transaction is a single continuous set of samples in memory,
 * fully described by a pointer and a length in bytes.
 * The continuous data transfer nature of the bus makes it suitable
 * for DMA operations, though the end user should not have to care about this.
 * DMA transfers allow the CPU to sleep or process other threads during the
 * full transactions. If DMA is not available the CPU must wake up twice to four
 * times every sample rate period to handle both channels in and out. With DMA
 * this can be reduced to once every transaction
 *
 * Sample transactions must be at least double buffered, adding a transaction
 * while the current transaction is on-going appends the new transaction to the
 * current on-going transaction. The appended transaction will automatically be
 * started after the current transaction to ensure continuous data transfers.
 *
 * Sample reception is identical to sample transmission. A transaction indicates
 * the memory space where to write the received samples to.
 *
 * ### Memory organization
 *
 * The organization of the transaction memory must be suitable for consumption
 * by the I2S peripherals. All words here are assumed to be at native endianness.
 *
 * 8 bit data must be organized as:
 *
 * ```
 * 32                          0
 * +------+------+------+------+
 * | L0   | R0   | L1   | R1   |
 * +------+------+------+------+
 * | L2   | R2   | L3   | R3   |
 * +------+------+------+------+
 * | ...  | ...  | ...  | ...  |
 * +------+------+------+------+
 * | LN-1 | RN-1 | LN   | RN   |
 * +------+------+------+------+
 * ```
 *
 * 16 bit samples must be organized as:
 *
 * ```
 * 32                          0
 * +-------------+-------------+
 * | L0          | R0          |
 * +-------------+-------------+
 * | L1          | R1          |
 * +-------------+-------------+
 * | ...         | ...         |
 * +-------------+-------------+
 * | LN          | RN          |
 * +-------------+-------------+
 * ```
 *
 * 24 bit samples must be organized as:
 *
 * ```
 * 32                          0
 * +------+--------------------+
 * | L0                 | 0x00 |
 * +--------------------+------+
 * | R0                 | 0x00 |
 * +--------------------+------+
 * | ...                | 0x00 |
 * +--------------------+------+
 * | LN                 | 0x00 |
 * +--------------------+------+
 * | RN                 | 0x00 |
 * +------+--------------------+
 * ```
 *
 * 32 bit samples must be organized as:
 *
 * ```
 * 32                          0
 * +---------------------------+
 * | L0                        |
 * +---------------------------+
 * | R0                        |
 * +---------------------------+
 * | ...                       |
 * +---------------------------+
 * | LN                        |
 * +---------------------------+
 * | RN                        |
 * +---------------------------+
 * ```
 *
 * ## Frequency
 *
 * The sample rate of the audio produced by the I2S bus matches the frequency of
 * the word clock configured. From this clock both the bit clock and the
 * controller clock are derived. The bit clock is defined as twice the bits per
 * channel and the master clock frequency is defined as 256 times the frequency
 * of the word clock.
 *
 * It is not always possible to perfectly match the requested sample rate on a
 * device. When configuring a sample rate, the actual sample rate can be requested
 * from the device via @ref i2s_get_frame_clock.
 *
 * ## Direction
 *
 * Peripherals can be configured at least as transmit or receive. Some
 * peripherals also support bidirectional.
 *
 * Notably the stm32 SPI/I2S peripheral is unidirectional and can be configured to both receive and
 * transmit audio.
 *
 * @file
 * @brief       Definitions low-level I2S driver interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef PERIPH_I2S_H
#define PERIPH_I2S_H

#include <stddef.h>
#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "clist.h"
#include "architecture.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Default I2S device access macro
 */
#ifndef I2S_DEV
#define I2S_DEV(x)         (x)
#endif

/**
 * Alignment required by the DMA for the I2S buffers
 */
#ifndef I2S_CPU_DMA_ALIGNMENT
#define I2S_CPU_DMA_ALIGNMENT   1
#endif

/**
 * Allocation requirements from the CPU DMA
 *
 * Usually some limitations on alignment of the data in memory
 */
#ifndef I2S_CPU_DMA_REQUIREMENTS
#define I2S_CPU_DMA_REQUIREMENTS __attribute__((aligned(I2S_CPU_DMA_ALIGNMENT)))
#endif

/**
 * I2S buffer data type
 */
#define i2s_buf_t   I2S_CPU_DMA_REQUIREMENTS uint8_t

/**
 * I2S transaction struct
 */
typedef struct {
    clist_node_t node;      /**< List of transactions */
    i2s_buf_t *buf;         /**< Ptr to the buffer containing the samples */
} i2s_transaction_t;

/**
 * @brief   Default type for I2S devices
 */
#ifndef HAVE_I2S_T
typedef unsigned int i2s_t;
#endif

/**
 * I2S function result values
 */
enum {
    I2S_OK = 0,          /**< No error */
    I2S_NODEV = -1,      /**< I2S device doesn't exist */
    I2S_NOCONF = -2,     /**< I2S device is not configured */
    I2S_NOWIDTH = -3,    /**< The requested bit width is not available */
    I2S_NOCHANNEL = -4,  /**< Requested number of channels is not available */
    I2S_NODIR = -5,      /**< Requested direction is not available on the device */
    I2S_NODATA = -6,     /**< No transactions supplied before starting */
    I2S_NOTRUNNING = -7, /**< No transactions supplied before starting */
};

/**
 * I2S mode definition for controller and target
 */
typedef enum {
    I2S_MODE_CONTROLLER,    /**< Configure I2S peripheral in controller mode */
    I2S_MODE_TARGET,        /**< Configure I2S peripheral in target mode */
} i2s_mode_t;

/**
 * I2S runtime state
 */
typedef enum {
    I2S_STATE_DISABLED,     /**< Interface is initialized, but not configured */
    I2S_STATE_READY,        /**< Interface is configured and ready to start */
    I2S_STATE_RUNNING,      /**< Interface is running and transmitting and/or receiving data */
    I2S_STATE_DRAINING,     /**< Interface is draining the queue to stop */
    I2S_STATE_ERROR,        /**< Interface is stuck in error mode */
} i2s_state_t;

/**
 * I2S data transmission direction
 *
 * Peripherals must support both receive and transmit, and might support both directions
 */
typedef enum {
    I2S_DIRECTION_NONE,     /**< No transfer (clock only) */
    I2S_DIRECTION_RECEIVE,  /**< Receive audio data */
    I2S_DIRECTION_TRANSMIT, /**< Transmit audio data */
    I2S_DIRECTION_BOTH,     /**< Both receive and transmit data (might not be supported by devices) */
} i2s_direction_t;

/**
 * I2S configuration struct
 */
typedef struct {
    i2s_mode_t mode;            /**< I2S control mode */
    i2s_direction_t direction;  /**< Receive, transmit or both */
    uint32_t frame_clock;       /**< Requested frame clock */
    size_t transaction_bytes;    /**< Number of bytes in a single transaction */
    uint8_t word_size;          /**< size in bits of a word */
    uint8_t channels;           /**< Number of channels in a frame */
} i2s_config_t;

/**
 * I2S events
 */
typedef enum {
    I2S_EVENT_NONE = 0,             /**< No event */
    I2S_EVENT_STOPPED,              /**< Queue drained and peripheral stopped */
    I2S_EVENT_ERR_TX_UDR,           /**< TX underrun error */
    I2S_EVENT_ERR_RX_OVR,           /**< RX Overrun error */
    I2S_EVENT_ERR_FRAME,            /**< Unexpected frame switch */
} i2s_event_t;

/**
 * @brief   Signature for transaction complete interrupt callback
 *
 * @note Must be assumed to be called in interrupt context
 *
 * @param[in] arg           context to the callback (optional)
 */
typedef void(*i2s_transaction_cb_t)(i2s_t dev, i2s_transaction_t *transaction, void *arg);

typedef void(*i2s_event_cb_t)(i2s_t dev, i2s_state_t state, i2s_event_t event, void *arg);

/**
 * @brief Set the buffer for a transaction
 *
 * @param transaction   Transaction to set the buffer for
 * @param buf           Buffer to set
 */
static inline void i2s_transaction_set_buffer(i2s_transaction_t *transaction, void *buf)
{
    assert(HAS_ALIGNMENT_OF(buf, I2S_CPU_DMA_ALIGNMENT));
    transaction->buf = buf;
}

/**
 * @brief Initialise an I2S peripheral
 *
 * @note Do not forget to also configure the peripheral via @ref i2s_configure
 *
 * @param   dev         I2S peripheral to initialise
 * @param   event_cb    Event callback called on state changes
 * @param   complete_cb Transaction completed callback
 * @param   arg         Extra context argument passed with the callbacks
 *
 * @return  0 on success, negative on error
 */
int i2s_init(i2s_t dev, i2s_event_cb_t event_cb, i2s_transaction_cb_t complete_cb, void *arg);

/**
 * @brief Reconfigure the I2S interface properties
 *
 * @note peripheral must not be running while reconfiguring
 *
 * @param   dev         I2S peripheral to (re)configure
 * @param   config      Configuration struct
 *
 * @return  negative return value on error
 */
int i2s_configure(i2s_t dev, const i2s_config_t *config);

/**
 * @brief Get the current configuration of an I2S peripheral
 *
 * @param   dev         I2S peripheral to get the configuration for
 * @param   config      Configuration struct to write the configuration in
 *
 * @return  I2S_OK on success
 * @return  I2S_NOCONF if the peripheral is not yet configured
 */
int i2s_get_configuration(i2s_t dev, i2s_config_t *config);

/**
 * @brief Get the current frame clock of the I2S device
 *
 * @param   dev         I2S device
 *
 * @return              The frame clock configured
 */
uint32_t i2s_get_frame_clock(i2s_t dev);

/**
 * @brief Get the current word size of the I2S device
 *
 * @param   dev         I2S device
 *
 * @return              The word size in bits
 */
uint8_t i2s_get_word_size(i2s_t dev);

/**
 * @brief Start the I2S stream.
 *
 * In controller mode this will start the clocks and will immediately start
 * transferring the available transactions. In target mode this will ready the
 * transactions for when the bus controller starts the clocks.
 *
 * @note    A data transaction must be configured before starting the device.
 *
 * @param   dev     I2S device to start
 *
 * @return          zero on success
 */
int i2s_start(i2s_t dev);

/**
 * @brief   Complete the remaining transactions and gracefully stop after.
 *
 * @param   dev     I2S device to drain the transmission queue for
 */
int i2s_drain(i2s_t dev);

/**
 * @brief   Stop the I2S stream immediately without processing the remaining queued transactions.
 *
 * @param   dev     I2S device to stop
 */
int i2s_stop(i2s_t dev);

/**
 * @brief Add a data transaction to the device
 *
 * @param   dev         I2S device
 * @param   transaction Transaction to add to the queue
 */
void i2s_add_transaction(i2s_t dev, i2s_transaction_t *transaction);

/**
 * @brief return the number of frames transmitted since the previous start.
 *
 * @param   dev         I2S device
 *
 * @return              The number of samples transmitted
 */
size_t i2s_frames_transmitted(i2s_t dev);

/**
 * @brief Write a number of samples into a pre-configured transaction
 *
 * @param   dev         I2S device for which the transaction is used
 * @param   source      Source buffer with samples
 * @param   bytes       Number of bytes in the @p source buffer
 * @param   transaction Transaction to fill from the source buffer
 * @param   offset      Offset in bytes to start writing into the transaction buffer
 *
 * @return  Number of bytes consumed from the @p source
 */
size_t i2s_write_transaction_buffer(i2s_t dev, const void *source, size_t bytes, i2s_transaction_t *transaction, size_t offset);

/**
 * @brief read a number of samples from a used transaction
 *
 * @param   dev         I2S device for which the transaction was used
 * @param   dest        Destination buffer for samples
 * @param   bytes       Maximum number of bytes to write into @p dest buffer
 * @param   transaction Transaction to fill the @p dest buffer with
 * @param   offset      Offset in bytes to start reading from the @p transaction buffer
 *
 * @return  Number of bytes consumed from the @p transaction
 */
size_t i2s_read_transaction_buffer(i2s_t dev, void *dest, size_t bytes, i2s_transaction_t *transaction, size_t offset);

/**
 * @brief Calculate the number of bytes needed for a single frame
 *
 * @param word_size Number of bits in a single word
 * @param channels  Number of channels in the frame
 *
 * @return The number of bytes needed for a single frame
 */
static inline size_t i2s_bytes_per_frame(uint8_t word_size, uint8_t channels)
{
    return ((channels * word_size) / 8);
}
/**
 * @brief Calculate the number of frames in a byte buffer
 *
 * @param num_bytes Number of bytes in the buffer
 * @param word_size Number of bits in a single word
 * @param channels  Number of channels in the frame
 *
 * @return The number of frames (rounded down) in the byte buffer
 */
static inline size_t i2s_bytes_to_frames(size_t num_bytes, uint8_t word_size, uint8_t channels)
{
    return num_bytes / i2s_bytes_per_frame(word_size, channels);
}

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_I2S_H */
/** @} */
