/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Wrapper for source code compatibility of ESP-IDF log with RIOT's log module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
#include_next "esp_log.h"

#if defined(RIOT_VERSION)

#  include "esp_common.h" /* includes esp_common_log.h */

#  ifndef LOG_LOCAL_LEVEL
#    define LOG_LOCAL_LEVEL LOG_LEVEL
#  endif

#  define ESP_LOG_LEVEL(level, unit, format, ...) \
    LOG_WITH_UNIT(level, unit, format "\n", ##__VA_ARGS__)

#  define ESP_LOGE(tag, format, ...) ESP_LOG_LEVEL(LOG_ERROR,   tag, format, ##__VA_ARGS__)
#  define ESP_LOGW(tag, format, ...) ESP_LOG_LEVEL(LOG_WARNING, tag, format, ##__VA_ARGS__)
#  define ESP_LOGI(tag, format, ...) ESP_LOG_LEVEL(LOG_INFO,    tag, format, ##__VA_ARGS__)
#  define ESP_LOGD(tag, format, ...) ESP_LOG_LEVEL(LOG_DEBUG,   tag, format, ##__VA_ARGS__)
#  define ESP_LOGV(tag, format, ...) ESP_LOG_LEVEL(LOG_ALL,     tag, format, ##__VA_ARGS__)

#  define ESP_LOG_LEVEL_LOCAL(level, unit, format, ...) \
    LOG_WITH_UNIT(level, unit, format, ##__VA_ARGS__)

#  define esp_log_write_rom(format, ...) esp_rom_printf(DRAM_STR(format), ##__VA_ARGS__)
#  define esp_log_write_formatted_rom(level, unit, ...) \
    ESP_LOG_FORMAT(esp_log_write_rom, level, unit, __VA_ARGS__)
#  define ESP_DRAM_LOG_LEVEL(level, unit, ...) \
    LOG_IMPL(esp_log_write_formatted_rom, level, unit, __VA_ARGS__)

#  define ESP_DRAM_LOGE(tag, format, ...) ESP_DRAM_LOG_LEVEL(LOG_ERROR  , tag, format "\n", ##__VA_ARGS__)
#  define ESP_DRAM_LOGW(tag, format, ...) ESP_DRAM_LOG_LEVEL(LOG_WARNING, tag, format "\n", ##__VA_ARGS__)
#  define ESP_DRAM_LOGI(tag, format, ...) ESP_DRAM_LOG_LEVEL(LOG_INFO   , tag, format "\n", ##__VA_ARGS__)
#  define ESP_DRAM_LOGD(tag, format, ...) ESP_DRAM_LOG_LEVEL(LOG_DEBUG  , tag, format "\n", ##__VA_ARGS__)
#  define ESP_DRAM_LOGV(tag, format, ...) ESP_DRAM_LOG_LEVEL(LOG_ALL    , tag, format "\n", ##__VA_ARGS__)

#endif /* defined(RIOT_VERSION) */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
