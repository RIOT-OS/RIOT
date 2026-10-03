/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "log.h"
#include "ansi_style.h"

#include "rom/ets_sys.h"

/**
 * @addtogroup  cpu_esp_common
 * @{
 *
 * @file
 * @brief       Common log macros for ESP SoCs
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#if defined(MODULE_ESP_LOG_COLORED)
#  define ESP_LOG_RESET_COLOR   ANSI_STYLE_RESET
#  define ESP_LOG_COLOR_LOG_ERROR   ANSI_STYLE(FG(RED), BOLD)
#  define ESP_LOG_COLOR_LOG_WARNING       ANSI_STYLE(FG(YELLOW), BOLD)
#  define ESP_LOG_COLOR_LOG_INFO       ANSI_STYLE(BOLD)
#  define ESP_LOG_COLOR_LOG_DEBUG       ANSI_STYLE(GREEN)
#  define ESP_LOG_COLOR_LOG_ALL         
#else /* MODULE_ESP_LOG_COLORED */
#  define ESP_LOG_RESET_COLOR
#  define ESP_LOG_COLOR_LOG_ERROR
#  define ESP_LOG_COLOR_LOG_WARNING
#  define ESP_LOG_COLOR_LOG_INFO
#  define ESP_LOG_COLOR_LOG_DEBUG
#  define ESP_LOG_COLOR_LOG_ALL
#endif /* MODULE_ESP_LOG_COLORED */

#ifndef LOG_RESET_COLOR
#  define LOG_RESET_COLOR ESP_LOG_RESET_COLOR
#  define LOG_COLOR_E ESP_LOG_COLOR_LOG_ERROR
#  define LOG_COLOR_W ESP_LOG_COLOR_LOG_WARNING
#  define LOG_COLOR_I ESP_LOG_COLOR_LOG_INFO
#  define LOG_COLOR_D ESP_LOG_COLOR_LOG_DEBUG
#  define LOG_COLOR_V ESP_LOG_COLOR_LOG_ALL
#endif

#define ESP_LOG_LETTER_LOG_ERROR "E"
#define ESP_LOG_LETTER_LOG_WARNING "W"
#define ESP_LOG_LETTER_LOG_INFO "I"
#define ESP_LOG_LETTER_LOG_DEBUG "D"
#define ESP_LOG_LETTER_LOG_VERBOSE "V"

#if defined(MODULE_ESP_LOG_TAGGED)
#    define ESP_LOG_FORMAT_DEFAULT(level, unit, format, ...) \
        ESP_LOG_COLOR_ ## level \
        ESP_LOG_LETTER_ ## level \
        " (%" PRIu32 ") [%s] " format ESP_LOG_RESET_COLOR, \
        system_get_time_ms(), unit, ##__VA_ARGS__
#else
#    define ESP_LOG_FORMAT_DEFAULT(level, unit, format, ...) \
        ESP_LOG_COLOR_ ## level \
        format ESP_LOG_RESET_COLOR, \
        ##__VA_ARGS__
#endif

#if defined(LOG_FORMAT)
#  define ESP_LOG_FORMAT LOG_FORMAT
#else 
#  define ESP_LOG_FORMAT ESP_LOG_FORMAT_DEFAULT
#endif

#define esp_log_write_early(level, unit, ...) \
    ets_printf(ESP_LOG_FORMAT(level, unit, __VA_ARGS__))

#define LOG_TAG_EARLY(level, _letter, unit, ...) \
    LOG_IMPL(esp_log_write_early, level, unit, __VA_ARGS__)

#define LOG_TAG(level, _letter, tag, format, ...) \
    LOG_WITH_UNIT(level, tag, format, ##__VA_ARGS__)


/** Tagged LOG_* definitions */
#define LOG_TAG_ERROR(tag, format, ...)   LOG_TAG(LOG_ERROR, E, tag, format, ##__VA_ARGS__)
#define LOG_TAG_WARNING(tag, format, ...) LOG_TAG(LOG_WARNING, W, tag, format, ##__VA_ARGS__)
#define LOG_TAG_INFO(tag, format, ...)    LOG_TAG(LOG_INFO, I, tag, format, ##__VA_ARGS__)
#define LOG_TAG_DEBUG(tag, format, ...)   LOG_TAG(LOG_DEBUG, D, tag, format, ##__VA_ARGS__)
#define LOG_TAG_ALL(tag, format, ...)     LOG_TAG(LOG_ALL, V, tag, format, ##__VA_ARGS__)

/** definitions for source code compatibility with ESP-IDF */
#define ESP_EARLY_LOGE(tag, format, ...) LOG_TAG_EARLY(LOG_ERROR, E, tag, format "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGW(tag, format, ...) LOG_TAG_EARLY(LOG_WARNING, W, tag, format "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGI(tag, format, ...) LOG_TAG_EARLY(LOG_INFO, I, tag, format "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGD(tag, format, ...) LOG_TAG_EARLY(LOG_DEBUG, D, tag, format "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGV(tag, format, ...) LOG_TAG_EARLY(LOG_ALL, V, tag, format "\n", ##__VA_ARGS__)

#ifdef CPU_ESP8266
#  define ESP_LOGE(tag, format, ...) LOG_TAG(LOG_ERROR, E, tag, format "\n", ##__VA_ARGS__)
#  define ESP_LOGW(tag, format, ...) LOG_TAG(LOG_WARNING, W, tag, format "\n", ##__VA_ARGS__)
#  define ESP_LOGI(tag, format, ...) LOG_TAG(LOG_INFO, I, tag, format "\n", ##__VA_ARGS__)
#  define ESP_LOGD(tag, format, ...) LOG_TAG(LOG_DEBUG, D, tag, format "\n", ##__VA_ARGS__)
#  define ESP_LOGV(tag, format, ...) LOG_TAG(LOG_ALL, V, tag, format "\n", ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */

/** @} */
