/*
 * SPDX-FileCopyrightText: 2019 Freie Universität Berlin.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @brief       Test custom lua loader ("require" module)
 *
 * @author      Juan Carrano <j.carrano@fu-berlin.de>
 *
 * This application defines some pure-lua and some C-extension modules.
 * First it tests running a module as a script (lua_riot_do_module).
 *
 * Then it goes into a loop reading single lines of input from stdin and
 * executing them as lua source. The test script can use that to test module
 * loading.
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "lua_run.h"
#include "lua_builtin.h"

#include "test_utils/expect.h"

static const uint8_t pure_module_1[] = "\n\
return {a='Quando uma lua'}\n\
";

static const uint8_t pure_module_2[] = "\n\
return {a='chega de repente'}\n\
";

static const uint8_t test_script[] = "\n\
print'I am a module, hi!'\n\
";

/* The -1 is because the final '\0' is not part of the source code that should
 * be read by lua.
 */
const struct lua_riot_builtin_lua _lua_riot_builtin_lua_table[] = {
    { "m1", pure_module_1, sizeof(pure_module_1) - 1 },
    { "m2", pure_module_2, sizeof(pure_module_2) - 1 },
    { "test", test_script, sizeof(test_script) - 1 }
};

extern int _luaopen_hello(lua_State *L);
extern int _luaopen_world(lua_State *L);

const struct lua_riot_builtin_c _lua_riot_builtin_c_table[] = {
    { "c1", _luaopen_hello },
    { "c2", _luaopen_world }
};

const struct lua_riot_builtin_lua *const lua_riot_builtin_lua_table = _lua_riot_builtin_lua_table;
const struct lua_riot_builtin_c *const lua_riot_builtin_c_table = _lua_riot_builtin_c_table;

const size_t lua_riot_builtin_lua_table_len =
    ARRAY_SIZE(_lua_riot_builtin_lua_table);
const size_t lua_riot_builtin_c_table_len =
    ARRAY_SIZE(_lua_riot_builtin_c_table);

#if (__SIZEOF_POINTER__ == 8)
#define LUA_MEM_SIZE (22000)
#else
#define LUA_MEM_SIZE (11000)
#endif
static char lua_mem[LUA_MEM_SIZE] __attribute__ ((aligned(__BIGGEST_ALIGNMENT__)));

#define LINEBUF_SZ (32)
static char linebuf[LINEBUF_SZ];

int main(void)
{
    int status;

    status = lua_riot_do_module("test", lua_mem, LUA_MEM_SIZE,
                                LUAR_LOAD_BASE, NULL);

    expect(status == LUAR_EXIT);

    while (fgets(linebuf, LINEBUF_SZ, stdin) != NULL) {
        int status;
        size_t linelen = strlen(linebuf);

        if (!linelen) {
            continue;
        }
        else if (linebuf[linelen - 1] != '\n') {
            puts("[ERROR] could not read a complete line");
            break;
        }

        status = lua_riot_do_buffer((unsigned char *)linebuf, linelen,
                                    lua_mem, LUA_MEM_SIZE,
                                    LUAR_LOAD_BASE | LUAR_LOAD_PACKAGE, NULL);
        expect(status == LUAR_EXIT);
    }

    return 0;
}
