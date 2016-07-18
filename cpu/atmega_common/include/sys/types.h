/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SYS_TYPES_H_
#define SYS_TYPES_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef      int32_t blkcnt_t;    /**< Used for file block counts */
typedef      int32_t blksize_t;   /**< Used for block sizes */
typedef     uint32_t clock_t;     /**< Used for system times in clock ticks */
typedef     uint32_t clockid_t;   /**< Used for clock ID type in the clock and timer functions */
typedef      int16_t dev_t;       /**< Used for device IDs */
typedef     uint32_t fsblkcnt_t;  /**< Used for file system block counts */
typedef     uint32_t fsfilcnt_t;  /**< Used for file system file counts */
typedef     uint16_t gid_t;       /**< Used for group IDs */
typedef     uint16_t id_t;        /**< Used as a general identifier */
typedef     uint32_t ino_t;       /**< Used for file serial numbers */
typedef     uint32_t key_t;       /**< Used for XSI interprocess communication */
typedef     uint32_t mode_t;      /**< Used for some file attributes */
typedef     uint16_t nlink_t;     /**< Used for link counts */
typedef      int32_t off_t;       /**< Used for file sizes and offsets */
typedef          int pid_t;       /**< Used for process IDs and process group IDs */
typedef unsigned int size_t;      /**< Used for sizes of objects */
typedef   signed int ssize_t;     /**< Used for a count of bytes or an error indication */
typedef      int32_t suseconds_t; /**< Used for time in microseconds */
typedef      int32_t time_t;      /**< Used for time in seconds */
typedef     uint32_t timer_t;     /**< Used for timer ID returned by timer_create() */
typedef     uint16_t uid_t;       /**< Used for user IDs */
typedef     uint32_t useconds_t;  /**< Used for time in microseconds */

#ifdef __cplusplus
}
#endif

#endif /* SYS_TYPES_H_ */
