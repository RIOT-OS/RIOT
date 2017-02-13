/*
 * Copyright (C) 2016 Michael Andersen <m.andersen@berkeley.edu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_rtt_stdio SEGGER RTT stdio
 * @ingroup     sys
 *
 * @brief       stdio init/read/write functions for SEGGER RTT. This is
 *              designed to shadow the functions in uart_stdio
 *
 * @{
 * @file
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 */
#ifndef RTT_STDIO_H
#define RTT_STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief initialize the module. This is a noop.
 */
void uart_stdio_init(void);

/**
 * @brief read @p len bytes from stdio uart into @p buffer
 *
 * @param[out]  buffer  buffer to read into
 * @param[in]   len     nr of bytes to read
 *
 * @return nr of bytes read
 * @return <0 on error
 */
int uart_stdio_read(char* buffer, int len);

/**
 * @brief write @p len bytes from @p buffer into uart
 *
 * @param[in]   buffer  buffer to read from
 * @param[in]   len     nr of bytes to write
 *
 * @return nr of bytes written
 * @return <0 on error
 */
int uart_stdio_write(const char* buffer, int len);

/**
 * @brief enable stdin polling, at a power consumption cost. This is enabled
 *        by default unless RTT_STDIO_DISABLE_STDIN is defined.
 */
void rtt_stdio_enable_stdin(void);

/**
 * @brief enable stdout blocking and free space polling. This must be done
 *        with caution because if there is no RTT client attached, all
 *        writes to stdout will block indefinitely. This can be enabled
 *        automatically by defining RTT_STDIO_ENABLE_BLOCKING_STDOUT
 */
void rtt_stdio_enable_blocking_stdout(void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* RTT_STDIO_H */
