/*
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup    sys_can_conn
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
#include "can/isotp.h"
#include "mbox.h"

#if defined(MODULE_CONN_CAN_ISOTP_MULTI) || defined(DOXYGEN)
#include "mutex.h"

#ifndef CONN_CAN_ISOTP_MBOX_SIZE
/**
 * @brief Mailbox size of a conn_can_isotp_t
 */
#define CONN_CAN_ISOTP_MBOX_SIZE (16)
#endif

/**
 * @brief ISO-TP connection
 *
 * When conn_can_isotp_multi module is used, this is a 'master' connection
 * which can be used to send and receive with multiple connections within
 * a single thread.
 *
 * If conn_can_isotp_multi is not used, this is a simple ISO-TP connection
 */
typedef struct conn_can_isotp_master conn_can_isotp_t;

/**
 * @brief ISO-TP salve connection
 *
 * This is a slave connection which exists only when conn_can_isotp_multi
 * module is used.
 */
typedef struct conn_can_isotp_slave {
    struct conn_can_isotp_slave *next;     /**< Next slave in the list */
    struct conn_can_isotp_master *master;  /**< Master connection holding the mailbox */
    struct isotp isotp;                    /**< ISO-TP parameters and status */
    int ifnum;                             /**< interface number */
    int bound;                             /**< 1 if connection is bound */
    can_rx_data_t *rx;                     /**< Buffered rx data */
} conn_can_isotp_slave_t;

/**
 * @brief ISO-TP master connection
 */
struct conn_can_isotp_master {
    /* slave fields */
    struct conn_can_isotp_slave *next;     /**< First slave in the list */
    struct conn_can_isotp_master *master;  /**< Master connection */
    struct isotp isotp;                    /**< ISO-TP parameters and status */
    int ifnum;                             /**< interface number */
    int bound;                             /**< 1 if connection is bound */
    can_rx_data_t *rx;                     /**< Buffered rx data */
    /* slave fields end */
    mutex_t lock;                          /**< Master lock */
    mbox_t mbox;                           /**< mailbox for the connection list */
    /** Connection list message queue */
    msg_t mbox_queue[CONN_CAN_ISOTP_MBOX_SIZE];
};

/**
 * @brief Initialize a slave connection
 *
 * This initializes a slave connection.
 *
 * This must be called on slave connections when conn_can_isotp_multi is used.
 * Does not exist otherwise.
 *
 * @param[in]       master      the master connection
 * @param[in,out]   slave       the slave connection to initialize
 */
static inline void conn_can_isotp_init_slave(conn_can_isotp_t *master, conn_can_isotp_slave_t *slave)
{
    slave->next = NULL;
    slave->master = master;
    slave->rx = NULL;
}
#else

#ifndef CONN_CAN_ISOTP_MBOX_SIZE
/**
 * @brief Mailbox size of a conn_can_isotp_t
 */
#define CONN_CAN_ISOTP_MBOX_SIZE (16)
#endif

/**
 * @brief   ISOTP connection
 */
typedef struct conn_can_isotp {
    struct isotp isotp; /**< ISO-TP connection */
    int ifnum;          /**< interface number */
    int bound;          /**< 1 if connection is bound */
    mbox_t mbox;        /**< mbox */
    /** message queue */
    msg_t mbox_queue[CONN_CAN_ISOTP_MBOX_SIZE];
} conn_can_isotp_t;
#endif /* MODULE_CONN_CAN_ISOTP_MULTI */

/**
 * @brief  Create can isotp connection socket
 *
 * @param[in,out] conn      ISO-TP connection
 * @param[in]     options   ISO-TP options
 * @param[in]     ifnum     can device Interface
 *
 * @return 0 if socket was successfully connected
 * @return any other negative number in case of an error
 */
int conn_can_isotp_create(conn_can_isotp_t *conn, struct isotp_options *options, int ifnum);

/**
 * @brief Bind a can isotp connection
 *
 * @param[in,out] conn          ISO-TP connection
 * @param[in]     fc_options    ISO-TP flow control options, can be NULL for default parameters
 *
 * @retval        0             on success
 * @retval        !=0           in case of an error
 */
int conn_can_isotp_bind(conn_can_isotp_t *conn, struct isotp_fc_options *fc_options);

/**
 * @brief  Close can isotp connection socket
 *
 * @param[in] conn          ISO-TP connection
 *
 * @return 0 if conn is closed correctly
 * @return any other negative number in case of an error
 */
int conn_can_isotp_close(conn_can_isotp_t *conn);

/**
 *  @brief  Receive isotp data
 *
 * @param[in] conn          ISO-TP connection
 * @param[out] buf          buf to fill in with received data
 * @param[in] size          size of the buffer in bytes
 * @param[in] timeout       timeout in us, 0 for infinite
 *
 * @return the number of bytes received
 * @return any other negative number in case of an error
 */
int conn_can_isotp_recv(conn_can_isotp_t *conn, void *buf, size_t size, uint32_t timeout);

/**
 * @brief  Generic can send
 *
 * @param[in] conn          ISO-TP connection
 * @param[in] buf           data to send
 * @param[in] size          size of the buffer in bytes
 * @param[in] flags         make function blocked or not
 *                          (CAN_ISOTP_TX_DONT_WAIT to ignore tx confirmation)
 *
 * @return the number of bytes sent
 * @return any other negative number in case of an error
 */
int conn_can_isotp_send(conn_can_isotp_t *conn, const void *buf, size_t size, int flags);

#if defined(MODULE_CONN_CAN_ISOTP_MULTI) || defined(DOXYGEN)
/**
 * @brief Wait for reception from multiple connections
 *
 * @param[out] conn        ISO-TP connection which received data
 * @param[in] master       the master connection
 * @param[in] timeout      timeout in us, 0 for infinite wait
 *
 * @return 0 if OK, < 0 if error
 */
int conn_can_isotp_select(conn_can_isotp_slave_t **conn, conn_can_isotp_t *master, uint32_t timeout);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
