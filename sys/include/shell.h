/*
 * Copyright (C) 2009-2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_shell Shell
 * @ingroup     sys
 * @brief       Simple shell interpreter
 *
 * @{
 *
 * @file
 * @brief       Shell interface definition
 */

#ifndef __SHELL_H
#define __SHELL_H

#include <stdint.h>

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default shell buffer size (maximum line length shell can handle)
 */
#define SHELL_DEFAULT_BUFSIZE   (128)

/**
 * @brief           Protype of a shell callback handler.
 * @details         The functions supplied to shell_run() must use this signature.
 *                  The argument list is terminated with a NULL, i.e ``argv[argc] == NULL`.
 *                  ``argv[0]`` is the function name.
 *
 *                  Escape sequences are removed before the function is called.
 *
 *                  The called function may edit `argv` and the contained strings,
 *                  but it must be taken care of not to leave the boundaries of the array.
 *                  This functionality can be used by getopt() or a similar function.
 * @param[in]       argc   Number of arguments supplied to the function invocation.
 * @param[in]       argv   The supplied argument list.
 *
 * @return 0 on success
 * @return Anything else on failure
 */
typedef int (*shell_command_handler_t)(int argc, char **argv);

/**
 * @brief           A single command in the list of the supported commands.
 * @details         The list of commands is NULL terminated,
 *                  i.e. the last element must be ``{ NULL, NULL, NULL }``.
 */
typedef struct shell_command_t {
    const char *name; /**< Name of the function */
    const char *desc; /**< Description to print in the "help" command. */
    shell_command_handler_t handler; /**< The callback function. */
} shell_command_t;

/**
 * @brief           Start a shell.
 *
 * @param[in]       commands    ptr to array of command structs
 * @param[in]       line_buf    Buffer that will be used for reading a line
 * @param[in]       len         nr of bytes that fit in line_buf
 *
 * @returns         This function does not return.
 */
void shell_run(const shell_command_t *commands, char *line_buf, int len) NORETURN;

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_H */
/** @} */
