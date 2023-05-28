/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
#ifndef EXTERNAL_MODULE_H
#define EXTERNAL_MODULE_H

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
#endif /* EXTERNAL_MODULE_H */
