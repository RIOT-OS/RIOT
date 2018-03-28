/*
 * Copyright (C) 1994-2017 Lua.org, PUC-Rio.
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
 * @brief   Replacement for the lua "package" module.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 * @author  Roberto Ierusalimschy
 *
 * This file replaces the loadlib.c that comes with lua. It removes support
 * for files (both lua files and c shared objects) and dynamic loading since
 * none of these are present in RIOT.
 *
 * Instead, modules are searched in statically defined tables. In the case
 * of C modules, the table contains pointers to C functions that act as module
 * loaders. For pure Lua modules, the source code must be given as a string
 * embedded in the application binary.
 *
 */

#define loadlib_c
#define LUA_LIB

#include "lprefix.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "binsearch.h"

#include "lua_builtin.h"
#include "lua_loadlib.h"

/* ======================== 'searchers' functions =========================== */

static int _ll_searcher_builtin_lua(lua_State *L, const char *name)
{
    const struct lua_riot_builtin_lua *lmodule =
        BINSEARCH_STR_P(lua_riot_builtin_lua_table,
                        lua_riot_builtin_lua_table_len,
                        name, name, LUAR_MAX_MODULE_NAME);

    if (lmodule != NULL) {
        int load_result = luaL_loadbuffer(L, (const char *)lmodule->code,
                                          lmodule->code_size,
                                          lmodule->name);
        if (load_result == LUA_OK) {
            lua_pushstring(L, name);    /* will be 2nd argument to module */
        }

        return load_result;
    }
    else {
        return LUAR_MODULE_NOTFOUND;
    }
}

/**
 * Search in the list of pure lua modules.
 *
 * If the module is found, the source code is compiled and the compiled chunk
 * is placed on the lua stack, followed by the module name (as a string).
 */
static int searcher_builtin_lua(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    int load_result = _ll_searcher_builtin_lua(L, name);

    switch (load_result) {
        case LUA_OK:
            return 2; /* there are two elements in the stack */
        case LUAR_MODULE_NOTFOUND:
            return luaL_error(L, "Module '%s' not found in Lua-builtins",
                              lua_tostring(L, 1));
        default:
            return luaL_error(L, "error loading module '%s' from Lua-builtins: \n%s",
                              lua_tostring(L, 1), lua_tostring(L, 2));
    }
}

static int _ll_searcher_builtin_c(lua_State *L, const char *name)
{
    const struct lua_riot_builtin_c *cmodule =
        BINSEARCH_STR_P(lua_riot_builtin_c_table,
                        lua_riot_builtin_c_table_len,
                        name, name, LUAR_MAX_MODULE_NAME);

    if (cmodule != NULL) {
        lua_pushcfunction(L, cmodule->luaopen);
        lua_pushstring(L, name);    /* will be 2nd argument to module */
        return LUA_OK;
    }
    else {
        return LUAR_MODULE_NOTFOUND;
    }
}

/**
 * Search in the list of C lua modules.
 *
 * If the module is found, the loader function is loaded with lua_pushcfunction
 * and is returned.
 */
static int searcher_builtin_c(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);
    int load_result = _ll_searcher_builtin_c(L, name);

    if (load_result == LUA_OK) {
        return 2;
    }
    else {
        return luaL_error(L, "Module '%s' not found in C-builtins",
                          lua_tostring(L, 1));
    }
}

int lua_riot_getloader(lua_State *L, const char *name)
{
    int load_result;

    load_result = _ll_searcher_builtin_lua(L, name);

    if (load_result == LUAR_MODULE_NOTFOUND) {
        load_result = _ll_searcher_builtin_c(L, name);
    }

    return load_result;
}

/* ======================== 'require' function ============================= */

static int searcher_preload(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);

    lua_getfield(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
    if (lua_getfield(L, -1, name) == LUA_TNIL) { /* not found? */
        lua_pushfstring(L, "\n\tno field package.preload['%s']", name);
    }
    return 1;
}

