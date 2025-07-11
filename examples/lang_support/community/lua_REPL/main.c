/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Lua shell in RIOT
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "architecture.h"
#include "lua_run.h"
#include "lua_builtin.h"
#include "blob/repl.lua.h"

/* The basic interpreter+repl needs about 13k ram AT Minimum but we need more
 * memory in order to do interesting stuff.
 */
#define MAIN_LUA_MEM_SIZE (40000)

static char lua_memory[MAIN_LUA_MEM_SIZE] __attribute__ ((aligned(__BIGGEST_ALIGNMENT__)));

#define BARE_MINIMUM_MODS (LUAR_LOAD_BASE | LUAR_LOAD_IO | LUAR_LOAD_CORO | LUAR_LOAD_PACKAGE)

const struct lua_riot_builtin_lua _lua_riot_builtin_lua_table[] = {
    { "repl", repl_lua, sizeof(repl_lua) }
};

const struct lua_riot_builtin_lua *const lua_riot_builtin_lua_table = _lua_riot_builtin_lua_table;

const size_t lua_riot_builtin_lua_table_len = 1;

int main(void)
{
    printf("Using memory range for Lua heap: %p - %p, %" PRIuSIZE " bytes\n",
           (void *)lua_memory, (void *)(lua_memory + MAIN_LUA_MEM_SIZE), sizeof(void *));

    while (1) {
        int status, value;
        puts("This is Lua: starting interactive session\n");

        status = lua_riot_do_module("repl", lua_memory, MAIN_LUA_MEM_SIZE,
                                    BARE_MINIMUM_MODS, &value);

        printf("Exited. status: %s, return code %d\n", lua_riot_strerror(status),
               value);
    }

    return 0;
}
