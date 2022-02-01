/*
 * Copyright (C) 2022 Gunar Schorcht
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
 * @brief       Wrapper for source code compatibility of ESP-IDF log with RIOT's log module
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef LOG_ESP_LOG_H
#define LOG_ESP_LOG_H

#ifndef DOXYGEN     /* Hide implementation details from doxygen */

#ifdef __cplusplus
extern "C" {
#endif

#include_next "esp_log.h"

#if defined(RIOT_VERSION)

#include "esp_common.h"

#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL LOG_LEVEL
#endif

#define ESP_LOG_LEVEL(level, tag, format, ...) \
            do { \
                if ((esp_log_level_t)level==ESP_LOG_ERROR ) { \
                    LOG_TAG(level, E, tag, format, ##__VA_ARGS__);  \
                } \
                else if ((esp_log_level_t)level==ESP_LOG_WARN ) { \
                    LOG_TAG(level, W, tag, format, ##__VA_ARGS__);  \
                } \
                else if ((esp_log_level_t)level==ESP_LOG_INFO ) { \
                    LOG_TAG(level, I, tag, format, ##__VA_ARGS__);  \
                } \
                else if ((esp_log_level_t)level==ESP_LOG_DEBUG ) { \
                    LOG_TAG(level, D, tag, format, ##__VA_ARGS__);  \
                } \
                else if ((esp_log_level_t)level==ESP_LOG_VERBOSE ) { \
                    LOG_TAG(level, V, tag, format, ##__VA_ARGS__);  \
                } \
            } while (0)

#define ESP_LOG_LEVEL_LOCAL(level, tag, format, ...) \
            do {               \
                if ( LOG_LOCAL_LEVEL >= level ) { \
                    ESP_LOG_LEVEL(level, tag, format, ##__VA_ARGS__); \
                } \
            } while (0)

#endif /* defined(RIOT_VERSION) */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* LOG_ESP_LOG_H */
