/*
 * Copyright (C) 2010 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_shell_commands Shell commands
 * @ingroup     sys
 *
 * @{
 *
 * @file
 * @brief       Shell command definitions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Disk manipulation command names
 * @{
 */
#define DISK_GET_SECTOR_SIZE    "dget_ssize"
#define DISK_GET_SECTOR_COUNT   "dget_scount"
#define DISK_GET_BLOCK_SIZE     "dget_bsize"
#define DISK_READ_SECTOR_CMD    "dread_sec"
#define DISK_READ_BYTES_CMD     "dread"
/** @} */

/**
 * @brief   List of shell commands
 */
extern const shell_command_t _shell_command_list[];

#ifdef __cplusplus
}
#endif

#endif /* SHELL_COMMANDS_H */
/** @} */
