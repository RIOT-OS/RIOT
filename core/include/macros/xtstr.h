/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_macros
 * @{
 *
 * @file
 * @brief       Macro to return string representation of x
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef MACROS_XTSTR_H
#define MACROS_XTSTR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     XTSTR
 *
 * @brief   A macro to return the string respresentation of x
 */
#ifndef XTSTR
#define _XTSTR(x)    # x
#define XTSTR(x)    _XTSTR(x)
#endif /* XTSTR */

#ifdef __cplusplus
}
#endif

#endif /* MACROS_XTSTR_H */
/** @} */
