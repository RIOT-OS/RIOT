/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    shell_sec Shell security functions
 * @ingroup     sys_shell
 * @brief       Shell security
 *
 * This module implements a customizable shell security system. It works with
 * 3 callbacks:
 *  - login, called when the login command is entered
 *  - passwd, called when the passwd command is entered
 *  - logout, called when the logout command is entered
 *
 * The callback are responsible to set the current security level. The shell will
 * then check the minimum security allowed for each command and compare it to the
 * security level set from @p shell_sec_set_level. If the intenal security level
 * is aboce the command one, the command handler will be called, otherwise an error
 * is displayed. This module allows a multi level security.
 *
 * @{
 *
 * @file
 * @brief       Shell security definition
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef SHELL_SEC_H
#define SHELL_SEC_H

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set the shell security callbacks
 *
 * @param[in] login_cb    login callback, called when the "login" command is entered
 * @param[in] passwd_cb   passwd callback, called when the "passwd" command is entered
 * @param[in] logout_cb   logout callback, called when the "logout" command is entered
 */
void shell_sec_set_cb(shell_command_handler_t login_cb, shell_command_handler_t passwd_cb, shell_command_handler_t logout_cb);

/**
 * @brief Set the current shell security level
 *
 * @param[in] level       security level to set
 */
void shell_sec_set_level(int level);

/**
 * @brief Get the current security level
 *
 * @return the current security level
 */
int shell_sec_get_level(void);

#ifdef __cplusplus
}
#endif

#endif /* SHELL_SEC_H */
/** @} */
