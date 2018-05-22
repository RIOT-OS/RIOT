/*
 * Copyright (C) 2018 FU Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Basic lua example application
 *
 * @author      Daniel Petry <daniel.petry@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <errno.h>
#include "lauxlib.h"
#include "lualib.h"

#include "main.lua.h"

int lua_run_script(const char *buffer, size_t buffer_len)
{

    lua_State *L = luaL_newstate();

    if (L == NULL) {
        puts("cannot create state: not enough memory");
        return ENOMEM;
    }

    luaL_openlibs(L);
    luaL_loadbuffer(L, buffer, buffer_len, "lua input script");

    if (lua_pcall(L, 0, 0, 0) != LUA_OK){
        puts("Lua script running failed");
        return EINTR;
    }

    lua_close(L);
    return 0;
}

int main(void)
{
    puts("Lua RIOT build");
    lua_run_script(main_lua, main_lua_len);
    return 0;
}
