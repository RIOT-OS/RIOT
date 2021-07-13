/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_stdio_nimble STDIO over NimBLE
 * @ingroup     sys
 *
 * @brief       Standard input/output backend using NimBLE.
 *
 * @{
 * @file
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 */

#ifndef STDIO_NIMBLE_H
#define STDIO_NIMBLE_H

#include "stdio_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STDIN_BUFSIZE
    /**
    * @brief Size of buffer for stdin in bytes
    */
    #define STDIN_BUFSIZE 1024
#endif

#ifndef STDOUT_BUFSIZE
    /**
    * @brief Size of buffer for stdout in bytes
    */
    #define STDOUT_BUFSIZE 2048
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_NIMBLE_H */
