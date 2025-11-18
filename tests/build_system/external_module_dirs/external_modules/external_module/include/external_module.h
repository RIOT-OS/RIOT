/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup
 * @ingroup
 * @brief
 * @{
 *
 * @file
 * @brief
 *
 * @author  Gaëtan Harter <gaetan.harter@fu-berlin.de>
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   true: this module hase been initialized
 *          false: this module has not been initialized
 */
extern bool external_module_initialized;

/**
 * @brief   A simple string message
 */
extern char *external_module_message;

/**
 * @brief   Auto-init function of this module to be called on system start up
 */
void auto_init_external_module(void);

#ifdef __cplusplus
}
#endif

/** @} */
