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

#include "attributes.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * @brief           The internal state of a shell session.
 * @details         Use shell_init() to initialize the datum,
 *                  and shell_run() to run the REPL session.
 */
typedef struct shell_t {
    const shell_command_t *command_list; /**< The commandlist supplied to shell_init(). */
    uint16_t shell_buffer_size;          /**< The maximum line length supplied to shell_init(). */
    int (*readchar)(void);               /**< The read function supplied to shell_init(). */
    void (*put_char)(int);               /**< The write function supplied to shell_init(). */
} shell_t;

/**
 * @brief           Initialize a shell session state.
 * @param[out]      shell               The datum to initialize.
 * @param[in]       shell_commands      Null-terminated list of commands to understand.
 *                                      Supply `NULL` to only feature the default commands.
 * @param           shell_buffer_size   The backing buffer for the command line.
 *                                      Allocated on the stack!
 * @param           read_char           A blocking function that reads one 8-bit character at a time.
 *                                      The valid code range is [0;255].
 *                                      A value of `< 0` denotes a read error.
 * @param           put_char            Function used to print back the last read character.
 *                                      Only valid unsigned chars in [0;255] will be supplied.
 */
void shell_init(shell_t *shell,
                const shell_command_t *shell_commands,
                uint16_t shell_buffer_size,
                int (*read_char)(void),
                void (*put_char)(int));

/**
 * @brief           Start the shell session.
 * @param[in]       shell   The session that was previously initialized with shell_init().
 * @returns         This function does not return.
 */
void shell_run(shell_t *shell) NORETURN;

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_H */
/** @} */
