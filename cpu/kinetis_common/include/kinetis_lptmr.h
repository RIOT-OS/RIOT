/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     kinetis
 * @{
 *
 * @file
 * @brief       Kinetis Low-Power Timer driver definition
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#ifndef __KINETIS_LPTMR_H
#define __KINETIS_LPTMR_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The definition of the LPTMR devices available. This can be expanded if
 * devices with multiple LPTMR units should be supported.
 */
typedef enum {
    KINETIS_LPTMR0 = 0
} kinetis_lptmr_t;

/**
 * The prescale values that will be used for the lptmr init
 */
typedef enum {
    KINETIS_LPTMR_PRE_DISABLE = -1,
    KINETIS_LPTMR_PRE_2     = 0,
    KINETIS_LPTMR_PRE_4     = 1,
    KINETIS_LPTMR_PRE_8     = 2,
    KINETIS_LPTMR_PRE_16    = 3,
    KINETIS_LPTMR_PRE_32    = 4,
    KINETIS_LPTMR_PRE_64    = 5,
    KINETIS_LPTMR_PRE_128   = 6,
    KINETIS_LPTMR_PRE_256   = 7,
    KINETIS_LPTMR_PRE_512   = 8,
    KINETIS_LPTMR_PRE_1024  = 9,
    KINETIS_LPTMR_PRE_2048  = 10,
    KINETIS_LPTMR_PRE_4096  = 11,
    KINETIS_LPTMR_PRE_8192  = 12,
    KINETIS_LPTMR_PRE_16384 = 13,
    KINETIS_LPTMR_PRE_32768 = 14,
    KINETIS_LPTMR_PRE_65536 = 15,
} kinetis_lptmr_prescale_t;

/**
 * @brief The function that will be called when the interrupt fired
 *
 * @param[in]   value   The current value of the timer
 * @param[in]   arg     The argument you passed when setting the callback
 */
typedef void (*kinetis_lptmr_cb_t)(uint16_t value, void *arg);

/**
 * @brief Initializes the lptmr with the specified parameters
 *
 * @param[in]   timer       The timer to initialize. Example: KINETIS_LPTMR0
 * @param[in]   clock       The clock to use. It is defined in cpu-conf.h of the implementation cpu
 * @param[in]   prescale    The prescaler configuration to use. See kinetis_pit.h
 * @param[in]   freerunning true if the timer should wrap after overflowing. False if it should wrap
 *                          after being equal to the configured value
 */
void kinetis_lptmr_init(kinetis_lptmr_t timer, kinetis_lptmr_clock_t clock, kinetis_lptmr_prescale_t prescale, bool freerunning);

/**
 * @brief Sets the callback of the lptmr interrupt
 *
 * @param[in]   timer       The timer to set the callback for
 * @param[in]   callback    The function to call.
 * @param[in]   argument    The argument to pass to the callback
 */
void kinetis_lptmr_set_callback(kinetis_lptmr_t timer, kinetis_lptmr_cb_t callback, void *argument);

/**
 * @brief Enable the interrupt for the lptmr
 *
 * @param[in]   timer       The timer to enable the interrupt for
 */
void kinetis_lptmr_enable_interrupt(kinetis_lptmr_t timer);

/**
 * @brief Disable the interrupt for the lptmr
 *
 * @param[in]   timer       The timer to enable the interrupt for
 */
void kinetis_lptmr_disable_interrupt(kinetis_lptmr_t timer);

/**
 * @brief Read the value of a timer
 *
 * @param[in]  timer  The timer to read the value of
 *
 * @return     The current value of the timer
 */
uint16_t kinetis_lptmr_read(kinetis_lptmr_t timer);

/**
 * @brief Read the remaining number of ticks until the timer fires
 *
 * @param[in]   timer      The timer that should be read
 * @param[out]  remaining  A pointer to the variable where the value should be stored
 *
 * @return      A status code of 0 if the operation was successful
 *              1 if it wasn't because no timer was set
 */
int kinetis_lptmr_get_remaining(kinetis_lptmr_t timer, uint16_t *remaining);

/**
 * @brief Set an absolute value to the timer.
 *
 * @note This is not guaranteed to actually set the specified value; when
 *       the timer has currently a value set it must be stopped to set a new
 *       one. Stopping the timer will also reset the timer register and there
 *       is no way for us to to prevent this.
 *
 * @param[in]  timer  The timer to set
 * @param[in]  value  The value the timer should be set to
 */
void kinetis_lptmr_set_absolute(kinetis_lptmr_t timer, uint16_t value);

/**
 * @brief Set the timer to the offset value specified
 *
 * @param[in]  timer   The timer to set
 * @param[in]  offset  The offset to set the timer to
 */
void kinetis_lptmr_set_offset(kinetis_lptmr_t timer, uint16_t offset);

/**
 * @brief Clears a timer
 *
 * @param[in]  timer  The timer to clear
 */
void kinetis_lptmr_clear(kinetis_lptmr_t timer);


#ifdef __cplusplus
}
#endif

#endif /* __KINETIS_LPTMR_H */
