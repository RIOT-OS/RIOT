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

#ifndef SHELL_H
#define SHELL_H

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
    XFA_USE_CONST(shell_command_xfa_t*, shell_commands_xfa); \
    static FLASH_ATTR const char _xfa_ ## cmd ## _cmd_name[] = #cmd; \
    static FLASH_ATTR const char _xfa_ ## cmd ## _cmd_desc[] = help; \
    static const shell_command_xfa_t _xfa_ ## cmd ## _cmd = { \
        .name = _xfa_ ## cmd ## _cmd_name, \
        .desc = _xfa_ ## cmd ## _cmd_desc, \
        .handler = &func \
    }; \
    XFA_ADD_PTR(shell_commands_xfa, cmd, cmd, &_xfa_ ## cmd ## _cmd)
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif

#endif /* SHELL_H */
/** @} */
