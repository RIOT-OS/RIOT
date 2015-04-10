/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SYS_READLINE_H_
#define SYS_READLINE_H_

#include "shell.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Module for a better shell.
 *
 * Options:
 * SHELL_HISTORY_SIZE=<n>: Use a simple history implementation. <n> gives the size of the history
 */

/**
 * FIXME: The buffer size must be defined globally available by the user of
 * the shell module. This definition is a copy of the one from "shell.c"
 */
#ifndef SHELL_BUFFER_SIZE
#define SHELL_BUFFER_SIZE 128
#endif // SHELL_BUFFER_SIZE

int readline(shell_t *shell, char *buf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* SYS_READLINE_H_ */
