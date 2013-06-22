#ifndef __SHELL_COMMANDS_H
#define __SHELL_COMMANDS_H

#include <shell.h>

#define DISK_GET_SECTOR_SIZE    "dget_ssize"
#define DISK_GET_SECTOR_COUNT   "dget_scount"
#define DISK_GET_BLOCK_SIZE     "dget_bsize"
#define DISK_READ_SECTOR_CMD    "dread_sec"
#define DISK_READ_BYTES_CMD     "dread"

extern const shell_command_t _shell_command_list[];

#endif /* __SHELL_COMMANDS_H */
