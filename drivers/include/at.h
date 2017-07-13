/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_at AT (Hayes) command set library
 * @ingroup     drivers
 * @brief       AT (Hayes) command set library
 *
 * This module provides functions to interact with devices using AT commands.
 *
 * Most functions compare the bytes echoed by the device with what they
 * intended to send, and bail out if there's no match.
 *
 * Furthermore, the library tries to copy with difficulties regarding different
 * line endings. It usually sends "<command><CR>", but expects
 * "<command>\LF\CR" as echo.
 *
 * As a debugging aid, when compiled with "-DAT_PRINT_INCOMING=1", every input
 * byte gets printed.
 * @{
 *
 * @file
 *
 * @brief       AT (Hayes) library interface
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef AT_H
#define AT_H

#include <stdint.h>

#include "isrpipe.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AT_END_OF_LINE
/** End of line character to send after the AT command */
#define AT_END_OF_LINE "\r"
#endif

/**
 * @brief AT device structure
 */
typedef struct {
    isrpipe_t isrpipe;      /**< isrpipe used for getting data from uart */
    uart_t uart;            /**< UART device where the AT device is attached */
} at_dev_t;


/**
 * @brief   Initialize AT device struct
 *
 * @param[in]   dev         struct to initialize
 * @param[in]   uart        UART the device is connected to
 * @param[in]   baudrate    baudrate of the device
 * @param[in]   buf         input buffer
 * @param[in]   bufsize     size of @p buf
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int at_dev_init(at_dev_t *dev, uart_t uart, uint32_t baudrate, char *buf, size_t bufsize);

/**
 * @brief   simple command helper
 *
 * This function sends an AT command to the device and waits for "OK".
 *
 * @param[in]   dev     device to operate on
 * @param[in]   command command string to send
 * @param[in]   timeout timeout (in usec)
 *
 * @returns     0 when device answers "OK"
 * @returns     <0 otherwise
 */
int at_send_cmd_wait_ok(at_dev_t *dev, const char *command, uint32_t timeout);

/**
 * @brief   send AT command, wait for a prompt
 *
 * This function will send the supplied @p command, then wait for the prompt (>)
 * character and return
 *
 * @param[in]   dev     device to operate on
 * @param[in]   command command string to send
 * @param[in]   timeout timeout (in usec)
 *
 * @return      0 when prompt is received
 * @return      <0 otherwise
 */
int at_send_cmd_wait_prompt(at_dev_t *dev, const char *command, uint32_t timeout);

/**
 * @brief   send AT command, wait for response
 *
 * This function will send the supplied @p command, then wait and return one
 * line of response.
 *
 * A possible empty line will be skipped.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   command     command to send
 * @param[out]  resp_buf    buffer for storing response
 * @param[in]   len         len of @p buffer
 * @param[in]   timeout     timeout (in usec)
 *
 * @returns     lenght of response on success
 * @returns     <0 on error
 */
ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout);

/**
 * @brief   send AT command, wait for multiline response
 *
 * This function will send the supplied @p command, then return all response
 * lines until the device sends "OK".
 *
 * If a line starts with "ERROR" or "+CME ERROR:", or the buffer is full, the
 * function returns -1.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   command     command to send
 * @param[out]  resp_buf    buffer for storing response
 * @param[in]   len         len of @p buffer
 * @param[in]   timeout     timeout (in usec)
 *
 * @returns     lenght of response on success
 * @returns     <0 on error
 */
ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout);

/**
 * @brief   Expect bytes from device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer containing bytes to expect
 * @param[in]   len     number of bytes to expect
 * @param[in]   timeout timeout (in usec)
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int at_expect_bytes(at_dev_t *dev, const char *bytes, size_t len, uint32_t timeout);

/**
 * @brief  Send raw bytes to a device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer containing bytes to send
 * @param[in]   len     number of bytes to send
 */
void at_send_bytes(at_dev_t *dev, const char *bytes, size_t len);

/**
 * @brief   send command to device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   command command to send
 * @param[in]   timeout timeout (in usec)
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int at_send_cmd(at_dev_t *dev, const char *command, uint32_t timeout);

/**
 * @brief   read a line from device
 *
 * @param[in]   dev         device to operate on
 * @param[in]   resp_buf    buffer to store line
 * @param[in]   len         size of @p buffer
 * @param[in]   timeout     timeout (in usec)
 *
 * @returns     line length on success
 * @returns     <0 on error
 */
ssize_t at_readline(at_dev_t *dev, char *resp_buf, size_t len, uint32_t timeout);

/**
 * @brief   drain device input buffer
 *
 * This function drains any possible bytes waiting in the device's input
 * buffer.
 *
 * @param[in]   dev     device to operate on
 */
void at_drain(at_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT_H */
