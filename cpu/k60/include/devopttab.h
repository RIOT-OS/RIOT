/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <reent.h>

/**
 * @ingroup         cpu_k60
 * @{
 *
 * @file
 * @brief           Device operations table.
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */
#ifndef DEVOPTTAB_H_
#define DEVOPTTAB_H_

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Device operations table
 *
 * Inspired by http://neptune.billgatliff.com/newlib.html
 *
 * A simple "device operations" table, with function pointers for all the kinds
 * of activities you would expect a stream-like device to support.
 */
typedef struct {
    const char *name; /**< Device filename */
    const int isatty; /**< isatty() return code (usually 0 or 1) */
    const int st_mode; /**< st_mode code, see man 2 stat */
    int (*open_r)(struct _reent *r, const char *path, int flags,
                  int mode);    /**< pointer to open() function for this device */
    int (*close_r)(struct _reent *r, int fd);   /**< pointer to close() function for this device */
    long(*write_r)(struct _reent *r, int fd, const char *ptr,
                   int len);   /**< pointer to write() function for this device */
    long(*read_r)(struct _reent *r, int fd, char *ptr,
                  int len);    /**< pointer to read() function for this device */
    long(*lseek_r)(struct _reent *r, int fd, int ptr,
                   int dir);   /**< pointer to lseek() function for this device */
    long(*fstat_r)(struct _reent *r, int fd, char *ptr,
                   int len);   /**< pointer to fstat() function for this device */
} devoptab_t;

#ifdef __cplusplus
}
#endif

#endif
/** @} */
