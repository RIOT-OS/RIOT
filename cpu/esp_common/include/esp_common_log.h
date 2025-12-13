/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Common log macros for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "buildinfo/cpu.h"
#include "log.h"

extern uint32_t system_get_time_ms (void);
extern int ets_printf(const char *fmt, ...);

#if MODULE_ESP_LOG_COLORED

#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       "\033[1;31m"
#define LOG_COLOR_W       "\033[1;33m"
#define LOG_COLOR_I       "\033[1m"
#define LOG_COLOR_D       "\033[0;32m"
#define LOG_COLOR_V

#else /* MODULE_ESP_LOG_COLORED */

#define LOG_RESET_COLOR
#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V

#endif /* MODULE_ESP_LOG_COLORED */

#if MODULE_ESP_LOG_TAGGED

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%" PRIu32 ") [%s] " format LOG_RESET_COLOR

#define LOG_TAG(level, letter, tag, format, ...) \
                 do { \
                    if ((unsigned)level <= (unsigned)LOG_LEVEL) { \
                        printf(LOG_FORMAT(letter, format), system_get_time_ms(), tag, ##__VA_ARGS__); \
                        fflush(stdout); \
                    } \
                } while (0)

#define LOG_TAG_EARLY(level, letter, tag, format, ...) \
                do { \
                    if (LOG_LEVEL >= level) { \
                        ets_printf(LOG_FORMAT(letter, format), system_get_time_ms(), tag, ##__VA_ARGS__); \
                    } \
                } while (0)

#else /* MODULE_ESP_LOG_TAGGED */

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter format LOG_RESET_COLOR

#define LOG_TAG(level, letter, tag, format, ...) \
                do { \
                    (void)tag; \
                    if ((unsigned)level <= (unsigned)LOG_LEVEL) { \
                        printf(LOG_FORMAT(letter, format), ##__VA_ARGS__); \
                        fflush(stdout); \
                    } \
                } while (0U)

#define LOG_TAG_EARLY(level, letter, tag, format, ...) \
                do { \
                    (void)tag; \
                    if ((unsigned)level <= (unsigned)LOG_LEVEL) { \
                        ets_printf(LOG_FORMAT(letter, format), ##__VA_ARGS__); \
                    } \
                } while (0U)

#endif /* MODULE_ESP_LOG_TAGGED */

/**
 * Override LOG_* definitions with a tagged version. By default the function
 * name is used as tag.
 */
#ifndef MODULE_LOG_PRINTFNOFORMAT
#undef LOG_ERROR
#undef LOG_INFO
#undef LOG_WARNING
#undef LOG_DEBUG
#define LOG_ERROR(format, ...)   LOG_TAG(LOG_ERROR, E, __func__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) LOG_TAG(LOG_WARNING, W, __func__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)    LOG_TAG(LOG_INFO, I, __func__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)   LOG_TAG(LOG_DEBUG, D, __func__, format, ##__VA_ARGS__)
#endif

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
#define ESP_LOGE(tag, format, ...) LOG_TAG(LOG_ERROR, E, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGW(tag, format, ...) LOG_TAG(LOG_WARNING, W, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGI(tag, format, ...) LOG_TAG(LOG_INFO, I, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGD(tag, format, ...) LOG_TAG(LOG_DEBUG, D, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGV(tag, format, ...) LOG_TAG(LOG_ALL, V, tag, format "\n", ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */

/** @} */
