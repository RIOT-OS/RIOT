/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdebug
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_auto_init_utils Utilities to influence the order of auto-initialized modules
 * @ingroup     sys
 *
 * @{
 * @file
 * @brief       Auto-initialization utilities
 *
 * A module priority value in RIOTs auto-initialization sequence is, as of now,
 * a hard coded 4-digit number, but is likely to be changed and automatically
 * generated in the future. Modules are initialized in ascending
 * (alpha)numerical order of their priority, which means that modules with
 * lower priorities are initialized first, as long as their priorities comply
 * with the current rules.
 *
 * @experimental
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include <stdint.h>
#include "xfa.h"
#include "macros/xtstr.h"
#include "modules.h"
#if IS_USED(MODULE_PREPROCESSOR_SUCCESSOR)
#include "preprocessor_successor.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_AUTO_INIT_ENABLE_DEBUG
/**
 * @brief   Print a debug message before a module is initialized
 */
#define CONFIG_AUTO_INIT_ENABLE_DEBUG 0
#endif

/**
 * @brief   Auto-init function type
 */
typedef void (*auto_init_fn_t)(void);

/**
 * @brief   Data type to store the priority of a module
 */
typedef uint16_t auto_init_prio_t;

/**
 * @brief   Type to represent a module to be auto-initialized
 */
typedef struct {
    auto_init_fn_t init;    /**< Function to initialize the module */
#if IS_ACTIVE(CONFIG_AUTO_INIT_ENABLE_DEBUG) || defined(DOXYGEN)
    auto_init_prio_t prio;  /**< Module priority */
    const char *name;       /**< Module auto-init function name */
#endif
} auto_init_module_t;

#if IS_ACTIVE(CONFIG_AUTO_INIT_ENABLE_DEBUG) || defined(DOXYGEN)
/**
 * @brief   Add a module to the auto-initialization array
 *
 * @param   function    Function to be called on initialization @ref auto_init_fn_t
 * @param   priority    Priority level @ref auto_init_prio_t
 */
#define AUTO_INIT(function, priority)                                                   \
    XFA_CONST(auto_init_module_t, auto_init_xfa, priority)                              \
    auto_init_xfa_ ## function                                                          \
        = { .init = (auto_init_fn_t)function,                                           \
            .prio = priority,                                                           \
            .name = XTSTR(function) }
#else
#define AUTO_INIT(function, priority)                                                   \
    XFA_CONST(auto_init_module_t, auto_init_xfa, priority)                              \
    auto_init_xfa_ ## function                                                          \
        = { .init = (auto_init_fn_t)function }
#endif

/**
 * @brief   Construct a priority value equal to @p priority + 1,
 *          to be used with @ref AUTO_INIT
 */
#define AUTO_INIT_PRIORITY_AFTER(priority)  RIOT_PP_SUCCESSOR(priority)

#ifdef __cplusplus
}
#endif

/** @} */
