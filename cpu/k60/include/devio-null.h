/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           Device I/O helpers for a no-op device.
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#ifndef DEVIO_NULL_H_
#define DEVIO_NULL_H_

#ifdef __cplusplus
extern "C"
{
#endif

int  devnull_open_r(struct _reent *r, const char *path, int flags, int mode);
int  devnull_close_r(struct _reent *r, int fd);
long devnull_write_r(struct _reent *r, int fd, const char *ptr, int len);
long devnull_read_r(struct _reent *r, int fd, char *ptr, int len);
long devnull_lseek_r(struct _reent *r, int fd, int ptr, int dir);
long devnull_fstat_r(struct _reent *r, int fd, char *ptr, int len);

#ifdef __cplusplus
}
#endif

#endif /* !defined(DEVIO_NULL_H_) */
/** @} */
