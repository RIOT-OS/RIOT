/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

#ifndef REGISTRY_NAMESPACE_SYS_H
#define REGISTRY_NAMESPACE_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "registry.h"

extern registry_namespace_t registry_sys;

/**
 * @brief This ENUM defines the IDs of configuration schemas in the "sys" namespace.
 * The IDs are needed by the int_path module to identify schemas using IDs instead
 * of pointers.
 */
typedef enum {
    REGISTRY_SYS_BOARD_LED  = 0,
    REGISTRY_SYS_RGB_LED    = 1,
} registry_sys_indices_t;

#ifdef __cplusplus
}
#endif

#endif /* REGISTRY_NAMESPACE_SYS_H */
/** @} */
