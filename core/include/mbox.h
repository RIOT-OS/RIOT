/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_mbox Mailboxes
 * @ingroup     core
 * @brief       Mailbox implementation
 *
 * @{
 *
 * @file
 * @brief       Mailbox API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef MBOX_H
#define MBOX_H

#include "list.h"
#include "cib.h"
#include "msg.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Static initializer for mbox objects */
#define MBOX_INIT(queue, queue_size) { \
        { 0 }, { 0 }, CIB_INIT(queue_size), queue \
}

/**
 * @brief Mailbox struct definition
 */
typedef struct {
    list_node_t readers;    /**< list of threads waiting for message    */
    list_node_t writers;    /**< list of threads waiting to send        */
    cib_t cib;              /**< cib for msg array                      */
    msg_t *msg_array;       /**< ptr to array of msg queue              */
} mbox_t;

enum {
    NON_BLOCKING = 0,       /**< non-blocking mode */
    BLOCKING,               /**< blocking mode */
};

/**
 * @brief Initialize mbox object
 *
 * @note The message queue size must be a power of two!
 *
 * @param[in]   mbox        ptr to mailbox to initialize
 * @param[in]   queue       array of msg_t used as queue
 * @param[in]   queue_size  number of msg_t objects in queue
 */
static inline void mbox_init(mbox_t *mbox, msg_t *queue,
                             unsigned int queue_size)
{
    mbox_t m = MBOX_INIT(queue, queue_size);

    *mbox = m;
}

/**
 * @brief Add message to mailbox
 *
 * If the mailbox is full, this function will return right away.
 *
 * @internal
 *
 * @param[in] mbox      ptr to mailbox to operate on
 * @param[in] msg       ptr to message that will be copied into mailbox
 * @param[in] blocking  block if 1, don't block if 0
 *
 * @return  1   if msg could be delivered
 * @return  0   otherwise
 */
int _mbox_put(mbox_t *mbox, msg_t *msg, int blocking);

/**
 * @brief Get message from mailbox
 *
 * If the mailbox is empty, this function will return right away.
 *
 * @internal
 *
 * @param[in] mbox  ptr to mailbox to operate on
 * @param[in] msg   ptr to storage for retrieved message
 * @param[in] blocking  block if 1, don't block if 0
 *
 * @return  1   if msg could be retrieved
 * @return  0   otherwise
 */
int _mbox_get(mbox_t *mbox, msg_t *msg, int blocking);

/**
 * @brief Add message to mailbox
 *
 * If the mailbox is full, this function will block until space becomes
 * available.
 *
 * @param[in] mbox  ptr to mailbox to operate on
 * @param[in] msg   ptr to message that will be copied into mailbox
 */
static inline void mbox_put(mbox_t *mbox, msg_t *msg)
{
    assume(irq_is_in() || irq_is_enabled());
    _mbox_put(mbox, msg, BLOCKING);
}

/**
 * @brief Add message to mailbox
 *
 * If the mailbox is full, this function will return right away.
 *
 * @param[in] mbox  ptr to mailbox to operate on
 * @param[in] msg   ptr to message that will be copied into mailbox
 *
 * @return  1   if msg could be delivered
 * @return  0   otherwise
 */
static inline int mbox_try_put(mbox_t *mbox, msg_t *msg)
{
    return _mbox_put(mbox, msg, NON_BLOCKING);
}

/**
 * @brief Get message from mailbox
 *
 * If the mailbox is empty, this function will block until a message becomes
 * available.
 *
 * @param[in] mbox  ptr to mailbox to operate on
 * @param[in] msg   ptr to storage for retrieved message
 */
static inline void mbox_get(mbox_t *mbox, msg_t *msg)
{
    assume(irq_is_in() || irq_is_enabled());
    _mbox_get(mbox, msg, BLOCKING);
}

/**
 * @brief Get message from mailbox
 *
 * If the mailbox is empty, this function will return right away.
 *
 * @param[in] mbox  ptr to mailbox to operate on
 * @param[in] msg   ptr to storage for retrieved message
 *
 * @return  1   if msg could be retrieved
 * @return  0   otherwise
 */
static inline int mbox_try_get(mbox_t *mbox, msg_t *msg)
{
    return _mbox_get(mbox, msg, NON_BLOCKING);
}

/**
 * @brief Get mbox queue size (capacity)
 *
 * @param[in] mbox  ptr to mailbox to operate on
 *
 * @return  size of mbox queue (or 0 if there's no queue)
 */
static inline size_t mbox_size(mbox_t *mbox)
{
    return mbox->cib.mask ? mbox->cib.mask + 1 : 0;
}

/**
 * @brief Get messages available in mbox
 *
 * Returns the number of messages that can be retrieved without blocking.
 *
 * @param[in] mbox  ptr to mailbox to operate on
 *
 * @return  number of available messages
 */
static inline size_t mbox_avail(mbox_t *mbox)
{
    return cib_avail(&mbox->cib);
}

/**
 * @brief   Unset's the mbox, effectively deinitializing and invalidating it.
 *
 * @param[in] mbox  ptr to mailbox to operate on
 */
static inline void mbox_unset(mbox_t *mbox)
{
    mbox->msg_array = NULL;
    mbox->cib.mask = 0;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MBOX_H */
