/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_can_conn Connection
 * @ingroup     sys_can
 * @brief       conn interface for CAN stack
 *
 * This is the user interface to send and receive raw CAN frames or ISO-TP datagrams
 *
 * @{
 *
 * @file
 * @brief       Definitions of generic CAN interface
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

#include "can/can.h"
#include "can/raw.h"
#include "mbox.h"

#ifndef CONN_CAN_RAW_MBOX_SIZE
/**
 * @brief Mailbox size of a conn_can_raw_t
 */
#define CONN_CAN_RAW_MBOX_SIZE (16)
#endif

/**
 * @name flags values
 * @{
 */
#define CONN_CAN_DONTWAIT     (1)     /**< Do not wait for Tx confirmation when sending */
#define CONN_CAN_RECVONLY     (2)     /**< Do not send anything on the bus */
/** @} */

/**
 * @brief   RAW CAN connection
 */
typedef struct conn_can_raw {
    int ifnum;                 /**< Interface number of the can device */
    int flags;                 /**< Config flags for that conn object */
    size_t count;              /**< number of filters set */
    struct can_filter *filter; /**< list of filter */
    mbox_t mbox;               /**< mbox */
    /**
     * message queue
     */
    msg_t mbox_queue[CONN_CAN_RAW_MBOX_SIZE];
} conn_can_raw_t;

/**
 * @brief  Create can connection socket
 *
 * @param[in,out]   conn    CAN connection
 * @param[in]       filter  list of filters to set
 * @param[in]       count   number of filters in @p filter
 * @param[in]       ifnum   can device Interface
 * @param[in]       flags   conn flags to set (CONN_CAN_RECVONLY)
 *
 * @post   @p filter must remain allocated until @p conn is closed
 *
 * @return 0 if socket was successfully connected
 * @return any other negative number in case of an error
 */
int conn_can_raw_create(conn_can_raw_t *conn, struct can_filter *filter, size_t count,
                        int ifnum, int flags);

/**
 * @brief  Close can connection socket
 *
 * @param[in] conn          CAN connection
 *
 * @return 0 if conn is closed correctly
 * @return any other negative number in case of an error.
 */
int conn_can_raw_close(conn_can_raw_t *conn);

/**
 *  @brief  Generic can receive
 *
 * @param[in] conn          CAN connection
 * @param[out] frame        CAN frame to receive
 * @param[in] timeout       timeout in us, 0 for infinite
 *
 * @return the number of bytes received
 * @return any other negative number in case of an error
 */
int conn_can_raw_recv(conn_can_raw_t *conn, can_frame_t *frame, uint32_t timeout);

/**
 * @brief  Generic can send
 *
 * @param[in] conn          CAN connection
 * @param[in] frame         frame to send
 * @param[in] flags         make function blocked or not
 *                          (CONN_CAN_DONTWAIT to ignore tx confirmation)
 *
 * @return the number of bytes sent
 * @return any other negative number in case of an error
 */
int conn_can_raw_send(conn_can_raw_t *conn, const can_frame_t *frame, int flags);

/**
 * @brief  Set raw CAN filters
 *
 * If filters were already set for this connection, it first unsets the previous filters
 * and sets the new ones.
 *
 * @param[in] conn          CAN connection
 * @param[in] filter        list of filters to set
 * @param[in] count         number of filters in @p filter
 *
 * @pre    previously set filters must be allocated until the end of the call
 * @post   @p filter must remain allocated until @p conn is closed or
 * conn_can_raw_set_filter() is called
 *
 * @return 0 if can filters were successfully set
 * @return any other negative number in case of an error
 */
int conn_can_raw_set_filter(conn_can_raw_t *conn, struct can_filter *filter, size_t count);

#ifdef __cplusplus
}
#endif

/** @} */
