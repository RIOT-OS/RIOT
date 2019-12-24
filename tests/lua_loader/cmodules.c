/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Define a couple of dummy lua extension modules
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * Normally one would not define more than one module in a single file, but
 * these modules are exactly the same: a single table with an integer value.
 *
 * @}
 */

#define LUA_LIB

#include "lprefix.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static const luaL_Reg funcs[] = {
    /* placeholder */
    { "X", NULL },
    { NULL, NULL }
};

static int _luaopen_X(lua_State *L, const char *xstring)
{
    luaL_newlib(L, funcs);

    lua_pushstring(L, xstring);
    lua_setfield(L, -2, "X");

    return 1;
}

int _luaopen_hello(lua_State *L)
{
    return _luaopen_X(L, "E se deixa no céu,");
}

int _luaopen_world(lua_State *L)
{
    return _luaopen_X(L, "como esquecida");
}
