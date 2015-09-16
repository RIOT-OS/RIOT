/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     posix
 * @{
 *
 * @file
 * @brief   string operations
 *
 * @see     <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/strings.h.html">
 *              The Open Group Base Specifications Issue 7, <strings.h>
 *          </a>
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef STRINGS_H
#define STRINGS_H

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Returns the position of the first (least significant) bit set in
 *          integer *i*, or 0 if no bits are set in i.
 *
 * @param[in] i    an integer number.
 *
 * @return  Position (1 for least significant bit) of the first bit set,
 *          0 if no bits are set.
 *
 * @see <a href="http://pubs.opengroup.org/onlinepubs/9699919799/functions/ffs.html">
 *          The Open Group Base Specification Issue 7, ffs
 *      </a>
 */
#define ffs(i) __builtin_ffs(i)

#ifdef __cplusplus
}
#endif

#endif /* STRINGS_H */
