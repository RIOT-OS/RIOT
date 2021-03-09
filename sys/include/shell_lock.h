/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup     sys_shell_lock Shell lock
 * @ingroup      sys
 * @brief        Simple module to provide a password protection for the shell.
 * @experimental This module is an experimental feature and only shows as a PoC
 *               how the shell could be protected with a password. Don't expect
 *               relevant security from it for production, as Man-in-the-Middle
 *               attacks are possible depending on the used connection method!
 *
 * @{
 *
 * @file
 * @brief       Shell interface definition
 */

#ifndef SHELL_LOCK_H
#define SHELL_LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "shell.h"

#ifdef MODULE_SHELL_LOCK
#ifndef SHELL_LOCK_PASSWORD
        #error Using MODULE_SHELL_LOCK requires defining SHELL_LOCK_PASSWORD
    #endif /* SHELL_LOCK_PASSWORD */
#endif /* MODULE_SHELL_LOCK */

/**
 * @brief Lock the login process after given attempts of failed logins for
 * a few seconds
 */
#define ATTEMPTS_BEFORE_TIME_LOCK 3

#ifndef SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS
/**
 * @brief Lock the shell after this time span without user input
 *        Defaults to 5 minutes but can be overwritten in the applications
 *        Makefile.
 */
#define SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS (5 * 60 * 1000)
#endif

/**
 * @brief   Entry point for the lock mechanism. If locked, the user will
 *          be asked for a password. This function won't return until the
 *          correct password has been entered.
 *
 * @param[in]   line_buf    Buffer for reading in the password from stdin
 * @param[in]   buf_size    Buffer size
 */
void shell_lock_checkpoint(char *line_buf, int buf_size);

/**
 * @brief   Returns true, if the shell is in the locked state.
 *
 * @return  Whether the shell is locked or not.
 */
bool shell_lock_is_locked(void);

#ifdef MODULE_SHELL_LOCK_AUTO_LOCKING
/**
 * @brief   Restart the timeout interval before the shell is locked
 *          automatically.
 */
void shell_lock_auto_lock_refresh(void);
#endif /* MODULE_SHELL_LOCK_AUTO_LOCKING */

/**
 * @brief   Command list containing all commands used for this module.
 *
 * This should be independent from the module "shell_commands", as the user may
 * don't want any other command than the mandatory "lock"-command.
 */
extern const shell_command_t _shell_lock_command_list[];

#ifdef __cplusplus
}
#endif

#endif /* SHELL_LOCK_H */
/** @} */
