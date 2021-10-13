/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup  pkg_lua
 * @{
 * @file
 *
 * @brief   Definitions for including built-in modules.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 * The modules must be placed in the tables lua_riot_builtin_lua_table (for lua
 * source code) and lua_riot_builtin_c_table (for C extensions) and the lengths
 * of these tables must be in lua_riot_builtin_lua_table_len and
 * lua_riot_builtin_c_table_len.
 *
 * These symbols are defined as weak, so there if they are not defined elsewhere
 * they will default to zero (or NULL), that is, empty tables.
 */

#ifndef LUA_BUILTIN_H
#define LUA_BUILTIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Avoid compilation errors where there are no external modules defined */
/**
 * Attribute to make symbols weak.
 *
 * @todo This should be made part of RIOT.
 */
#define WEAK __attribute__((weak))

/**
 * Only the first `LUAR_MAX_MODULE_NAME` characters of a module name
 * will be considered when performing a lookup.
 */
#define LUAR_MAX_MODULE_NAME 64

/**
 * Entry describing a pure lua module whose source is built into the
 * application binary.
 */
struct lua_riot_builtin_lua {
    const char *name;   /*!< Name of the module */
    const uint8_t *code;   /*!< Lua source code buffer*/
    size_t code_size;   /*!< Size of the source code buffer. */
};

/**
 * Entry describing a c lua module built into the
 * application binary.
 */
struct lua_riot_builtin_c {
    const char *name;               /*!< Name of the module */
    int (*luaopen)(lua_State *);    /*!< Loader function. It must place the module
                                     *  table at the top of the lua stack.
                                     *  @todo Add better docs.
                                     */
};

/** Table containing all built in pure lua modules */
extern WEAK const struct lua_riot_builtin_lua *const lua_riot_builtin_lua_table;
/** Number of elements of lua_riot_builtin_lua_table */
extern WEAK const size_t lua_riot_builtin_lua_table_len;

/** Table containing all built in c lua modules */
extern WEAK const struct lua_riot_builtin_c *const lua_riot_builtin_c_table;
/** Number of elements of lua_riot_builtin_c_table */
extern WEAK const size_t lua_riot_builtin_c_table_len;

#ifdef __cplusplus
extern "C" }
#endif

#endif /* LUA_BUILTIN_H */

/** @} */
