/*
 * Copyright (C) 2018 Gunar Schorcht
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
 * @brief       Common log macros
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef ESP_COMMON_LOG_H
#define ESP_COMMON_LOG_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

#define LOG_TAG(level, tag, ...) do { \
    if ((level) <= LOG_LEVEL) log_write_tagged(level, tag, __VA_ARGS__); } while (0U)

/**
 * Override LOG_* definitions with a tagged version. By default the function
 * name is used tag.
 */
#undef LOG_ERROR
#undef LOG_INFO
#undef LOG_WARNING
#undef LOG_DEBUG
#define LOG_ERROR(fmt, ...)   LOG_TAG(LOG_ERROR  , __func__, fmt, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) LOG_TAG(LOG_WARNING, __func__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)    LOG_TAG(LOG_INFO   , __func__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   LOG_TAG(LOG_DEBUG  , __func__, fmt, ##__VA_ARGS__)

/** Tagged LOG_* definitions */
#define LOG_TAG_ERROR(tag, fmt, ...)   LOG_TAG(LOG_ERROR  , tag, fmt, ##__VA_ARGS__)
#define LOG_TAG_WARNING(tag, fmt, ...) LOG_TAG(LOG_WARNING, tag, fmt, ##__VA_ARGS__)
#define LOG_TAG_INFO(tag, fmt, ...)    LOG_TAG(LOG_INFO   , tag, fmt, ##__VA_ARGS__)
#define LOG_TAG_DEBUG(tag, fmt, ...)   LOG_TAG(LOG_DEBUG  , tag, fmt, ##__VA_ARGS__)

/** definitions for source code compatibility with ESP-IDF */
#define ESP_EARLY_LOGE(tag, fmt, ...) LOG_TAG(LOG_ERROR  , tag, fmt "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGW(tag, fmt, ...) LOG_TAG(LOG_WARNING, tag, fmt "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGI(tag, fmt, ...) LOG_TAG(LOG_INFO   , tag, fmt "\n", ##__VA_ARGS__)
/*
#define ESP_EARLY_LOGI(tag, fmt, ...) ets_printf("I (%u) %s: " fmt "\n", \
                                                 system_get_time_ms(), tag, ##__VA_ARGS__)
*/
#define ESP_LOGE(tag, fmt, ...) LOG_TAG(LOG_ERROR  , tag, fmt "\n", ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) LOG_TAG(LOG_WARNING, tag, fmt "\n", ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) LOG_TAG(LOG_INFO   , tag, fmt "\n", ##__VA_ARGS__)

#if ENABLE_DEBUG

#define ESP_EARLY_LOGD(tag, fmt, ...) LOG_TAG(LOG_DEBUG, tag, fmt "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGV(tag, fmt, ...) LOG_TAG(LOG_ALL  , tag, fmt "\n", ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) LOG_TAG(LOG_DEBUG, tag, fmt "\n", ##__VA_ARGS__)
#define ESP_LOGV(tag, fmt, ...) LOG_TAG(LOG_ALL  , tag, fmt "\n", ##__VA_ARGS__)

#else /* ENABLE_DEBUG */

#define ESP_EARLY_LOGD( tag, format, ... ) (void)tag
#define ESP_EARLY_LOGV( tag, format, ... ) (void)tag
#define ESP_LOGD( tag, format, ... ) (void)tag
#define ESP_LOGV( tag, format, ... ) (void)tag

#endif /* ENABLE_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */

#endif /* ESP_COMMON_LOG_H */
