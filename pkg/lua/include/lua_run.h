/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup  pkg_lua
 * @file
 * @{
 *
 * @brief   Convenience functions for running Lua code.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 * This functions make it easy to create and use new Lua context:
 * It provides:
 *
 * - Easy to use routines for executing modules as scripts.
 * - Control over which modules get loaded.
 * - Support for using a local heap allocator.
 * - Out of memory handling via setjmp/longjmp.
 *
 * This library is not strictly required, as all of the functionality could be
 * implemented in terms of the public lua api, but it covers most of the use
 * cases, and thus avoids code repetition in applications.
 *
 */

#ifndef LUA_RUN_H
#define LUA_RUN_H

#include <stdint.h>

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert a library index into a bit mask.
 */
#define LUAR_LOAD_FLAG(n) (((uint16_t)1) << (n))

/**
 * Order in which the builtin libraries are loaded.
 */
enum LUAR_LOAD_ORDER {
    LUAR_LOAD_O_BASE,
    LUAR_LOAD_O_PACKAGE,
    LUAR_LOAD_O_CORO,
    LUAR_LOAD_O_TABLE,
    LUAR_LOAD_O_IO,
    LUAR_LOAD_O_OS,
    LUAR_LOAD_O_STRING,
    LUAR_LOAD_O_MATH,
    LUAR_LOAD_O_UTF8,
    LUAR_LOAD_O_DEBUG,
    LUAR_LOAD_O_ALL,
};

/** Load the base globals (_G) */
#define LUAR_LOAD_BASE      LUAR_LOAD_FLAG(LUAR_LOAD_O_BASE)
/** Load ´package´ */
#define LUAR_LOAD_PACKAGE   LUAR_LOAD_FLAG(LUAR_LOAD_O_PACKAGE)
/** Load ´coroutine´ */
#define LUAR_LOAD_CORO      LUAR_LOAD_FLAG(LUAR_LOAD_O_CORO)
/** Load ´table´ */
#define LUAR_LOAD_TABLE     LUAR_LOAD_FLAG(LUAR_LOAD_O_TABLE)
/** Load ´io´ */
#define LUAR_LOAD_IO        LUAR_LOAD_FLAG(LUAR_LOAD_O_IO)
/** Load ´os´ */
#define LUAR_LOAD_OS        LUAR_LOAD_FLAG(LUAR_LOAD_O_OS)
/** Load ´string´ */
#define LUAR_LOAD_STRING    LUAR_LOAD_FLAG(LUAR_LOAD_O_STRING)
/** Load ´math´ */
#define LUAR_LOAD_MATH      LUAR_LOAD_FLAG(LUAR_LOAD_O_MATH)
/** Load ´utf8´ */
#define LUAR_LOAD_UTF8      LUAR_LOAD_FLAG(LUAR_LOAD_O_UTF8)
/** Load ´debug´ */
#define LUAR_LOAD_DEBUG     LUAR_LOAD_FLAG(LUAR_LOAD_O_DEBUG)

/* TODO: maybe we can implement a "restricted base" package containing a subset
 * of base that is safe. */

#define LUAR_LOAD_ALL        (0xFFFF)               /** Load all standard modules */
#define LUAR_LOAD_NONE       (0x0000)               /** Do not load any modules */

/** Errors that can be raised when running lua code. */
enum LUAR_ERRORS {
    LUAR_EXIT,          /** The program exited without error. */
    LUAR_STARTUP_ERR,   /** Error setting up the interpreter. */
    LUAR_LOAD_ERR,      /** Error while loading libraries. */
    LUAR_NOMODULE,      /** The specified module could not be found. */
    LUAR_COMPILE_ERR,   /** The Lua code failed to compile. */
    LUAR_RUNTIME_ERR,   /** Error in code execution. */
    LUAR_MEMORY_ERR,    /** Lua could not allocate enough memory */
    LUAR_INTERNAL_ERR   /** Error inside the Lua VM.
                         *  Right now, if this happens, you may leak memory from
                         *  the heap. If your program is the only one using the
                         *  dynamic allocation, just clean the heap.
                         */
};

/**
 * Human-readable description of the errors
 */
extern const char *lua_riot_str_errors[];

/**
 * Return a string describing an error from LUAR_ERRORS.
 *
 * @param   errn    Error number as returned by lua_riot_do_buffer() or
 *                  lua_riot_do_buffer()
 *
 * @return A string describing the error, or "Unknown error".
 */
LUALIB_API const char *lua_riot_strerror(int errn);

/**
 * Initialize a lua state and set the panic handler.
 *
 * @todo    Use a per-state allocator
 *
 * @param   memory      Pointer to memory region that will be used as heap for
 *                      the allocator. Currently this functionality is not
 *                      supported and this must be set to NULL.
 * @param   mem_size    Size of the memory region that will be used as heap.
 *                      Currently this functionality is not supported and this
 *                      must be set to 0.
 * @param   panicf      Function to be passed to lua_atpanic. If set to NULL,
 *                      a generic function that does nothing will be used.
 *
 * @return      the new state, or NULL if there is a memory allocation error.
 */
LUALIB_API lua_State *lua_riot_newstate(void *memory, size_t mem_size,
                                        lua_CFunction panicf);

/**
 * Terminate the lua state.
 *
 * You must call this function if you want the finalizers (the __gc metamethods)
 * to be called.
 */
#ifndef LUA_DEBUG
    #define lua_riot_close lua_close
#else
    #define lua_riot_close luaB_close
#endif /* LUA_DEBUG */

/**
 * Open builtin libraries.
 *
 * This is like luaL_openlibs but it allows selecting which libraries will
 * be loaded.
 *
 * Libraries are loaded in the order specified by the LUAR_LOAD_ORDER enum. If
 * there is an error the load sequence is aborted and the index of the library
 * that failed is reported.
 *
 * If debugging is enabled (compile with the LUA_DEBUG macro), then the test
 * library will be unconditionally loaded.
 *
 * @param   L           Lua state
 * @param   modmask     Binary mask that indicates which modules should be
 *                      loaded. The mask is made from a combination of the
 *                      LUAR_LOAD_* macros.
 *
 * @return      The index of the library that failed to load, or LUAR_LOAD_O_ALL
 *              if all libraries were loaded.
 */
LUALIB_API int lua_riot_openlibs(lua_State *L, uint16_t modmask);

/**
 * Initialize the interpreter and run a built-in module in protected mode.
 *
 * In addition to running code in protected mode, this also sets a panic
 * function that long-jumps back to this function, in case there is an internal
 * interpreter error (LUAR_INTERNAL_ERR).
 * Right now the only things that the application can are either to abort(),
 * or to manually reset the heap (only if there's no other thread using it).
 *
 * @param       modname     name of the module.
 * @param       memory      @see lua_riot_newstate()
 * @param       mem_size    @see lua_riot_newstate()
 * @param       modmask     @see lua_riot_newstate()
 * @param[out]  retval      Value returned by the lua code, if it is a number,
 *                          or zero if no value is returned or the value is not
 *                          a number. If retval is null, the value is not stored.
 *
 * @return      An error code ( @see LUAR_ERRORS). LUAR_EXIT indicates no error.
 */
LUALIB_API int lua_riot_do_module(const char *modname, void *memory, size_t mem_size,
                                  uint16_t modmask, int *retval);

/**
 * Initialize the interpreter and run a user supplied buffer in protected mode.
 *
 * Only text data (i.e. lua source code) can be loaded by this function. The
 * lua interpreter is not robust against corrupt binary code.
 *
 * @see lua_riot_do_module() for more information on internal errors.
 *
 * @param       buf     Text data (lua source code).
 * @param       buflen  Size of the text data in bytes. If buf is
 *                      a zero-terminated string, the zero must not be counted.
 * @param       memory      @see lua_riot_newstate()
 * @param       mem_size    @see lua_riot_newstate()
 * @param       modmask     @see lua_riot_newstate()
 * @param[out]  retval      @see lua_riot_do_module()
 * @return      @see lua_riot_do_module().
 */
LUALIB_API int lua_riot_do_buffer(const uint8_t *buf, size_t buflen, void *memory,
                                  size_t mem_size, uint16_t modmask, int *retval);

#ifdef __cplusplus
extern "C" }
#endif

/** @} */

#endif /* LUA_RUN_H */
