/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech
Description: Timer objects and scheduling management
License: Revised BSD License, see LICENSE.TXT file include in the project
Maintainer: Miguel Luis and Gregory Cristian
*/
#ifndef __TIMER_H__
#define __TIMER_H__

#include "xtimer.h"

/*!
 * \brief Timer object description
 */
typedef struct TimerEvent_s
{
    xtimer_t dev;
    msg_t  msg;
    kernel_pid_t pid;
}TimerEvent_t;

/*!
 * \brief Timer time variable definition
 */
#ifndef TimerTime_t
typedef uint64_t TimerTime_t;
#endif

/*!
 * \brief Initializes the timer object
 *
 * \remark TimerSetValue function must be called before starting the timer.
 *         this function initializes timestamp and reload value at 0.
 *
 * \param [IN] obj          Structure containing the timer object parameters
 * \param [IN] callback     Function callback called at the end of the timeout
 */
//void TimerInit( TimerEvent_t *obj, void ( *cb )( void ), kernel_pid_t target_pid);

/*!
 * Timer IRQ event handler
 */
//void TimerIrqHandler( void );

/*!
 * \brief Starts and adds the timer object to the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
//void TimerStart( TimerEvent_t *obj, uint8_t opt);

/*!
 * \brief Stops and removes the timer object from the list of timer events
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
//void TimerStop( TimerEvent_t *obj );

/*!
 * \brief Resets the timer object
 *
 * \param [IN] obj Structure containing the timer object parameters
 */
//void TimerReset( TimerEvent_t *obj, uint8_t opt);

/*!
 * \brief Set timer new timeout value
 *
 * \param [IN] obj   Structure containing the timer object parameters
 * \param [IN] value New timer timeout value
 */
//void TimerSetValue( TimerEvent_t *obj, uint32_t value, uint8_t message_ct);

/*!
 * \brief Read the current time
 *
 * \retval time returns current time
 */
//TimerTime_t TimerGetCurrentTime( void );

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \param [IN] savedTime    fix moment in Time
 * \retval time             returns elapsed time
 */
//TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime );

/*!
 * \brief Return the Time elapsed since a fix moment in Time
 *
 * \param [IN] eventInFuture    fix moment in the future
 * \retval time             returns difference between now and future event
 */
//TimerTime_t TimerGetFutureTime( TimerTime_t eventInFuture );

/*!
 * \brief Manages the entry into ARM cortex deep-sleep mode
 */
//void TimerLowPowerHandler( uint32_t time );

#endif  // __TIMER_H__
