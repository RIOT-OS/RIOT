/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         pkg_semtech-loramac
 * @brief           Semtech LoRaMAC timer compatibility definitions
 * @{
 *
 * @file
 *
 * @author          José Ignacio Alamos <jialamos@uc.cl>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author          Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef SEMTECH_LORAMAC_TIMER_H
#define SEMTECH_LORAMAC_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ztimer.h"
#include "msg.h"

#include "semtech_loramac.h"

/**
 * @brief   Timer object description
 */
typedef struct TimerEvent_s {
    uint32_t timeout;   /**< Timer timeout in us */
    uint8_t running;    /**< Check if timer is running */
    ztimer_t dev;       /**< ztimer instance attached to this LoRaMAC timer */
    msg_t msg;          /**< message attacher to this LoRaMAC timer */
    void (*cb)(void);   /**< callback to call when timer timeout */
} TimerEvent_t;

/**
 * @brief   Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint32_t TimerTime_t;
#endif

/**
 * @brief   Initializes the timer object
 *
 * @remark  TimerSetValue function must be called before starting the timer.
 *          this function initializes timestamp and reload value at 0.
 *
 * @param[in] obj          Structure containing the timer object parameters
 * @param[in] callback     Function callback called at the end of the timeout
 */
void TimerInit(TimerEvent_t *obj, void (*callback)(void));

/**
 * @brief   Timer IRQ event handler
 */
void TimerIrqHandler(void);

/**
 * @brief   Starts and adds the timer object to the list of timer events
 *
 * @param[in] obj Structure containing the timer object parameters
 */
void TimerStart(TimerEvent_t *obj);

/**
 * @brief   Stops and removes the timer object from the list of timer events
 *
 * @param[in] obj Structure containing the timer object parameters
 */
void TimerStop(TimerEvent_t *obj);

/**
 * @brief   Resets the timer object
 *
 * @param[in] obj Structure containing the timer object parameters
 */
void TimerReset(TimerEvent_t *obj);

/**
 * @brief   Set timer new timeout value
 *
 * @param[in] obj   Structure containing the timer object parameters
 * @param[in] value New timer timeout value
 */
void TimerSetValue(TimerEvent_t *obj, uint32_t value);

/**
 * @brief  Read the current time
 *
 * @return current time
 */
TimerTime_t TimerGetCurrentTime(void);

/**
 * @brief   Return the time elapsed since a fix moment in Time
 *
 * @param[in] time    fix moment in Time
 * @return    elapsed time
 */
TimerTime_t TimerGetElapsedTime(TimerTime_t time);

/**
 * @brief   Return the time elapsed since a fix moment in time
 *
 * @param[in] time         fix moment in the future
 * @returns                difference between now and future event
 */
TimerTime_t TimerGetFutureTime(TimerTime_t time);

/**
 * @brief   Manages the entry into low power mode
 */
void TimerLowPowerHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* SEMTECH_LORAMAC_TIMER_H */
/** @} */
