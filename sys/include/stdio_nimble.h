/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup     sys_stdio_nimble STDIO over NimBLE
 * @ingroup      sys_stdio
 *
 * To enable stdio over nimBLE, add the module `stdio_nimble`:
 *
 *     USEMODULE += stdio_nimble
 *
 * @experimental This feature is experimental as some use-cases, such as examples/twr_aloha, show
 *               unexpected behaviour.
 *
 * @brief        Standard input/output backend using NimBLE.
 *
 * @note         'stdio_read' blocks until at least one character was read.
 *
 * @note         'stdio_write' is considered non-blocking even though it uses a mutex to protect the
 *               write buffer since only 'stdio_write' uses this mutex. Characters will be written
 *               in FIFO mode. Characters that do not fit in the buffer will be dropped.
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

/**
 * @brief Whether to clear the buffers when establishing a new connection or
 *        not. Defaults to true.
 */
#ifndef CONFIG_STDIO_NIMBLE_CLEAR_BUFFER_ON_CONNECT
#define CONFIG_STDIO_NIMBLE_CLEAR_BUFFER_ON_CONNECT 1
#endif

/**
 * @brief Size of buffer for stdin in bytes
 *
 * @note Must be a power of two!
 */
#ifndef CONFIG_STDIO_NIMBLE_STDIN_BUFSIZE
#define CONFIG_STDIO_NIMBLE_STDIN_BUFSIZE 1024
#endif

/**
 * @brief Size of buffer for stdout in bytes
 *
 * @note Must be a power of two!
 */
#ifndef CONFIG_STDIO_NIMBLE_STDOUT_BUFSIZE
#define CONFIG_STDIO_NIMBLE_STDOUT_BUFSIZE 2048
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_NIMBLE_H */
