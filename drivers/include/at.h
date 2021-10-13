/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_at AT (Hayes) command set library
 * @ingroup     drivers_misc
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
 *
 * ## Unsolicited Result Codes (URC) ##
 * An unsolicited result code is a string message that is not triggered as a
 * information text response to a previous AT command and can be output at any
 * time to inform a specific event or status change.
 *
 * The module provides a basic URC handling by adding the `at_urc` module to the
 * application. This allows to @ref at_add_urc "register" and
 * @ref at_remove_urc "de-register" URC strings to check. Later,
 * @ref at_process_urc can be called to check if any of the registered URCs have
 * been detected. If a registered URC has been detected the correspondent
 * @ref at_urc_t::cb "callback function" is called. The mode of operation
 * requires that the user of the module processes periodically the URCs.
 *
 * Alternatively, one of the `at_urc_isr_<priority>` modules can be included.
 * `priority` can be one of `low`, `medium` or `highest`, which correspond to
 * the priority of the thread that processes the URCs. For more information on
 * the priorities check the @ref sys_event module. This will extend the
 * functionality of `at_urc` by processing the URCs when the @ref AT_RECV_EOL_2
 * character is detected and there is no pending response. This works by posting
 * an @ref sys_event "event" to an event thread that processes the URCs.
 *
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
#include <unistd.h>
#include <stdbool.h>

