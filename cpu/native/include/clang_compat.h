/*
 * clang_compat.h Undefines macros of clang on OSX to use RIOT's macros
 *
 * Copyright (C) 2014 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef __CLANG_COMPAT_H

#undef HTONS
#undef HTONL
#undef HTONLL
#undef NTOHS
#undef NTOHL
#undef NTOHLL

#endif
