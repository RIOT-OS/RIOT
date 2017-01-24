/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_tftp  TFTP Support Library
 * @ingroup     net_gnrc
 * @brief       Add's support for TFTP protocol parsing
 * @{
 *
 * @file
 * @brief       TFTP support library
 *
 * The TFTP module add's support for the TFTP protocol.
 * It implements the following RFC's:
 *  - https://tools.ietf.org/html/rfc1350
 *     (RFC1350 The TFTP Protocol (Revision 2)
 *
 *  - https://tools.ietf.org/html/rfc2347
 *     (RFC2347 TFTP Option Extension)
 *
 *  - https://tools.ietf.org/html/rfc2348
 *     (RFC2348 TFTP Blocksize Option)
 *
 *  - https://tools.ietf.org/html/rfc2349
 *     (RFC2349 TFTP Timeout Interval and Transfer Size Options)
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef GNRC_TFTP_H
#define GNRC_TFTP_H

#include <inttypes.h>

#include "byteorder.h"
#include "kernel_types.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/nettype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The maximum allowed length of the transfer filename
 */
#ifndef GNRC_TFTP_MAX_FILENAME_LEN
#define GNRC_TFTP_MAX_FILENAME_LEN          (64)
#endif

/**
 * @brief The base source port to be used by TFTP
 */
#ifndef GNRC_TFTP_DEFAULT_SRC_PORT
#define GNRC_TFTP_DEFAULT_SRC_PORT          (10690)
#endif

/**
 * @brief The default destination port of the TFTP server
 */
#ifndef GNRC_TFTP_DEFAULT_DST_PORT
#define GNRC_TFTP_DEFAULT_DST_PORT          (69)
#endif

/**
 * @brief The maximum allowed data bytes in the data packet
 */
#ifndef GNRC_TFTP_MAX_TRANSFER_UNIT
#define GNRC_TFTP_MAX_TRANSFER_UNIT         (512)
#endif

/**
 * @brief The number of retries that must be made before stopping a transfer
 */
#ifndef GNRC_TFTP_MAX_RETRIES
#define GNRC_TFTP_MAX_RETRIES               (5)
#endif

/**
 * @brief The default timeout of a data packet
 */
#ifndef GNRC_TFTP_DEFAULT_TIMEOUT
#define GNRC_TFTP_DEFAULT_TIMEOUT           (1 * US_PER_SEC)
#endif

/**
 * @brief TFTP action to perform
 */
typedef enum {
    TFTP_READ,
    TFTP_WRITE
} tftp_action_t;

/**
 * @brief TFTP Transfer modes
 */
typedef enum {
    TTM_ASCII,
    TTM_OCTET,
    TTM_MAIL
} tftp_mode_t;

/**
 * @brief TFTP stop / finish events
 */
typedef enum {
    TFTP_SUCCESS,           /**< The transfer was successful */
    TFTP_PEER_ERROR,        /**< The peer send the given error */
    TFTP_INTERN_ERROR       /**< There was an internal error */
} tftp_event_t;

/**
 * @brief   callback define which is called when a new server request is placed
 *          or when an client read request is made and the data length option is received
 *
 * @param [in] action       The action the transfer want to perform
 * @param [in] mode         The data mode of the transfer
 * @param [in] file_name    The filename of the file being transfered
 * @param [in/out] data_len When a read action is performed, the application must give
 *                          the total transfer size of the data. When a write action
 *                          is performed the total transfer size will be given.
 */
typedef bool (*tftp_start_cb_t)(tftp_action_t action, tftp_mode_t mode,
                                const char *file_name, size_t *data_len);

/**
 * @brief   callback define which is called to get or set data from/to the user application
 */
typedef int (*tftp_data_cb_t)(uint32_t offset, void *data, size_t data_len);

/**
 * @brief   callback define which is called when an transfer is stopped
 */
typedef void (*tftp_stop_cb_t)(tftp_event_t event, const char *msg);

/**
 * @brief Start the TFTP server
 *
 * @param [in] data_cb      called for each read data block
 * @param [in] start_cb     called if a new client connection is requested
 * @param [in] stop_cb      called if the transfer has finished
 * @param [in] use_options  when set the client uses the option extensions
 *
 * @return 1 on success
 * @return -1 on failure
 */
int gnrc_tftp_server(tftp_data_cb_t data_cb, tftp_start_cb_t start_cb, tftp_stop_cb_t stop_cb, bool use_options);

/**
 * @brief Stop the TFTP server
 *
 * @return 1 on success
 * @return -1 on failure
 */
int gnrc_tftp_server_stop(void);

/**
 * @brief Start an TFTP client read action from the given destination
 *
 * @param [in] addr         the address of the server
 * @param [in] file_name    the filename of the file to get
 * @param [in] mode         the transfer mode
 * @param [in] data_cb      called for each read data block
 * @param [in] start_cb     called if the server returns the transfer_size option
 * @param [in] stop_cb      called if the transfer has finished
 * @param [in] use_option   when set the client uses the option extensions
 *
 * @return 1 on success
 * @return -1 on failure
 */
int gnrc_tftp_client_read(ipv6_addr_t *addr, const char *file_name, tftp_mode_t mode,
                          tftp_data_cb_t data_cb, tftp_start_cb_t start_cb, tftp_stop_cb_t stop_cb,
                          bool use_option);

/**
 * @brief Start an TFTP client write action to the given destination
 *
 * @param [in] addr         the address of the server
 * @param [in] file_name    the filename of the file to write
 * @param [in] mode         the transfer mode
 * @param [in] data_cb      called to store the received block
 * @param [in] total_size   the total size of the transfer
 * @param [in] stop_cb      called if the server returns the transfer_size option
 * @param [in] use_option   when set the client uses the option extensions
 *
 * @return 1 on success
 * @return -1 on failure
 */
int gnrc_tftp_client_write(ipv6_addr_t *addr, const char *file_name, tftp_mode_t mode,
                           tftp_data_cb_t data_cb, size_t total_size, tftp_stop_cb_t stop_cb,
                           bool use_option);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TFTP_H */

/**
 * @}
 */
