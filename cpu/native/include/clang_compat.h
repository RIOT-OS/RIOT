/*
 * clang_compat.h Undefines macros of clang on OSX to use RIOT's macros
 *
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CLANG_COMPAT_H
#define CLANG_COMPAT_H

#ifdef __cplusplus
extern "C" {
#endif

#undef htons
#undef htonl
#undef htonll
#undef ntohs
#undef ntohl
#undef ntohll

#ifdef __cplusplus
}
#endif

#endif /* CLANG_COMPAT_H */
