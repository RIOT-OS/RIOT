/*
 * Copyright (C) 2009-2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_shell Shell
 * @ingroup     sys
 * @brief       Simple shell interpreter
 *
 * ## Security expectations
 *
 * Access to the shell grants access to the system that may exercise any power
 * the firmware can exercise. While some commands only provide limited access
 * to the system, and it is best practice for commands to validate their input,
 * there is no expectation of security of the system when an attacker gains
 * access to the shell.
 *
 * ## Usage
 *
 * Enable the `shell` module e.g. by adding the following snippet to your
 * applications `Makefile`.
 *
 * ```
 * USEMODULE += shell
 * ```
 *
 * And run the shell using @ref shell_run_forever e.g. from the `main` thread
 * after everything is set up. This call will never return.
 *
 * ## Builtin Commands
 *
 * The commands `help` and `help_json` are builtins that print the list of
 * available commands: The former prints a human readable table and is always
 * available, the latter requires module `shell_builtin_cmd_help_json` to be
 * used and will give the same info machine readable.
 *
 * @{
 *
 * @file
 * @brief       Shell interface definition
 */

#include <stdint.h>
#include "periph/pm.h"

#include "modules.h"
#include "xfa.h"

#ifndef __cplusplus
#include "flash_utils.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup sys_shell_config Shell compile time configurations
 * @ingroup config
 * @{
 */
/**
 * @brief Shutdown RIOT on shell exit
 *
 * @note On native platform this option defaults to 1.
 */
#ifndef CONFIG_SHELL_SHUTDOWN_ON_EXIT
/* Some systems (e.g Ubuntu 20.04) close stdin on CTRL-D / EOF
 * That means we can't just re-start the shell.
 * Instead terminate RIOT, which is also the behavior a user would
 * expect from a CLI application.
 */
#  if defined(CPU_NATIVE) && !IS_ACTIVE(MODULE_SHELL_LOCK)
#    define CONFIG_SHELL_SHUTDOWN_ON_EXIT 1
#  else
#    define CONFIG_SHELL_SHUTDOWN_ON_EXIT 0
#  endif
#endif

/**
 * @brief Set to 1 to disable shell's echo
 */
#ifndef CONFIG_SHELL_NO_ECHO
#define CONFIG_SHELL_NO_ECHO 0
#endif

/**
 * @brief Set to 1 to disable shell's prompt
 */
#ifndef CONFIG_SHELL_NO_PROMPT
#define CONFIG_SHELL_NO_PROMPT 0
#endif

/** @} */

/**
 * @brief Default shell buffer size (maximum line length shell can handle)
 *
 * @warning When terminals that buffer input and send the full command line in
 *   one go are used on stdin implementations with fast bursts of data,
 *   it may be necessary to increase the @ref STDIO_RX_BUFSIZE to make
 *   practical use of this buffer, especially because the current mechanism of
 *   passing stdin (`isrpipe_t stdin_isrpipe`) does not support backpressure
 *   and overflows silently. As a consequence, commands through such terminals
 *   appear to be truncated at @ref STDIO_RX_BUFSIZE bytes (defaulting to 64)
 *   unless the command is sent in parts (on many terminals, by pressing Ctrl-D
 *   half way through the command).
 *
 *   For example, this affects systems with direct USB stdio (@ref
 *   usbus_cdc_acm_stdio) with the default terminal `pyterm`.
 */
#define SHELL_DEFAULT_BUFSIZE   (128)

/**
 * @brief           Optional hook after readline has triggered.
 * @details         User implemented function gets called after the shell
 *                  readline is complete.
 * @note            Only executed with the `shell_hooks` module.
 */
void shell_post_readline_hook(void);

/**
 * @brief           Optional hook before shell command is called.
 * @details         User implemented function gets called before a valid shell
 *                  command will be called.
 * @note            Only executed with the `shell_hooks` module.
 *
 * @param[in]       argc   Number of arguments supplied to the function invocation.
 * @param[in]       argv   The supplied argument list.
 */
void shell_pre_command_hook(int argc, char **argv);

/**
 * @brief           Optional hook after shell command is called.
 * @details         User implemented function gets called before a valid shell
 *                  command will be called.
 * @note            Only executed with the `shell_hooks` module.
 *
 * @param[in]       ret    Return value of the shell command.
 * @param[in]       argc   Number of arguments supplied to the function invocation.
 * @param[in]       argv   The supplied argument list.
 */
void shell_post_command_hook(int ret, int argc, char **argv);

/**
 * @brief           Prototype of a shell callback handler.
 * @details         The functions supplied to shell_run() must use this signature.
 *                  It is designed to mimic the function signature of `main()`.
 *                  For this reason, the argument list is terminated with a
 *                  `NULL`, i.e `argv[argc] == NULL` (which is an ANSI-C
 *                  requirement, and a detail that newlib's `getopt()`
 *                  implementation relies on). The function name is passed in
 *                  `argv[0]`.
 *
 *                  Escape sequences are removed before the function is called.
 *
 *                  The called function may edit `argv` and the contained strings,
 *                  but it must be taken care of not to leave the boundaries of the array.
 *                  This functionality is another property that many `getopt()`
 *                  implementations rely on to provide their so-called "permute"
 *                  feature extension.
 *
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

#ifndef __cplusplus
/**
 * @brief           A single command in the list of the supported commands.
 *
 * This type is used internally by the @ref SHELL_COMMAND macro.
 */
typedef struct {
    FLASH_ATTR const char *name; /**< Name of the function */
    FLASH_ATTR const char *desc; /**< Description to print in the "help"
                                  *   command. */
    shell_command_handler_t handler; /**< The callback function. */
} shell_command_xfa_t;
#endif /* __cplusplus */

/**
 * @brief           Start a shell and exit once EOF is reached.
 *
 * @param[in]       commands    ptr to array of command structs
 * @param[in]       line_buf    Buffer that will be used for reading a line
 * @param[in]       len         nr of bytes that fit in line_buf
 */
void shell_run_once(const shell_command_t *commands, char *line_buf, int len);

/**
 * @brief           Start a shell and restart it if it exits
 *
 *                  If `CONFIG_SHELL_SHUTDOWN_ON_EXIT` is set (e.g. on native)
 *                  the shell will instead shut down RIOT once EOF is reached.
 *
 * @param[in]       commands    ptr to array of command structs
 * @param[in]       line_buf    Buffer that will be used for reading a line
 * @param[in]       len         nr of bytes that fit in line_buf
 */
static inline void shell_run_forever(const shell_command_t *commands,
                                     char *line_buf, int len)
{
    while (1) {
        shell_run_once(commands, line_buf, len);

        if (IS_ACTIVE(CONFIG_SHELL_SHUTDOWN_ON_EXIT)) {
            pm_off();
        }
    }
}

/**
 * @brief           Back-porting alias for @ref shell_run_forever
 *
 * @param[in]       commands    ptr to array of command structs
 * @param[in]       line_buf    Buffer that will be used for reading a line
 * @param[in]       len         nr of bytes that fit in line_buf
 */
static inline void shell_run(const shell_command_t *commands,
                             char *line_buf, int len)
{
    shell_run_forever(commands, line_buf, len);
}

/**
 * @brief           Parse and run a line of text as a shell command with
 *                  arguments.
 *
 * @param[in]       commands    ptr to array of command structs
 * @param[in]       line        The input line to parse
 *
 * @returns         return value of the found command
 * @returns         -ENOEXEC if no valid command could be found
 */
int shell_handle_input_line(const shell_command_t *commands, char *line);

/**
 * @brief           Read shell commands from a file and run them.
 *
 * @note            This requires the `vfs` module.
 *
 * @param[in]       commands    ptr to array of command structs
 * @param[in]       filename    file to read shell commands from
 * @param[out]      line_nr     line on which an error occurred, may be NULL
 *
 * @returns         0 if all commands were executed successful
 *                  error return of failed command otherwise
 */
int shell_parse_file(const shell_command_t *commands,
                     const char *filename, unsigned *line_nr);

/**
 * @brief   Read a single line from standard input into a buffer.
 *
 * In addition to copying characters, this routine echoes the line back to
 * stdout and also supports primitive line editing.
 *
 * If the input line is too long, the input will still be consumed until the end
 * to prevent the next line from containing garbage.
 *
 * We allow Unix (`\n`), DOS (`\r\n`), and Mac linebreaks (`\r`).
 * QEMU transmits only a single `\r` == 13 on hitting enter ("-serial stdio").
 * DOS newlines are handled like hitting enter twice.
 *
 * @param   buf     Buffer where the input will be placed.
 * @param   size    Size of the buffer. The maximum line length will be one less
 *                  than size, to accommodate for the null terminator.
 *                  The minimum buffer size is 1.
 *
 * @return  length of the read line, excluding the terminator, if reading was
 *          successful.
 * @return  EOF, if the end of the input stream was reached.
 * @return  -ENOBUFS if the buffer size was exceeded.
 */
int shell_readline(char *buf, size_t size);

#ifndef __cplusplus
/**
 * @brief   Define shell command
 *
 * @note    This is not available from C++, but a trivial C file can easily
 *          hook up a `extern "C"` function implemented in C++.
 *
 * This macro is a helper for defining a shell command and adding it to the
 * shell commands XFA (cross file array).
 *
 * Shell commands added using this macros will be sorted *after* builtins and
 * commands passed via parameter to `shell_run_once()`. If a command with the
 * same name exists in any of those, they will make a command added via this
 * macro inaccassible.
 *
 * Commands added with this macro will be sorted alphanumerically by `name`.
 *
 * @experimental This should be considered experimental API, subject to change
 *               without notice!
 *
 * Example:
 *
 * ```.c
 * #include "shell.h"
 * static int _my_command(int argc, char **argv) {
 *   // ...
 * }
 * SHELL_COMMAND(my_command, "my command help text", _my_command);
 * ```
 */
#define SHELL_COMMAND(cmd, help, func) \
    XFA_USE_CONST(shell_command_xfa_t, shell_commands_xfa_v2); \
    static FLASH_ATTR const char _xfa_ ## cmd ## _cmd_name[] = #cmd; \
    static FLASH_ATTR const char _xfa_ ## cmd ## _cmd_desc[] = help; \
    XFA_CONST(shell_command_xfa_t, shell_commands_xfa_v2, 0) _xfa_ ## cmd ## _cmd = { \
        .name = _xfa_ ## cmd ## _cmd_name, \
        .desc = _xfa_ ## cmd ## _cmd_desc, \
        .handler = &func \
    };
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif

/** @} */
