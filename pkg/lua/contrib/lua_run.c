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
 * @brief   Convenience functions for running Lua code.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 */

#define LUA_LIB

#include "lprefix.h"

#include <stdio.h>
#include <setjmp.h>

#include "kernel_defines.h"
#include "tlsf.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lua_run.h"
#include "lua_loadlib.h"

const char *lua_riot_str_errors[] = {
    "No errors",
    "Error setting up the interpreter",
    "Error while loading a builtin library",
    "Cannot find the specified module",
    "Compilation / syntax error",
    "Unprotected error (uncaught exception)",
    "Out of memory",
    "Internal interpreter error",
    "Unknown error"
};

/* The lua docs state the behavior in these cases:
 *
 *  1.              ptr=?, size=0    -> free(ptr)
        therefore   ptr=NULL, size=0 -> NOP
 *  2.               ptr=? , size!=0  -> realloc(ptr, size)
 *
 * The  TLSF code for realloc says:
 *  / * Zero-size requests are treated as free. * /
 *  if (ptr && size == 0)
 *  {
 *      tlsf_free(tlsf, ptr);
 *  }
 *  / * Requests with NULL pointers are treated as malloc. * /
 *  else if (!ptr)
 *  {
 *      p = tlsf_malloc(tlsf, size);
 *  }
 *
 * Therefore it is safe to use tlsf_realloc here.
 */
static void *lua_tlsf_alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
    tlsf_t tlsf = ud;

    (void)osize;

    return tlsf_realloc(tlsf, ptr, nsize);
}

LUALIB_API lua_State *lua_riot_newstate(void *memory, size_t mem_size,
                                        lua_CFunction panicf)
{
    lua_State *L;

    #ifdef LUA_DEBUG
        Memcontrol *mc = memory;
    #endif

    /* If we are using the lua debug module, let's reserve a space for the
     * memcontrol block directly. We don't use the allocator because we lose
     * the pointer, so we won't be able to free it and we will get a false
     * positive if we try to check for memory leaks.
     */
    #ifdef LUA_DEBUG
        memory = (Memcontrol *)memory + 1;
        mem_size -= (uint8_t *)memory - (uint8_t *)mc;
    #endif

    tlsf_t tlsf = tlsf_create_with_pool(memory, mem_size);

    #ifdef LUA_DEBUG
        luaB_init_memcontrol(mc, lua_tlsf_alloc, tlsf);
        L = luaB_newstate(mc);
    #else
        L = lua_newstate(lua_tlsf_alloc, tlsf);
    #endif

    if (L != NULL) {
        lua_atpanic(L, panicf);
    }

    return L;
}

static const luaL_Reg loadedlibs[LUAR_LOAD_O_ALL] = {
    { "_G", luaopen_base },
    { LUA_LOADLIBNAME, luaopen_package },
    { LUA_COLIBNAME, luaopen_coroutine },
    { LUA_TABLIBNAME, luaopen_table },
    { LUA_IOLIBNAME, luaopen_io },
    { LUA_OSLIBNAME, luaopen_os },
    { LUA_STRLIBNAME, luaopen_string },
    { LUA_MATHLIBNAME, luaopen_math },
    { LUA_UTF8LIBNAME, luaopen_utf8 },
    { LUA_DBLIBNAME, luaopen_debug },
};

LUALIB_API int lua_riot_openlibs(lua_State *L, uint16_t modmask)
{
    int lib_index;

    #ifdef LUA_DEBUG
        luaL_requiref(L, LUA_TESTLIBNAME, luaB_opentests, 1);
        lua_pop(L, 1);
    #endif

    for (lib_index = 0; lib_index < LUAR_LOAD_O_ALL;
         lib_index++, modmask >>= 1) {
        const luaL_Reg *lib = loadedlibs + lib_index;

        if (!(modmask & 1)) {
            continue;
        }
        /* TODO: how can the loading fail? */
        luaL_requiref(L, lib->name, lib->func, 1);
        lua_pop(L, 1);  /* remove lib from stack (it is already global) */
    }

    return lib_index;
}

/**
 * Jump back to a save point (defined with setjmp).
 *
 * @note    This function never returns!
 */
