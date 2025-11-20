/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

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
 * line endings. It usually sends `<command><CR>`, but expects
 * `<command>\LF\CR` as echo.
 *
 * As a debugging aid, when compiled with `-DAT_PRINT_INCOMING=1`, every input
 * byte gets printed.
 *
 * ## Command echoing ##
 * Most DCEs (Data Circuit-terminating Equipment, aka modem) support command
 * echoing and enable it by default, and so does this driver.
 * If you disabled echoing on the DCE, you can compile this driver NOT to expect
 * echoing by defining CONFIG_AT_SEND_SKIP_ECHO.
 * Note, if the driver is NOT expecting command echoing but the DCE is echoing,
 * it should work just fine if and only if the EOL sequences of both DCE and
 * DTE (Data Terminal Equipmend - i.e. the device using this driver) match, i.e.
 * `AT_RECV_EOL_1 AT_RECV_EOL_2 == AT_SEND_EOL`.
 * In other words, if you are unsure about the echoing behavior of the DCE or
 * want to support both, set AT_SEND_EOL = AT_RECV_EOL_1 AT_RECV_EOL_2 and
 * define CONFIG_AT_SEND_SKIP_ECHO. This works because the URC (Unsolicited
 * Result Code) logic will intercept the echoes (see below).
 *
 * ## Unsolicited Result Codes (URC) ##
 * An unsolicited result code is a string message that is not triggered as a
 * information text response to a previous AT command and can be output at any
 * time to inform a specific event or status change.
 *
 * Some DCEs (Data Circuit-terminating Equipment, aka modem), like the LTE
 * modules from uBlox define a grace period where URCs are guaranteed NOT to be
 * sent as the time span between:
 *  - the command EOL character reception AND command being internally accepted
 *  - the EOL character of the last response line
 *
 * As follows, there is an indeterminate amount of time between:
 *  - the command EOL character being sent
 *  - the command EOL character reception AND command being internally accepted,
 *    i.e. the begin of the grace period
 *
 * In other words, we can get a URC (or more?) just after issuing the command
 * and before the first line of response. The net effect is that such URCs will
 * appear to be the first line of response to the last issued command.
 *
 * Regardless of whether URC handling is enabled or not, URC interception
 * mechanics depend on command echoing:
 *  1. echo enabled: by observation, it seems that the grace period begins
 *   BEFORE the echoed command. This has the advantage that we ALWAYS know what
 *   the first line of response must look like and so if it doesn't, then it's a
 *   URC. Thus, any procedure that calls at_send_cmd() internally will catch any
 *   URC.
 *  2. echo disabled: commands that expect a particular type of response (e.g.
 *   @ref at_send_cmd_get_resp_wait_ok() with a non-trivial prefix,
 *   @ref at_send_cmd_wait_ok() etc.) will catch any URC. For the rest, it is
 *   the application's responsibility to decide whether the received answer is
 *   an URC or not and if yes, then @ref at_postprocess_urc() can be called with
 *   the response as parameter.
 *
 * URC handling can be enabled by adding the `at_urc` module to the
 * application. This allows to @ref at_add_urc "register" and @ref at_remove_urc
 * "de-register" URC strings to check. Later, URCs can be processed in three
 * different ways:
 *  - automatically, whenever any at_* method that intercepts URCs is called.
 *    Such methods are marked in their docstring
 *  - manually, by calling at_process_urc() periodically
 *  - manually, by calling at_postprocess_urc() with an URC as parameter. The
 *    URC is assumed to have been obtained from the device through methods that
 *    do not automatically handle URCs (for example through @ref at_recv_bytes())
 * If a registered URC has been detected the correspondent @ref at_urc_t::cb
 * "callback function" is called.
 *
 * ## Error reporting ##
 * Most DCEs (Data Circuit-terminating Equipment, aka modem) can return extra
 * error information instead of the rather opaque "ERROR" message. They have the
 * form:
 *  - `+CMS ERROR: err_code>` for SMS-related commands
 *  - `+CME ERROR: <err_code>` for other commands
 *
 * For `+CME`, this behavior is usually off by default and can be toggled with:
 *  `AT+CMEE=<type>`
 * where `<type>` may be:
 *  - 0 disable extended error reporting, return `ERROR`
 *  - 1 enable extended error reporting, with `<err_code>` integer
 *  - 2 enable extended error reporting, with `<err_code>` as string
 * Check your DCE's manual for more information.
 *
 * Some of the API calls below support detailed error reporting. Whenever they
 * detect extended error responses, -AT_ERR_EXTENDED is returned and `<err_code>`
 * can be retrieved by calling @ref at_get_err_info().
 *
 * @{
 *
 * @file
 *
 * @brief       AT (Hayes) library interface
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

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
 * @brief convenience macro for the EOL sequence sent by the DCE
 */
#define AT_RECV_EOL AT_RECV_EOL_1 AT_RECV_EOL_2

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
/** @} */

#if defined(MODULE_AT_URC) || DOXYGEN
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

/** Error cause can be further investigated. */
#define AT_ERR_EXTENDED 200

/** Shortcut for getting send end of line length */
#define AT_SEND_EOL_LEN  (sizeof(CONFIG_AT_SEND_EOL) - 1)

/**
 * @brief AT device structure
 */
typedef struct {
    isrpipe_t isrpipe;      /**< isrpipe used for getting data from uart */
    uart_t uart;            /**< UART device where the AT device is attached */
    char *rp_buf;           /**< response parsing buffer */
    size_t rp_buf_size;     /**< response parsing buffer size */
#ifdef MODULE_AT_URC
    clist_node_t urc_list;  /**< list to keep track of all registered urc's */
#ifdef MODULE_AT_URC_ISR
    bool awaiting_response; /**< indicates if the driver waits for a response */
    event_t event;          /**< event posted from ISR to process urc's */
#endif
#endif
} at_dev_t;

/**
 * @brief AT device initialization parameters
*/
typedef struct {
    uart_t uart;            /**< UART device where the AT device is attached */
    uint32_t baudrate;      /**< UART device baudrate */
    char *rx_buf;           /**< UART rx buffer */
    size_t rx_buf_size;     /**< UART rx buffer size */
    /**
     * Response parsing buffer - used for classifying DCE responses and holding
     * detailed error information. Must be at least 16 bytes.
     * If you don't care about URCs (MODULE_AT_URC is undefined) this must only
     * be large enough to hold responses like `OK`, `ERROR` or `+CME ERROR: <err_code>`.
     * Otherwise adapt its size to the maximum length of the URCs you are expecting
     * and actually care about. */
    char *rp_buf;
    size_t rp_buf_size;     /**< response parsing buffer size */
} at_dev_init_t;

/**
 * @brief Get extended error information of the last command sent.
 *
 * If a previous at_* method returned with -AT_ERR_EXTENDED, you can retrieve
 * a pointer to the error string with this.
 *
 * @param[in] dev device to operate on
 *
 * @retval string containing the error value.
 */
static inline char const *at_get_err_info(at_dev_t const *dev)
{
    return dev->rp_buf;
}
/**
 * @brief   Initialize AT device struct
 *
 * @param[in]   dev         struct to initialize
 * @param[in]   init        init struct, may be destroyed after return
 *
 * @retval     success code UART_OK on success
 * @retval     error code UART_NODEV or UART_NOBAUD otherwise
 */
int at_dev_init(at_dev_t *dev, at_dev_init_t const *init);

/**
 * @brief   Simple command helper
 *
 * This function sends an AT command to the device and waits for "OK".
 *
 * URCs are automatically handled
 *
 * @param[in]   dev     device to operate on
 * @param[in]   command command string to send
 * @param[in]   timeout timeout (in usec)
 *
 * @retval      0 when device answers "OK"
 * @retval     -AT_ERR_EXTENDED if failed and a error code can be retrieved with
 *              @ref at_get_err_info() (i.e. DCE answered with `CMx ERROR`)
 * @retval     <0 other failures
 */
int at_send_cmd_wait_ok(at_dev_t *dev, const char *command, uint32_t timeout);

/**
 * @brief   Send AT command, wait for a prompt
 *
 * This function sends the supplied @p command, then waits for the prompt (>)
 * character and returns
 *
 * URCs are automatically handled
 *
 * @param[in]   dev         device to operate on
 * @param[in]   command     command string to send
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      0 when prompt is received
 * @retval     -AT_ERR_EXTENDED if failed and a error code can be retrieved with
 *              @ref at_get_err_info() (i.e. DCE answered with `CMx ERROR`)
 * @retval     <0 other failures
 */
int at_send_cmd_wait_prompt(at_dev_t *dev, const char *command, uint32_t timeout);

/**
 * Waits for the prompt character (>).
 *
 * @param[in]   dev         device to operate on
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      0 when prompt is received
 * @retval     -AT_ERR_EXTENDED if failed and a error code can be retrieved with
 *              @ref at_get_err_info() (i.e. DCE answered with `CMx ERROR`)
 * @retval     <0 other failures
 */
int at_wait_prompt(at_dev_t *dev, uint32_t timeout);
/**
 * @brief   Send AT command, wait for response
 *
 * This function sends the supplied @p command, then waits and returns one
 * line of response. The response is guaranteed null-terminated.
 *
 * Some URCs are automatically handled. The response returned can be an
 * URC. In that case, @ref at_postprocess_urc() can be called with the response
 * as parameter.
 *
 * A possible empty line will be skipped.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   command     command to send
 * @param[out]  resp_buf    buffer for storing response
 * @param[in]   len         len of @p buffer
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      n length of response on success
 * @retval     -ENOBUFS if the supplied buffer was to small.
 * @retval     <0 on error
 */
ssize_t at_send_cmd_get_resp(at_dev_t *dev, const char *command, char *resp_buf,
                             size_t len, uint32_t timeout);
/**
 * @brief Wait for a response with a specific prefix.
 *
 * If the provided prefix is NULL or empty, this behaves just like
 * @ref at_readline_skip_empty(). Otherwise, it repeatedly calls @ref
 * at_readline_skip_empty() and:
 *  - if the prefix matches: discards the prefix from the response, copies the
 *    rest to @p resp_buf, stops and returns the resulting response length
 *  - if the prefix does not match:
 *    - if the response is OK: stops and returns 0
 *    - if the response is ERROR/CMx ERROR: stops and returns <0
 *    - none of the above: handles response as URC and repeats with the next
 *      line
 *
 * @param[in]   dev         device to operate on
 * @param[in]   resp_prefix expected prefix in the response
 * @param[out]  resp_buf    buffer for storing response
 * @param[in]   len         len of @p buffer
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      n response length if the prefix of the response matches
 * @retval      0 if the response was OK
 * @retval     -AT_ERR_EXTENDED if failed and a error code can be retrieved with
 *              @ref at_get_err_info() (i.e. DCE answered with `CMx ERROR`)
 *  @retval    <0 on error
 */
ssize_t at_get_resp_with_prefix(at_dev_t *dev, const char *resp_prefix,
                                char *resp_buf, size_t len, uint32_t timeout);
/**
 * @brief   Send AT command, wait for response plus OK
 *
 * This function sends the supplied @p command, then waits and returns one
 * line of response. The response is guaranteed null-terminated.
 *
 * Calls following in order:
 *  - @ref at_send_cmd()
 *  - @ref at_get_resp_with_prefix()
 *  - @ref at_wait_ok()
 *
 * URCs are automatically handled. If no prefix is provided, the response
 * may be an URC. In that case, @ref at_postprocess_urc() can be called with the
 * response as parameter.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   command     command to send
 * @param[in]   resp_prefix expected prefix in the response
 * @param[out]  resp_buf    buffer for storing response
 * @param[in]   len         len of @p buffer
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      n length of response on success
 * @retval     -AT_ERR_EXTENDED if failed and a error code can be retrieved with
 *              @ref at_get_err_info() (i.e. DCE answered with `CMx ERROR`)
 * @retval     -ENOBUFS if the supplied buffer was to small.
 * @retval     <0 other failures
 */
ssize_t at_send_cmd_get_resp_wait_ok(at_dev_t *dev, const char *command, const char *resp_prefix,
                                     char *resp_buf, size_t len, uint32_t timeout);

/**
 * @brief   Send AT command, wait for multiline response
 *
 * This function sends the supplied @p command, then returns all response
 * lines until the device sends "OK". The response is guaranteed null-terminated.
 *
 * Some URCs are automatically handled. The first m response lines can be
 * URCs. In that case, @ref at_postprocess_urc() can be called with each line
 * as parameter.
 *
 * If a line contains a DTE error response, this function stops and returns
 * accordingly. Any lines received prior to that are considered to be URCs and
 * thus handled.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   command     command to send
 * @param[out]  resp_buf    buffer for storing response
 * @param[in]   len         len of @p resp_buf
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      n length of response on success
 * @retval     -AT_ERR_EXTENDED if failed and a error code can be retrieved with
 *              @ref at_get_err_info() (i.e. DCE answered with `CMx ERROR`)
 * @retval     -ENOBUFS if the supplied buffer was to small.
 * @retval     <0 other failures
 */
ssize_t at_send_cmd_get_lines(at_dev_t *dev, const char *command, char *resp_buf,
                              size_t len, uint32_t timeout);

/**
 * @brief   Expect bytes from device
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer containing bytes to expect (NULL-terminated)
 * @param[in]   timeout timeout (in usec)
 *
 * @retval      0 on success
 * @retval     <0 otherwise
 */
int at_expect_bytes(at_dev_t *dev, const char *bytes, uint32_t timeout);

/**
 * @brief   Repeatedly calls at_expect_bytes() until a match or timeout occurs
 *
 * @param[in]   dev     device to operate on
 * @param[in]   bytes   buffer containing bytes to expect (NULL-terminated)
 * @param[in]   timeout timeout (in usec)
 *
 * @retval      0 on success
 * @retval     <0 otherwise
 */
int at_wait_bytes(at_dev_t *dev, const char *bytes, uint32_t timeout);

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
 * @retval                      0 on success
 * @retval                     <0 on error
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
 * @retval      n Number of bytes read, eventually zero if no bytes available
 */
ssize_t at_recv_bytes(at_dev_t *dev, char *bytes, size_t len, uint32_t timeout);

/**
 * @brief   Send command to device
 *
 * Some URCs may be handled.
 *
 * @param[in]   dev     device to operate on
 * @param[in]   command command to send
 * @param[in]   timeout timeout (in usec)
 *
 * @retval      0 on success
 * @retval     <0 otherwise
 */
int at_send_cmd(at_dev_t *dev, const char *command, uint32_t timeout);

/**
 * @brief   Parse a response from the device.
 *
 * This is always called automatically in functions that may return -AT_ERR_EXTENDED.
 * However, if you read the response by other methods (e.g. with @ref at_recv_bytes()),
 * you might want to call this on the response so that you don't have to parse it yourself.
 *
 * @retval  0 if the response is "OK"
 * @retval -AT_ERR_EXTENDED if the response is `+CMx ERROR: <err>`, and `<err>`
 *          has been successfully copied to @p dev->rp_buf
 * @retval -1 if the response is "ERROR", or `+CMx ERROR: <err>` but `<err>`
 *          could not be copied
 * @retval  1 otherwise
 */
int at_parse_resp(at_dev_t *dev, char const *resp);

/**
 * @brief   Read a line from device
 *
 * Stops at the first DCE EOL sequence. The response is guaranteed null-terminated.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   resp_buf    buffer to store line
 * @param[in]   len         size of @p resp_buf
 * @param[in]   keep_eol    true to keep the trailing EOL sequence in the response
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      n line length on success
 * @retval     -ENOBUFS if the supplied buffer was to small.
 * @retval     <0 on error
 */
ssize_t at_readline(at_dev_t *dev, char *resp_buf, size_t len, bool keep_eol, uint32_t timeout);

/**
 * @brief   Read a line from device, skipping a possibly empty line.
 *
 * Stops at the first DCE EOL sequence AFTER any non-EOL sequence. The response
 * is guaranteed null-terminated.
 *
 * @param[in]   dev         device to operate on
 * @param[in]   resp_buf    buffer to store line
 * @param[in]   len         size of @p resp_buf
 * @param[in]   keep_eol    true to keep the trailing EOL sequence in the response
 * @param[in]   timeout     timeout (in usec)
 *
 * @retval      n line length on success
 * @retval     -ENOBUFS if the supplied buffer was to small.
 * @retval     <0 on error
 */
ssize_t at_readline_skip_empty(at_dev_t *dev, char *resp_buf, size_t len,
                               bool keep_eol, uint32_t timeout);

/**
 * @brief Wait for an OK response.
 *
 * Useful when crafting the command-response sequence by yourself.
 *
 * URCs are automatically handled
 *
 * @param[in]   dev     device to operate on
 * @param[in]   timeout timeout (in usec)
 *
 * @retval      0 when device answers "OK"
 * @retval     -AT_ERR_EXTENDED if failed and a error code can be retrieved with
 *              @ref at_get_err_info() (i.e. DCE answered with `CMx ERROR`)
 * @retval     <0 other failures
 */
int at_wait_ok(at_dev_t *dev, uint32_t timeout);

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

/**
 * @brief   Process one URC from the provided buffer
 *
 * Useful if you e.g. called @ref at_send_cmd_get_lines() and the first lines
 * are URCs.
 *
 * @param[in]   dev device to operate on
 * @param[in]   buf buffer containing an URC
 */
void at_postprocess_urc(at_dev_t *dev, char *buf);
/**
 * @brief   Process all URCs from the provided buffer
 *
 * Useful if you e.g. called @ref at_recv_bytes(), found what you were interested
 * in, and there might be some URCs left in the buffer.
 *
 * @param[in]   dev device to operate on
 * @param[in]   buf buffer containing URCs
 */
void at_postprocess_urc_all(at_dev_t *dev, char *buf);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
