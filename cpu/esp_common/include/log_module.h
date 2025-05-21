/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Log module to realize consistent log messages for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "esp_common_log.h"

#ifdef MODULE_LOG_PRINTFNOFORMAT

static inline void log_write(unsigned level, const char *format, ...) {
    (void)level;
    puts(format);
}

#else /* MODULE_LOG_PRINTFNOFORMAT */

#define log_write(level, ...) \
                do { \
                    if (level == LOG_ERROR) { \
                        LOG_TAG(LOG_ERROR, E, __func__, ##__VA_ARGS__); \
                    } \
                    else if (level == LOG_WARNING) { \
                        LOG_TAG(LOG_WARNING, W, __func__, ##__VA_ARGS__); \
                    } \
                    else if (level == LOG_INFO) { \
                        LOG_TAG(LOG_INFO, D, __func__, ##__VA_ARGS__); \
                    } \
                    else if (level == LOG_DEBUG) { \
                        LOG_TAG(LOG_DEBUG, E, __func__, ##__VA_ARGS__); \
                    } \
                } while (0U)

#endif /* MODULE_LOG_PRINTFNOFORMAT */

#ifdef __cplusplus
}
#endif
/**@}*/
