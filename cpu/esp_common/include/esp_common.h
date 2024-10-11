/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Common helper macros for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef ESP_COMMON_H
#define ESP_COMMON_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
#include "esp_common_log.h"
#include "macros/utils.h"
#include "macros/xtstr.h"

#if !defined(ICACHE_FLASH)
#ifndef ICACHE_RAM_ATTR
/** Places the code with this attribute in the IRAM. */
#define ICACHE_RAM_ATTR  __attribute__((section(".iram0.text")))
#endif
#else /* ICACHE_FLASH */
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif
#endif /* ICACHE_FLASH */

#ifndef RTC_BSS_ATTR
#define RTC_BSS_ATTR __attribute__((section(".rtc.bss")))
#endif

#ifdef CPU_ESP8266
#ifndef RTC_DATA_ATTR
#define RTC_DATA_ATTR __attribute__((section(".rtc.data")))
#endif
#endif

/** Print out a message that function is not yet implemented */
#define NOT_YET_IMPLEMENTED()     LOG_INFO("%s not yet implemented\n", __func__)
/** Print out a message that function is not supported */
#define NOT_SUPPORTED()           LOG_INFO("%s not supported\n", __func__)

#if ENABLE_DEBUG
/**
  * @brief  Parameter check with return a value.
  *
  * If ENABLE_DEBUG is true, the macro checks a condition and returns with a value
  * if the condition is not fulfilled.
  * @param  cond    the condition
  * @param  err     the return value in the case the condition is not fulfilled.
  */
#define CHECK_PARAM_RET(cond, err)  if (!(cond)) \
                                    { \
                                        DEBUG("%s parameter condition (" #cond ") " \
                                              "not fulfilled\n", __func__); \
                                        return err; \
                                    }

/**
 * @brief  Parameter check without return value.
 *
 * If ENABLE_DEBUG is true, the macro checks a condition and returns without a
 * value if the condition is not fulfilled.
 * @param  cond    the condition
 */
#define CHECK_PARAM(cond)   if (!(cond)) \
                            { \
                                DEBUG("%s parameter condition (" #cond ") " \
                                      "not fulfilled\n", __func__); \
                                return; \
                            }

#else /* ENABLE_DEBUG */

#define CHECK_PARAM_RET(cond, err) if (!(cond)) { return err; }
#define CHECK_PARAM(cond)          if (!(cond)) { return; }

#endif /* ENABLE_DEBUG */

/**
  * @brief  function name mappings for source code compatibility with ESP8266 port
  * @{
  */
#ifdef CPU_ESP32
#define system_get_cpu_freq     ets_get_cpu_frequency
#define system_update_cpu_freq  ets_update_cpu_frequency
#endif /* CPU_FAM_ESP32 */
/** @} */

/** @} */

/** microseconds per millisecond */
#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC 1000UL
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC  1000UL
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */

/** @} */
#endif /* ESP_COMMON_H */
