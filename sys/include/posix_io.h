/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_posix
 * @{
 * @file        posix_io.h
 * @brief       POSIX-like IO
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Oliver Hahm <oleg@hobbykeller.org>
 * @author      Martin Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __READ_H
#define __READ_H

#include "kernel_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPEN 0
#define CLOSE 1
#define READ 2
#define WRITE 3

struct posix_iop_t {
    int nbytes;
    char *buffer;
};

int posix_open(int pid, int flags);
int posix_close(int pid);
int posix_read(int pid, char *buffer, int bufsize);
int posix_write(int pid, char *buffer, int bufsize);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __READ_H */