NORETURN static int _jump_back(lua_State *L)
{
    jmp_buf *jump_buffer = *(jmp_buf **)lua_getextraspace(L);

    /* FIXME: I don't think it's OK to print a message */
    lua_writestringerror("PANIC: unprotected error in call to Lua API (%s)\n",
                         lua_tostring(L, -1));

    /* TODO: try to return some info about the error object. */

    longjmp(*jump_buffer, 1);
}

static int lua_riot_do_module_or_buf(const uint8_t *buf, size_t buflen,
                                     const char *modname, void *memory, size_t mem_size,
                                     uint16_t modmask, int *retval)
{
    jmp_buf jump_buffer;
    lua_State *volatile L = NULL;
    volatile int tmp_retval = 0; /* we need to make it volatile because of the
                                    setjmp/longjmp */
    volatile int status = LUAR_EXIT;
    int compilation_result;

    if (setjmp(jump_buffer)) { /* We'll teleport back here if something goes wrong*/
        status = LUAR_INTERNAL_ERR;
        goto lua_riot_do_error;
    }

    L = lua_riot_newstate(memory, mem_size, _jump_back);
    if (L == NULL) {
        status = LUAR_STARTUP_ERR;
        goto lua_riot_do_error;
    }

    /* lua_getextraspace() gives us a pointer to an are large enough to hold a
     * pointer.
     *
     * We store a pointer to the jump buffer in that area.
     *
     * lua_getextraspace() is therefore a pointer to a pointer to jump_buffer.
     */
    *(jmp_buf **)lua_getextraspace(L) = &jump_buffer;

    tmp_retval = lua_riot_openlibs(L, modmask);
    if (tmp_retval != LUAR_LOAD_O_ALL) {
        status = LUAR_LOAD_ERR;
        goto lua_riot_do_error;
    }

    if (buf == NULL) {
        compilation_result = lua_riot_getloader(L, modname);
    }
    else {
        compilation_result = luaL_loadbufferx(L, (const char *)buf,
                                              buflen, modname, "t");
    }

    switch (compilation_result) {
        case LUAR_MODULE_NOTFOUND:
            status = LUAR_NOMODULE;
            goto lua_riot_do_error;
        case LUA_ERRSYNTAX:
            status = LUAR_COMPILE_ERR;
            goto lua_riot_do_error;
        case LUA_ERRMEM:    /* fallthrough */
        case LUA_ERRGCMM:   /* fallthrough */
        default:
            status = LUAR_MEMORY_ERR;
            goto lua_riot_do_error;
        case LUA_OK:
            break;
    }

    if (buf != NULL) {
        lua_pushstring(L, modname);
    }

    switch (lua_pcall(L, 1, 1, 0)) {
        case LUA_ERRRUN:    /* fallthrough */
        case LUA_ERRGCMM:   /* fallthrough */
        default:
            status = LUAR_RUNTIME_ERR;
            puts(lua_tostring(L, -1));
            goto lua_riot_do_error;
        case LUA_ERRMEM:
            status = LUAR_MEMORY_ERR;
            goto lua_riot_do_error;
        case LUA_OK:
            break;
    }

    tmp_retval = lua_tonumber(L, 1);

lua_riot_do_error:

    if (L != NULL) {
        lua_riot_close(L);
    }

    if (retval != NULL) {
        *retval = tmp_retval;
    }

    return status;
}


LUALIB_API int lua_riot_do_module(const char *modname, void *memory, size_t mem_size,
                                  uint16_t modmask, int *retval)
{
    return lua_riot_do_module_or_buf(NULL, 0, modname, memory, mem_size, modmask,
                                     retval);
}

LUALIB_API int lua_riot_do_buffer(const uint8_t *buf, size_t buflen, void *memory,
                                  size_t mem_size, uint16_t modmask, int *retval)
{
    return lua_riot_do_module_or_buf(buf, buflen, "=BUFFER", memory, mem_size,
                                     modmask, retval);
}

#define MAX_ERR_STRING (ARRAY_SIZE(lua_riot_str_errors) - 1)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

LUALIB_API const char *lua_riot_strerror(int errn)
{
    return lua_riot_str_errors[MIN((unsigned int)errn, MAX_ERR_STRING)];
}

/** @} */
