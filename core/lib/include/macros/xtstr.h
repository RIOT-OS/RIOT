/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     core_macros
 * @{
 *
 * @file
 * @brief       Macro to return string representation of x
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     XTSTR
 *
 * @brief   A macro to return the string representation of x
 */
#ifndef XTSTR
# ifndef DOXYGEN
#  define _XTSTR(x)    # x
# endif /* DOXYGEN */
# define XTSTR(x)    _XTSTR(x)
#endif /* XTSTR */

#ifdef __cplusplus
}
#endif

/** @} */
