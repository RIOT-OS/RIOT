/*
 * SPDX-FileCopyrightText: 2023-2025 Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2023-2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @defgroup    sys_registry_namespace_sys RIOT Registry Sys Namespace
 * @ingroup     sys
 * @brief       RIOT Registry Namespace Sys module
 * @{
 *
 * This module provides common sys configuration schemas for the RIOT Registry sys module
 *
 * @file
 *
 * @author      Lasse Rosenow <lasse.rosenow@haw-hamburg.de>
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"

/**
 * @brief Sys namespace for system modules.
 */
extern registry_namespace_t registry_sys;

/**
 * @brief This ENUM defines the IDs of configuration schemas in the "sys" namespace.
 * The IDs are needed by the int_path module to identify schemas using IDs instead
 * of pointers.
 */
typedef enum {
    REGISTRY_SYS_BOARD_LED = 0,
    REGISTRY_SYS_RGB_LED = 1,
} registry_sys_indices_t;

#ifdef __cplusplus
}
#endif
/** @} */
