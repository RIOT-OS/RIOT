/*
 * Copyright (C) 2017 Gilles DOFFE <gdoffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_qdec Quadrature Decoder (QDEC)
 * @ingroup     drivers_periph
 * @brief       Low-level QDEC peripheral driver
 *
 * This file was inspired by pwm.h written by :
 *  Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * QDEC interface enables access to CPU peripherals acquiring quadrature
 * signals. On most platforms, this interface will be implemented based
 * on hardware timers.
 *
 * A quadrature encoder outputs two wave forms that are 90 degrees
 * out of phase.
 *
 * @verbatim
 *
 * Clockwise (C) :              Counter-clockwise (CC) :
 *  ___________________          ___________________
 * | Phase |  A  |  B  |        | Phase |  A  |  B  |
 * |-------------------|        |-------------------|
 * |   1   |  0  |  0  |        |   1   |  0  |  0  |
 * |-------------------|        |-------------------|
 * |   2   |  0  |  1  |        |   2   |  1  |  0  |
 * |-------------------|        |-------------------|
 * |   3   |  1  |  1  |        |   3   |  1  |  1  |
 * |-------------------|        |-------------------|
 * |   4   |  1  |  0  |        |   4   |  0  |  1  |
 *  -------------------          -------------------
 *         __    __                   __    __
 *      __|  |__|  |_               _|  |__|  |__
 *        __    __                     __    __
 *      _|  |__|  |__               __|  |__|  |_
 *
 * These signals are decoded to produce a count up or a count down.
 * On rising or falling edge of one signal, other signal state is checked to
 * determine direction.
 *
 *  1. Rising edge on A and signal B is up       => (C)  => increment counter
 *  2. Rising edge on A and signal B is down     => (CC) => decrement counter
 *  3. Falling edge on A and signal B is up      => (C)  => decrement counter
 *  4. Falling edge on A and signal B is down    => (CC) => increment counter
 *  5. Rising edge on B and signal A is up       => (C)  => decrement counter
 *  6. Rising edge on B and signal A is down     => (CC) => increment counter
 *  7. Falling edge on B and signal A is up      => (C)  => increment counter
 *  8. Falling edge on B and signal A is down    => (CC) => decrement counter
 * @endverbatim
 *
 * According to these cases, three modes are available :
 *  - X1 mode : signal A, rising edges (cases 1 and 2)
 *  - X2 mode : signal A, rising and falling edges (cases 1, 2, 3 and 4)
 *  - X4 mode : signals A and B, rising and falling edges (all cases)
 *
 * The mapping/configuration of QDEC devices (timers) and the used pins has to be
 * done in the board configuration (the board's `periph_conf.h).
 *
 * When using the QDEC interface, first thing you have to do is initialize the
 * QDEC device with the targeted mode. Once the device is initialized,
 * it will start counting quadrature signals on all configured pins
 * immediately.
 *
 * @{
 * @file
 * @brief       Low-level QDEC peripheral driver interface definitions
 *
 * @author      Gilles DOFFE <gdoffe@gmail.com>
 */

#ifndef PERIPH_QDEC_H
#define PERIPH_QDEC_H

#include <stdint.h>
#include <limits.h>

#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default QDEC access macro
 */
#ifndef QDEC_DEV
#define QDEC_DEV(x)          (x)
#endif

/**
 * @brief  Default QDEC undefined value
 */
#ifndef QDEC_UNDEF
#define QDEC_UNDEF           (UINT_FAST8_MAX)
#endif

/**
 * @brief   Default QDEC type definition
 */
#ifndef HAVE_QDEC_T
typedef uint_fast8_t qdec_t;
#endif

/**
 * @brief   Default QDEC mode definition
 */
#ifndef HAVE_QDEC_MODE_T
typedef enum {
    QDEC_X1,    /* X1 mode */
    QDEC_X2,    /* X2 mode */
    QDEC_X4,    /* X4 mode */
} qdec_mode_t;
#endif

/**
 * @brief   Signature of event callback functions triggered from interrupts
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*qdec_cb_t)(void *arg);

/**
 * @brief   Default interrupt context entry holding callback and argument
 */
#ifndef HAVE_TIMER_ISR_CTX_T
typedef struct {
    qdec_cb_t cb;           /**< callback executed from qdec interrupt */
    void *arg;              /**< optional argument given to that callback */
} qdec_isr_ctx_t;
#endif

/**
 * @brief   Initialize a QDEC device
 *
 * The QDEC module is based on virtual QDEC devices.
 * The QDEC devices can be configured to run in three modes :
 *      - X1
 *      - X2
 *      - X4
 * See description above for more details about modes.
 *
 * On QDEC counter overflow, an interrupt is triggered.
 * The interruption calls the callback defined.
 *
 * @param[in] dev           QDEC device to initialize
 * @param[in] mode          QDEC mode : X1, X2 or X4
 * @param[in] cb            Callback on QDEC timer overflow
 * @param[in] arg           Callback arguments
 *
 * @return                  error code on error
 * @return                  0 on success
 */
int32_t qdec_init(qdec_t dev, qdec_mode_t mode, qdec_cb_t cb, void *arg);

/**
 * @brief Read the current value of the given qdec device
 *
 * @param[in] dev           the qdec to read the current value from
 *
 * @return                  the qdecs current value
 */
int32_t qdec_read(qdec_t dev);

/**
 * @brief Read the current value of the given qdec device and reset it
 *
 * @param[in] dev           the qdec to read the current value from
 *
 * @return                  the qdecs current value
 */
int32_t qdec_read_and_reset(qdec_t dev);

/**
 * @brief Start the given qdec timer
 *
 * This function is only needed if the qdec timer was stopped manually before.
 *
 * @param[in] qdec          the qdec device to start
 */
void qdec_start(qdec_t qdec);

/**
 * @brief Stop the given qdec timer
 *
 * This will effect all of the timer's channels.
 *
 * @param[in] qdec          the qdec device to stop
 */
void qdec_stop(qdec_t qdec);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_QDEC_H */
