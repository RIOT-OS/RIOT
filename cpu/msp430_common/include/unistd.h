/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef UNISTD_H_
#define UNISTD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "msp430_types.h"

#define STDIN_FILENO    0   ///< stdin file descriptor
#define STDOUT_FILENO   1   ///< stdout file descriptor
#define STDERR_FILENO   2   ///< stderr file descriptor

int close(int fildes);

typedef uint32_t useconds_t;
int usleep(useconds_t usec);
unsigned int sleep(unsigned int seconds);

#ifdef __cplusplus
}
#endif

#endif /* UNISTD_H_ */
