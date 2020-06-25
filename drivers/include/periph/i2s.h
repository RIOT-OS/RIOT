/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2020 Inria
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
 *   often included in implementations.
 *
 * # Design goals
 *
 * The I2S bus requires a continuous stream of data when enabled.
 *
 *
 * # Details
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
 * transactions. If DMA is not available the CPU must wake up twice to four
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
 * The organization of the transaction memory must be suitable for consumption by
 * the I2S peripherals.
 *
 * 8 bit data must be organized as:
 *
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
 *
 * 16 bit samples must be organized as:
 *
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
 *
 * 24 bit samples must be organized as:
 *
 * 32                          0
 * +------+--------------------+
 * | SIGN | L0                 |
 * +------+--------------------+
 * | SIGN | R0                 |
 * +------+--------------------+
 * | SIGN | ...                |
 * +------+--------------------+
 * | SIGN | LN                 |
 * +------+--------------------+
 * | SIGN | RN                 |
 * +------+--------------------+
 *
 * 32 bit samples must be organized as:
 *
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
 *
 * ## Frequency
 *
 * The sample rate of the audio produced by the I2S bus matches the frequency of
 * the word clock configured. From this clock both the bit clock and the master
 * clock are derived. The bit clock is defined as twice the bits per channel and
 * the master clock is defined as 256 times the frequency of the word clock.
 *
 * It is not always possible to perfectly match the requested sample rate on a
 * device. When configuring a sample rate, the device will return the actual
 * sample rate configured.
 *
 * TODO: Configure closest matching frequency or the highest frequency lower
 * than the one requested?
 *
 * @file
 * @brief       Definitions low-level I2S driver interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef PERIPH_I2S_H
#define PERIPH_I2S_H

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default I2S device access macro
 */
#ifndef I2S_DEV
#define I2S_DEV(x)         (x)
#endif

/**
 * @brief   Default type for I2S devices
 */
#ifndef HAVE_I2S_T
typedef unsigned int i2s_t;
#endif

typedef enum {
    I2S_OK = 0,         /**< Everything OK */
    I2S_NODEV = -1,     /**< I2S device doesn't exist */
    I2S_NOWIDTH = -2,   /**< The requested bit width is not available */
    I2S_NOMODE = -3,    /**< The requested mode (master/slave) is not available */
}

typedef enum {
    I2S_MODE_MASTER,    /**< Configure I2S peripheral in master mode */
    I2S_MODE_SLAVE,     /**< Configure I2S peripheral in slave mode */
} i2s_mode_t;

/**
 * @brief   Signature for transaction complete interrupt callback
 *
 * @param[in] arg           context to the callback (optional)
 */
typedef void(*i2s_complete_cb_t)(i2s_t dev, void *arg);

typedef void(*i2s_event_cb_t)(i2s_t dev, void *arg);

/**
 * @brief   Signature for pointer update required interrupt callback
 *
 * @param[in] arg   context to the callback (optional)
 */
typedef void(*i2s_ptr_req_cb_t)(void *arg);

/**
 * @brief   Interrupt context for an I2S device
 *
 * TODO: what was this again?
 */
#ifndef HAVE_I2S_ISR_CTX_T
typedef struct {
    i2s_complete_cb_t complete_cb;     /**< data received interrupt callback */
    i2s_ptr_req_cb_t  ptr_req_cb;
    void *arg;              /**< argument to both callback routines */
} i2s_isr_ctx_t;
#endif

int i2s_init(i2s_t dev, uint32_t frequency, size_t width, i2s_mode_t mode,
             i2s_event_cb_t cb, void *arg);

void i2s_set_ptr(i2s_t i2s, const uint8_t *ptr);

/**
 * @brief Configure the sample rate of the I2S device
 *
 * @note    Due to device and clock limitations, the requested sample rate might
 *          not match the resulting sample rate in the device
 *
 * @warning The I2S device must be stopped before switching sample rates
 *
 * @param   dev         I2S device
 * @param   sample_rate Sample rate to configure
 *
 * @returns             The resulting sample rate.
 */
uint32_t i2s_set_sample_rate(i2s_t dev, uint32_t sample_rate);

/**
 * @brief Get the current sample rate of the I2S device
 *
 * @param   dev         I2S device
 *
 * @return              The sample rate configured
 */
uint32_t i2s_get_sample_rate(i2s_t dev);

/**
 * @brief   Start the I2S stream.
 *
 * In master mode this will start the clocks and will immediately start
 * transferring the available transactions. In slave mode this will ready the
 * transactions for when the bus master starts the clocks.
 *
 * @note    A data transaction must be configured before starting the device
 */
void i2s_start(i2s_t dev);

/**
 * @brief   Stop the I2S stream.
 *
 * In master mode this will stop the transfer after finishing the current
 * transaction. In slave mode TBD.
 */
void i2s_stop(i2s_t dev);

/**
 * @brief Add a data transaction to the device
 */
void i2s_add_transaction(i2s_t dev, const int32_t *data, size_t len);

/**
 * @brief return the number of samples remaining for the current transfer.
 *
 * This counts both left and right channels as separate samples.
 *
 * @param   dev         I2S device
 *
 * @return              The number of remaining samples
 */
size_t i2s_remaining(i2s_t dev);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_I2S_H */
