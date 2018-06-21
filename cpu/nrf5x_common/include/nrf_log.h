/*
 * Copyright (C) 2018 Aalto university
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       nRF5x shared functions for logging
 *
 * @author      Pekka Nikander <pekka.nikander@iki.fi>
 */

#ifndef NRF_LOG_H
#define NRF_LOG_H

#include <stdio.h>

#if NRF_MODULE_ENABLED(NRF_LOG)
#include "nrf_strerror.h"
#define NRF_LOG_ERROR_STRING_GET(code) nrf_strerror_get(code)
#else
#define NRF_LOG_ERROR_STRING_GET(code) ""
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NRF_LOG_DEFAULT_LEVEL
#define NRF_LOG_DEFAULT_LEVEL 4
#endif

#ifndef NRF_LOG_LEVEL
#define NRF_LOG_LEVEL NRF_LOG_DEFAULT_LEVEL
#endif

#define NRF_LOG_LEVEL_FATAL   0
#define NRF_LOG_LEVEL_ERROR   1
#define NRF_LOG_LEVEL_WARNING 2
#define NRF_LOG_LEVEL_INFO    3
#define NRF_LOG_LEVEL_DEBUG   4

#define NRF_LOG_ERROR(...)                     NRF_LOG_INTERNAL(NRF_LOG_LEVEL_ERROR,   __VA_ARGS__)
#define NRF_LOG_WARNING(...)                   NRF_LOG_INTERNAL(NRF_LOG_LEVEL_WARNING, __VA_ARGS__)
#define NRF_LOG_INFO(...)                      NRF_LOG_INTERNAL(NRF_LOG_LEVEL_INFO,    __VA_ARGS__)
#define NRF_LOG_DEBUG(...)                     NRF_LOG_INTERNAL(NRF_LOG_LEVEL_DEBUG,   __VA_ARGS__)

#define NRF_LOG_INST_ERROR(p_inst,...)         NRF_LOG_INTERNAL(NRF_LOG_LEVEL_ERROR,   __VA_ARGS__)
#define NRF_LOG_INST_WARNING(p_inst,...)       NRF_LOG_INTERNAL(NRF_LOG_LEVEL_WARNING, __VA_ARGS__)
#define NRF_LOG_INST_INFO(p_inst,...)          NRF_LOG_INTERNAL(NRF_LOG_LEVEL_INFO,    __VA_ARGS__)
#define NRF_LOG_INST_DEBUG(p_inst,...)         NRF_LOG_INTERNAL(NRF_LOG_LEVEL_DEBUG,   __VA_ARGS__)

#define NRF_LOG_RAW_INFO(...)                  NRF_LOG_INTERNAL_RAW(__VA_ARGS__)

/* Unimplemented as of now */

#define NRF_LOG_HEXDUMP_ERROR(p_data, len)
#define NRF_LOG_HEXDUMP_WARNING(p_data, len)
#define NRF_LOG_HEXDUMP_INFO(p_data, len)
#define NRF_LOG_HEXDUMP_DEBUG(p_data, len)

#define NRF_LOG_HEXDUMP_INST_ERROR(p_inst, p_data, len)
#define NRF_LOG_HEXDUMP_INST_WARNING(p_inst, p_data, len)
#define NRF_LOG_HEXDUMP_INST_INFO(p_inst, p_data, len)
#define NRF_LOG_HEXDUMP_INST_DEBUG(p_inst, p_data, len)

#define NRF_LOG_RAW_HEXDUMP_INFO(p_data, len)

#define NRF_LOG_MODULE_REGISTER()

#define NRF_LOG_INTERNAL(level, ...) \
    do { if (level <= NRF_LOG_LEVEL) { printf(__VA_ARGS__); putchar('\n'); } } while(0)

#ifdef __cplusplus
}
#endif

#endif /* NRF_LOG_H */
/** @} */
