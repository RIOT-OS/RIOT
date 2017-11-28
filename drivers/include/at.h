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
 * Furthermore, the library tries to cope with difficulties regarding different
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
#include "clist.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AT_EOL
/** End of line character to send after the AT command */
#define AT_EOL "\r"
#endif

/** Shortcut for getting end of line length */
#define AT_EOL_LEN  (sizeof(AT_EOL) - 1)

#ifndef AT_BUF_SIZE
/** Internal buffer size used to process out-of-band data */
#define AT_BUF_SIZE (128)
#endif

/**
 * @brief   Out-of-band data callback
 *
 * @param[in]   arg     optional argument
 * @param[in]   urc     urc string received from the device
 */
typedef void (*at_oob_cb_t)(void *arg, const char *urc);

/**
 * @brief   Out-of-band data structure
 */
typedef struct {
    clist_node_t list_node; /**< node list */
    at_oob_cb_t cb;         /**< callback */
    const char *urc;        /**< URC which must match */
    void *arg;              /**< optional argument */
} at_oob_t;

/**
 * @brief AT device structure
 */
typedef struct {
    isrpipe_t isrpipe;      /**< isrpipe used for getting data from uart */
    uart_t uart;            /**< UART device where the AT device is attached */
    clist_node_t oob_list;
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
 * @brief   Simple command helper
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
 * @brief   Send AT command, wait for a prompt
 *
 * This function sends the supplied @p command, then waits for the prompt (>)
 * character and returns
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
 * @brief   Send AT command, wait for response
 *
 * This function sends the supplied @p command, then waits and returns one
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
 * @returns     length of response on success
 * @returns     <0 on error
 */
ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout);

/**
 * @brief   Send AT command, wait for multiline response
 *
 * This function sends the supplied @p command, then returns all response
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
 * @returns     length of response on success
 * @returns     <0 on error
 */
ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf, size_t len, uint32_t timeout);

/**
 * @brief   Expect bytes from device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer containing bytes to expect (NULL-terminated)
 * @param[in]   timeout timeout (in usec)
 *
 * @returns     0 on success
 * @returns     <0 otherwise
 */
int at_expect_bytes(at_dev_t *dev, const char *bytes, uint32_t timeout);

/**
 * @brief   Send raw bytes to a device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer containing bytes to send
 * @param[in]   len     number of bytes to send
 */
void at_send_bytes(at_dev_t *dev, const char *bytes, size_t len);

/**
 * @brief   Send command to device
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
 * @brief   Read a line from device
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
 * @brief   Drain device input buffer
 *
 * This function drains any possible bytes waiting in the device's input
 * buffer.
 *
 * @param[in]   dev     device to operate on
 */
void at_drain(at_dev_t *dev);

/**
 * @brief   add a callback for an out-of-bound data
 *
 * @param[in]   dev     device to operate on
 * @param[in]   oob     out-of-band value to register
 */
void at_add_oob(at_dev_t *dev, at_oob_t *oob);

/**
 * @brief   remove an out-of-band data from the list
 *
 * @param[in]   dev     device to operate on
 * @param[in]   oob     out-of-band value to remove
 */
void at_remove_oob(at_dev_t *dev, at_oob_t *oob);

/**
 * @brief   process out-of-band data received from the device
 *
 * The function returns immediately if no data is available to be read.
 *
 * @param[in]   dev     device to operate on
 */
void at_process_oob(at_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT_H */
