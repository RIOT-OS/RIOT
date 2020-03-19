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
 * @brief       Architecture specific RTC functions for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef RTC_ARCH_H
#define RTC_ARCH_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Called before the power management enters a light or deep sleep mode
 * @param   mode    sleep mode that is entered
 * @return          time to sleep in us
 */
uint64_t rtc_pm_sleep_enter(unsigned mode);

/**
 * @brief   Called after the power management left light sleep mode
 * @param   cause   wake-up cause
 */
void rtc_pm_sleep_exit(uint32_t cause);

#ifdef __cplusplus
}
#endif

#endif /* RTC_ARCH_H */
