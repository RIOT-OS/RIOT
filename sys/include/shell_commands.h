/*
 * Copyright (C) 2010 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
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

#define DISK_GET_SECTOR_SIZE    "dget_ssize"
#define DISK_GET_SECTOR_COUNT   "dget_scount"
#define DISK_GET_BLOCK_SIZE     "dget_bsize"
#define DISK_READ_SECTOR_CMD    "dread_sec"
#define DISK_READ_BYTES_CMD     "dread"

#define __registered__ __attribute__((used,section("._shell_command_list"))) static
#define REGISTER_SHELL_COMMAND(name, desc, handler) __registered__ shell_command_t handler ## _command = {name, desc, handler}

extern const shell_command_t _shell_command_list;

#ifdef __cplusplus
}
#endif

#endif /* SHELL_COMMANDS_H */
/** @} */
