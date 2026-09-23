/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup  pkg_lua
 * @{
 * @file
 *
 * @brief   Lightweight C interface to the package loader.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error code for when a modules is not found.
 *
 * The numeric value is chosen so that there is no collision with Lua's
 * own error codes.
 */
#define LUAR_MODULE_NOTFOUND 50

/**
 * Load a module as a chunk.
 *
 * This function is a lightweight "require". It does not require the "package"
 * module to be loaded and does not register the module.
 * Only the builtin tables are searched.
 *
 * Upon successful execution, the compiled chunk will be at the top of the lua
 * stack.
 *
 * @param   L       Initialized Lua interpreter state.
 * @param   name    Name of the module.
 *
 * @return      Same as lua_load. If the module is a C-module, then this will
 *              always succeed and return LUA_OK.
 */
int lua_riot_getloader(lua_State *L, const char *name);

#ifdef __cplusplus
extern "C"
}
#endif

/** @} */
