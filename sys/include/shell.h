/*
 * Copyright (C) 2009-2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_shell Shell
 * @ingroup     sys
 * @brief       Simple shell interpreter
 */

#ifndef __SHELL_H
#define __SHELL_H

#include <stdint.h>

#include "attributes.h"

/**
 * @brief           Protype of a shell callback handler.
 * @details         The functions supplied to shell_init() must use this signature.
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
 */
typedef void (*shell_command_handler_t)(int argc, char **argv);

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
 * @brief           Start the shell session.
 * @param[in]       shell_commands      Null-terminated list of commands to understand.
 *                                      Supply `NULL` to only feature the default commands.
 * @param           shell_buffer_size   The backing buffer for the command line.
 *                                      Allocated on the stack!
 * @returns         This function does not return.
 */
void shell_run(const shell_command_t *shell_commands, uint16_t shell_buffer_size) NORETURN;

#endif /* __SHELL_H */
