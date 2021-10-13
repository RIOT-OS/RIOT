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
#include "lua_run.h"

#include "blob/main.lua.h"

#define LUA_MEM_SIZE (11000)
static char lua_mem[LUA_MEM_SIZE] __attribute__ ((aligned(__BIGGEST_ALIGNMENT__)));

int lua_run_script(const uint8_t *buffer, size_t buffer_len)
{
    lua_State *L = lua_riot_newstate(lua_mem, sizeof(lua_mem), NULL);

    if (L == NULL) {
        puts("cannot create state: not enough memory");
        return ENOMEM;
    }

    lua_riot_openlibs(L, LUAR_LOAD_BASE);
    luaL_loadbuffer(L, (const char *)buffer, buffer_len, "lua input script");

    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
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
    puts("Lua interpreter exited");

    return 0;
}