static void findloader(lua_State *L, const char *name)
{
    int i;
    luaL_Buffer msg; /* to build error message */

    luaL_buffinit(L, &msg);
    /* push 'package.searchers' to index 3 in the stack */
    if (lua_getfield(L, lua_upvalueindex(1), "searchers") != LUA_TTABLE) {
        luaL_error(L, "'package.searchers' must be a table");
    }
    /*  iterate over available searchers to find a loader */
    for (i = 1;; i++) {
        if (lua_rawgeti(L, 3, i) == LUA_TNIL) { /* no more searchers? */
            lua_pop(L, 1);                      /* remove nil */
            luaL_pushresult(&msg);              /* create error message */
            luaL_error(L, "module '%s' not found:%s", name, lua_tostring(L, -1));
        }
        lua_pushstring(L, name);
        lua_call(L, 1, 2);              /* call it */
        if (lua_isfunction(L, -2)) {    /* did it find a loader? */
            return;                     /* module loader found */
        }
        else if (lua_isstring(L, -2)) { /* searcher returned error message? */
            lua_pop(L, 1);              /* remove extra return */
            luaL_addvalue(&msg);        /* concatenate error message */
        }
        else {
            lua_pop(L, 2); /* remove both returns */
        }
    }
}

static int ll_require(lua_State *L)
{
    const char *name = luaL_checkstring(L, 1);

    lua_settop(L, 1);           /* LOADED table will be at index 2 */
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    lua_getfield(L, 2, name);   /* LOADED[name] */
    if (lua_toboolean(L, -1)) { /* is it there? */
        return 1;               /* package is already loaded */
    }
    /* else must load package */
    lua_pop(L, 1);                              /* remove 'getfield' result */
    findloader(L, name);
    lua_pushstring(L, name);                    /* pass name as argument to module loader */
    lua_insert(L, -2);                          /* name is 1st argument (before search data) */
    lua_call(L, 2, 1);                          /* run loader to load module */
    if (!lua_isnil(L, -1)) {                    /* non-nil return? */
        lua_setfield(L, 2, name);               /* LOADED[name] = returned value */
    }
    if (lua_getfield(L, 2, name) == LUA_TNIL) { /* module set no value? */
        lua_pushboolean(L, 1);                  /* use true as result */
        lua_pushvalue(L, -1);                   /* extra copy to be returned */
        lua_setfield(L, 2, name);               /* LOADED[name] = true */
    }
    return 1;
}

/* ====================== 'package' module loader =========================== */

static const luaL_Reg pk_funcs[] = {
    /* placeholders */
    { "preload", NULL },
    { "searchers", NULL },
    { "loaded", NULL },
    { NULL, NULL }
};


static const luaL_Reg ll_funcs[] = {
    { "require", ll_require },
    { NULL, NULL }
};

LUAMOD_API int luaopen_package(lua_State *L)
{
    static const lua_CFunction searchers[] =
    { searcher_preload, searcher_builtin_lua, searcher_builtin_c, NULL };
    int i;

    luaL_newlib(L, pk_funcs); /* create 'package' table */

    /* create 'searchers' table */
    lua_createtable(L, sizeof(searchers) / sizeof(searchers[0]) - 1, 0);
    /* fill it with predefined searchers */
    for (i = 0; searchers[i] != NULL; i++) {
        lua_pushvalue(L, -2); /* set 'package' as upvalue for all searchers */
        lua_pushcclosure(L, searchers[i], 1);
        lua_rawseti(L, -2, i + 1);
    }

    lua_setfield(L, -2, "searchers"); /* put it in field 'searchers' */

    /* set field 'loaded' */
    luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    lua_setfield(L, -2, "loaded");

    /* set field 'preload' */
    luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
    lua_setfield(L, -2, "preload");


    lua_pushglobaltable(L);
    lua_pushvalue(L, -2);           /* set 'package' as upvalue for next lib */
    luaL_setfuncs(L, ll_funcs, 1);  /* open lib into global table */
    lua_pop(L, 1);                  /* pop global table */
    return 1;                       /* return 'package' table */
}

/** @} */