#include "isrpipe.h"
#include "periph/uart.h"
#include "clist.h"
#include "kernel_defines.h"

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_at_config     AT driver compile configuration
 * @ingroup config_drivers_misc
 * @{
 */
/**
 * @brief End of line character to send after the AT command.
 */
#if IS_ACTIVE(CONFIG_AT_SEND_EOL_WINDOWS)
#define CONFIG_AT_SEND_EOL   "\r\n"
#elif IS_ACTIVE(CONFIG_AT_SEND_EOL_UNIX)
#define CONFIG_AT_SEND_EOL   "\n"
#elif IS_ACTIVE(CONFIG_AT_SEND_EOL_MAC)
#define CONFIG_AT_SEND_EOL   "\r"
#endif

#ifndef CONFIG_AT_SEND_EOL
#define CONFIG_AT_SEND_EOL "\r"
#endif

/**
 * @brief Enable this to disable check for echo after an AT
 * command is sent.
 */
#ifdef DOXYGEN
#define CONFIG_AT_SEND_SKIP_ECHO
#endif

/**
 * @brief Enable/disable the expected echo after an AT command is sent.
 *
 * @deprecated Use inverse @ref CONFIG_AT_SEND_SKIP_ECHO instead.
 * Will be removed after 2021.01 release.
 */
#ifndef AT_SEND_ECHO
#if IS_ACTIVE(CONFIG_AT_SEND_SKIP_ECHO)
#define AT_SEND_ECHO 0
#else
#define AT_SEND_ECHO 1
#endif
#endif

/**
 * @brief 1st end of line character received (S3 aka CR character for a modem).
 */
#ifndef AT_RECV_EOL_1
#define AT_RECV_EOL_1   "\r"
#endif

/**
 * @brief 1st end of line character received (S4 aka LF character for a modem).
 */
#ifndef AT_RECV_EOL_2
#define AT_RECV_EOL_2   "\n"
#endif

/**
 * @brief default OK reply of an AT device.
 */
#ifndef CONFIG_AT_RECV_OK
#define CONFIG_AT_RECV_OK "OK"
#endif

/**
 * @brief default ERROR reply of an AT device.
 */
#ifndef CONFIG_AT_RECV_ERROR
#define CONFIG_AT_RECV_ERROR "ERROR"
#endif

#if defined(MODULE_AT_URC) || DOXYGEN

/**
 * @brief   Default buffer size used to process unsolicited result code data.
 *          (as exponent of 2^n).
 *
 *          As the buffer size ALWAYS needs to be power of two, this option
 *          represents the exponent of 2^n, which will be used as the size of
 *          the buffer.
 */
#ifndef CONFIG_AT_BUF_SIZE_EXP
#define CONFIG_AT_BUF_SIZE_EXP (7U)
#endif
/** @} */

/**
 * @brief   Size of buffer used to process unsolicited result code data.
 */
#ifndef AT_BUF_SIZE
#define AT_BUF_SIZE   (1 << CONFIG_AT_BUF_SIZE_EXP)
#endif

/**
 * @brief   Unsolicited result code callback
 *
 * @param[in]   arg     optional argument
 * @param[in]   code    urc string received from the device
 */
typedef void (*at_urc_cb_t)(void *arg, const char *code);

/**
 * @brief   Unsolicited result code data structure
 */
typedef struct {
    clist_node_t list_node; /**< node list */
    at_urc_cb_t cb;         /**< callback */
    const char *code;       /**< URC string which must match */
    void *arg;              /**< optional argument */
} at_urc_t;

#endif /* MODULE_AT_URC */

/** Shortcut for getting send end of line length */
#define AT_SEND_EOL_LEN  (sizeof(CONFIG_AT_SEND_EOL) - 1)

/**
 * @brief AT device structure
 */
typedef struct {
    isrpipe_t isrpipe;      /**< isrpipe used for getting data from uart */
    uart_t uart;            /**< UART device where the AT device is attached */
#ifdef MODULE_AT_URC
    clist_node_t urc_list;  /**< list to keep track of all registered urc's */
#ifdef MODULE_AT_URC_ISR
    bool awaiting_response; /**< indicates if the driver waits for a response */
    event_t event;          /**< event posted from ISR to process urc's */
#endif
#endif
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
 * @returns     success code UART_OK on success
 * @returns     error code UART_NODEV or UART_NOBAUD otherwise
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
 * If a line starts with "ERROR" or the buffer is full, the function returns -1.
 * If a line starts with "+CME ERROR" or +CMS ERROR", the function returns -2.
 * In this case resp_buf contains the error string.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   command     command to send
 * @param[out]  resp_buf    buffer for storing response
 * @param[in]   len         len of @p resp_buf
 * @param[in]   keep_eol    true to keep the CR character in the response
 * @param[in]   timeout     timeout (in usec)
 *
 * @returns     length of response on success
 * @returns     -1 on error
 * @returns     -2 on CMS or CME error
 */
ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf,
                              size_t len, bool keep_eol, uint32_t timeout);

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
 * @brief   Receives bytes into @p bytes buffer until the string pattern
 * @p string is received or the buffer is full.
 *
 * @param[in] dev               device to operate on
 * @param[in] string            string pattern to expect
 * @param[out] bytes            buffer to store received bytes
 * @param[in, out] bytes_len    pointer to the maximum number of bytes to
 *                              receive. On return stores the amount of received
 *                              bytes.
 * @param[in] timeout           timeout (in usec) of inactivity to finish read
 *
 * @returns                     0 on success
 * @returns                     <0 on error
 */
int at_recv_bytes_until_string(at_dev_t *dev, const char *string,
                               char *bytes, size_t *bytes_len,
                               uint32_t timeout);

/**
 * @brief   Send raw bytes to a device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer containing bytes to send
 * @param[in]   len     number of bytes to send
 */
void at_send_bytes(at_dev_t *dev, const char *bytes, size_t len);

/**
 * @brief   Receive raw bytes from a device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer where store received bytes
 * @param[in]   len     maximum number of bytes to receive
 * @param[in]   timeout timeout (in usec) of inactivity to finish read
 *
 * @returns     Number of bytes read, eventually zero if no bytes available
 */
ssize_t at_recv_bytes(at_dev_t *dev, char *bytes, size_t len, uint32_t timeout);

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
 * @param[in]   len         size of @p resp_buf
 * @param[in]   keep_eol    true to keep the CR character in the response
 * @param[in]   timeout     timeout (in usec)
 *
 * @returns     line length on success
 * @returns     <0 on error
 */
ssize_t at_readline(at_dev_t *dev, char *resp_buf, size_t len, bool keep_eol, uint32_t timeout);

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
 * @brief   Power device on
 *
 * @param[in]   dev     device to power on
 */
void at_dev_poweron(at_dev_t *dev);

/**
 * @brief   Power device off
 *
 * @param[in]   dev     device to power off
 */
void at_dev_poweroff(at_dev_t *dev);

#if defined(MODULE_AT_URC) || DOXYGEN
/**
 * @brief   Add a callback for an unsolicited response code
 *
 * @param[in]   dev     device to operate on
 * @param[in]   urc     unsolicited result code to register
 */
void at_add_urc(at_dev_t *dev, at_urc_t *urc);

/**
 * @brief   Remove an unsolicited response code from the list
 *
 * @param[in]   dev     device to operate on
 * @param[in]   urc     unsolicited result code to remove
 */
void at_remove_urc(at_dev_t *dev, at_urc_t *urc);

/**
 * @brief   Process out-of-band data received from the device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   timeout timeout (in usec)
 */
void at_process_urc(at_dev_t *dev, uint32_t timeout);
#endif

#ifdef __cplusplus
}
#endif

#endif /* AT_H */
/** @} */
