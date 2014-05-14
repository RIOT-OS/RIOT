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
 * @file        side_sys__stat.h
 * @brief       Auxilary header to extend <sys/stat.h>
 *
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_stat.h.html">
 *              The Open Group Base Specifications Issue 7, <sys/stat.h>
 *          </a>
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef __SIDE_SYS__STAT_H_
#define __SIDE_SYS__STAT_H_

#include <sys/stat.h>

/* TODO */
int lstat(const char *restrict, struct stat *restrict);

#endif /* __SIDE_SYS__STAT_H_ */
/** @} */
