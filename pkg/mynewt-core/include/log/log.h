/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       System logging header for mynewt-core
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef LOG_LOG_H
#define LOG_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

/**
 * @name Logging convenience defines wrappers
 * @{
 */
#define LOG_WARN(...)       LOG(LOG_WARNING, __VA_ARGS__)
#define LOG_CRITICAL(...)   LOG(LOG_ERROR, __VA_ARGS__)
#define log_register(__X, __Y, __Z, __A, __B) {}
/** @} */

/**
 * @brief Empty log structure
 */
struct log {
};

#ifdef __cplusplus
}
#endif

#endif /* LOG_LOG_H */
