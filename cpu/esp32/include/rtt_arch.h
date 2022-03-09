/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture specific RTT functions for ESP32
 *
 * The RTT peripheral driver implements a 32-bit RTT counter with a frequency
 * of 32.768 kHz. It uses either
 *
 * - the 48-bit RTC counter if an external 32.678 kHz crystal is connected or
 * - the 64-bit microsecond system timer.
 *
 * For this purpose, a hardware abstraction layer is defined by a driver
 * interface of the type rtt_hw_driver_t, which generally provides a
 * 48-bit RTC counter with a frequency of 32.678 kHz. This way the RTT
 * implementation always sees a 48-bit counter with a frequency of 32.768 kHz
 * regardless of which hardware implementation is actually used.
 *
 * If pseudomodule `esp_rtc_timer_32` is enabled by the board definition and
 * the 32.768 kHz crystal is actually connected, the 48-bit RTC counter
 * is used. Otherwise, the 64 bit microsecond system timer is used.
 *
 * Since the 64-bit microsecond system timer does not work during light/deep
 * or during a reboot, the status of the 64-bit microsecond system timer is
 * saved in RTC memory before entering a sleep mode or reboot. When leaving
 * the sleep mode or after a reboot, it will be updated from the RTC counter.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef RTT_ARCH_H
#define RTT_ARCH_H

#include "periph/rtt.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum value of the 48-bit RTT hardware counter
 */
#define RTT_HW_COUNTER_MAX  ((1ULL << 48) - 1)

/**
 * @brief   RTT hardware abstraction layer driver
 */
typedef struct {
    /**
     * @brief       Init the current hardware counter
     */
    void (*init)(void);

    /**
     * @brief       Get the current hardware counter value
     * @return      48-bit counter value with a frequency of 32.768 kHz
     */
    uint64_t (*get_counter)(void);

    /**
     * @brief       Set the hardware alarm
     * @param[in]   alarm   alarm time as 32.768 kHz ticks
     * @param[in]   cb      function called on alarm interrupt
     * @param[in]   arg     argument used as parameter for the @p cb function
     */
    void (*set_alarm)(uint32_t alarm, rtt_cb_t cb, void *arg);

    /**
     * @brief       Clear the hardware alarm
     */
    void (*clear_alarm)(void);

    /**
     * @brief       Save the counter value before sleep or reboot if necessary
     */
    void (*save_counter)(void);

    /**
     * @brief       Restore the counter value before sleep or reboot
     * @param[in]   in_init true if function is called after deep sleep or
     *              reboot, false otherwise
     */
    void (*restore_counter)(bool in_init);

    /**
     * @brief       Enable the RTT hardware counter
     */
    void (*poweron)(void);

    /**
     * @brief       Disable the RTT hardware counter
     */
    void (*poweroff)(void);

} rtt_hw_driver_t;

/**
 * @brief   Called before the power management enters a light or deep sleep mode
 * @param   mode    sleep mode that is entered
 * @return          time to sleep in us
 */
uint64_t rtt_pm_sleep_enter(unsigned mode);

/**
 * @brief   Called after the power management left light sleep mode
 * @param   cause   wake-up cause
 */
void rtt_pm_sleep_exit(uint32_t cause);

#ifdef __cplusplus
}
#endif

#endif /* RTT_ARCH_H */
/** @} */
