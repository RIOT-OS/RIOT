/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  posix
 * @{
 *
 * @file        dirent.h
 * @brief       format of directory entries
 *
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/dirent.h.html">
 *              The Open Group Base Specifications Issue 7, <dirent.h>
 *          </a>
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __DIRENT_H_
#define __DIRENT_H_

#include <sys/types.h>

#define DIR_NAME_LEN (32)

/* TODO */
struct dirent {
    ino_t d_ino;
    char d_name[DIR_NAME_LEN];
};

/* TODO */
typedef void DIR;

/* TODO */
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int closedir(DIR *);

#endif /* __DIRENT_H_ */
/** @} */
