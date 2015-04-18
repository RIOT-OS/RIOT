/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k60
 * @{
 *
 * @file
 * @brief       Device I/O helpers for a no-op device, implementations.
 *
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 *
 * @}
 */

#include <reent.h>
#include <errno.h>
#include <string.h>
#include "devio-null.h"

int  devnull_open_r(struct _reent *r, const char *path, int flags, int mode)
{
    return 0;
}

int  devnull_close_r(struct _reent *r, int fd)
{
    return 0;
}

long devnull_write_r(struct _reent *r, int fd, const char *ptr, int len)
{
    /* Aaand... it's gone!*/
    return len;
}

long devnull_read_r(struct _reent *r, int fd, char *ptr, int len)
{
    /* Read null bytes */
    memset(ptr, '\0', len);
    return len;
}

long devnull_lseek_r(struct _reent *r, int fd, int ptr, int dir)
{
    r->_errno = ENOSYS;
    return -1;
}

long devnull_fstat_r(struct _reent *r, int fd, char *ptr, int len)
{
    r->_errno = ENOSYS;
    return -1;
}
