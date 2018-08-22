/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_stdio STDIO abstraction
 * @ingroup     sys
 *
 * @brief       Simple standard input/output (STDIO) abstraction for RIOT
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef STDIO_BASE_H
#define STDIO_BASE_H

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize the module
 */
void stdio_init(void);

/**
 * @brief read @p len bytes from stdio uart into @p buffer
 *
 * @param[out]  buffer  buffer to read into
 * @param[in]   max_len nr of bytes to read
 *
 * @return nr of bytes read
 * @return <0 on error
 */
ssize_t stdio_read(void* buffer, size_t max_len);

/**
 * @brief write @p len bytes from @p buffer into uart
 *
 * @param[in]   buffer  buffer to read from
 * @param[in]   len     nr of bytes to write
 *
 * @return nr of bytes written
 * @return <0 on error
 */
ssize_t stdio_write(const void* buffer, size_t len);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_BASE_H */
