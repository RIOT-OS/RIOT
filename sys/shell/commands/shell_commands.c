/*
 * Copyright (C) 2014  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides prototypes and sets up available shell commands
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include "shell_commands.h"

#define __end__ __attribute__((used,section("._shell_command_list_end"))) static
__end__ shell_command_t _null_command = {NULL, NULL, NULL};
