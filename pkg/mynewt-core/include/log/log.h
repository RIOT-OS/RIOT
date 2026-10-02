/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
