/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    mbox    Message box
 * @ingroup     sys
 * @brief       Allows sending of @ref msg_t without a specific target thread
 * @{
 *
 * @file
 * @brief   Message box definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef MBOX_H_
#define MBOX_H_

#include <stdbool.h>

#include "cib.h"
#include "msg.h"
#include "mutex.h"
#include "sema.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Message box type
 */
typedef struct {
    msg_t *msgs;            /**< memory holding messages */
    cib_t msgs_queue;       /**< queue of the message box */
    mutex_t mutex;          /**< mutex for the message box */
    sema_t not_empty;       /**< semaphore to signal that message box is not empty anymore */
    sema_t not_full;        /**< semaphore to signal that message box is not full anymore */

    /**
     * @brief counter for messages waiting to be added to the message box (if full)
     */
    volatile int waiting;
} mbox_t;

/**
 * @brief   Static initializer for message box.
 *
 * @pre (@p array != NULL) && (@p num must be power of two) && (@p num <= MAX_INT/2)
 *
 * @param[in] array Pointer to preallocated array of @ref msg_t structures.
 * @param[in] num   Number of @p msg_t structures in @p array.
 *
 * @return  Statically initialized message box
 */
#define MBOX_CREATE(array, num) { array, CIB_INIT(num), MUTEX_INIT, \
                                  SEMA_CREATE(1), SEMA_CREATE(0), 0 }

/**
 * @brief   Static initializer for message box.
 *
 * @pre (@pre mbox != NULL) && (@p array != NULL) &&
 *      (@p num must be power of two) && (@p num <= (INT_MAX / 2))
 *
 * @param[out] mbox The initialized message box.
 * @param[in] array Pointer to preallocated array of @ref msg_t structures.
 * @param[in] num   Number of @p msg_t structures in @p array.
 */
void mbox_create(mbox_t *mbox, msg_t *array, int num);

/**
 * @brief   Destroys a message box
 *
 * @pre (mbox != NULL)
 *
 * It empties the semaphore so waiting threads get signalized about the message box's destruction.
 * A destroyed message box must not be used.
 *
 * @param[in] mbox Message box to destroy.
 */
void mbox_destroy(mbox_t *mbox);

/**
 * @brief   Checks (not thread-safe), if a message box is empty
 *
 * @param[in] mbox  A message box.
 *
 * @return  true, if message box is empty
 * @return  false, if message box is not empty
 */
static inline bool mbox_is_empty(const mbox_t *mbox)
{
    /* cib_avail() does not change cib */
    return (cib_avail((cib_t *)&mbox->msgs_queue) == 0);
}

/**
 * @brief   Checks (not thread-safe), if a message box is full
 *
 * @param[in] mbox  A message box.
 *
 * @return  true, if message box is full
 * @return  false, if message box is not full
 */
static inline bool mbox_is_full(const mbox_t *mbox)
{
    /* cib_avail() does not change cib */
    return (cib_avail((cib_t *)&mbox->msgs_queue) == (mbox->msgs_queue.mask + 1));
}

/**
 * @brief   Put message into message box blocking.
 *
 * @pre (mbox != NULL) && (msg != NULL)
 *
 * This function will always succeed in putting the message into the message box (unless there is
 * an error as reported through the return value). If the message box is full it will block.
 *
 * @param[in] mbox  The message box to put the message into.
 * @param[in] msg   A message.
 *
 * @return  0, on success.
 * @return  -ECANCELED, when @p mbox was destroyed during execution.
 * @return  -EOVERFLOW, if one of the mbox::not_empty semaphores would overflow.
 */
int mbox_put(mbox_t *mbox, msg_t *msg);

/**
 * @brief   Put message into message box non-blocking.
 *
 * @pre (mbox != NULL) && (msg != NULL)
 *
 * This function will not block if @p mbox is full, but signal this fact through its return value.
 *
 * @param[in] mbox  The message box to put the message into.
 * @param[in] msg   A message.
 *
 * @return  0, on success.
 * @return  -EAGAIN, if @p mbox was full.
 * @return  -EOVERFLOW, if one of the mbox::not_empty would overflow.
 */
int mbox_tryput(mbox_t *mbox, msg_t *msg);

/**
 * @brief   Get message from message box blocking with timeout.
 *
 * @pre (mbox != NULL) && (msg != NULL)
 *
 * This function gets a message from the message box. If the message box is empty it will block
 * for @b timeout microseconds.
 *
 * @param[in] mbox      The message box to get the message from
 * @param[out] msg      The message received.
 * @param[in] timeout   Time in microseconds until getting of message times out.
 *
 * @return  0, on success.
 * @return  -ECANCELED, when @p mbox was destroyed during execution.
 * @return  -EOVERFLOW, if one of the mbox::not_full semaphores would overflow.
 * @return  -ETIMEDOUT, when getting of message timed out.
 */
int mbox_get_timed(mbox_t *mbox, msg_t *msg, uint64_t timeout);

/**
 * @brief   Get message from message box blocking with timeout.
 *
 * @pre (mbox != NULL) && (msg != NULL)
 *
 * This function gets a message from the message box. If the message box is empty it will block.
 *
 * @param[in] mbox      The message box to get the message from
 * @param[out] msg      The message received.
 *
 * @return  0, on success.
 * @return  -ECANCELED, when @p mbox was destroyed during execution.
 * @return  -EOVERFLOW, if one of the mbox::not_full semaphores would overflow.
 */
static inline int mbox_get(mbox_t *mbox, msg_t *msg)
{
    return mbox_get_timed(mbox, msg, 0);
}

/**
 * @brief   Get message from message box non-blocking.
 *
 * @pre (mbox != NULL) && (msg != NULL)
 *
 * This function will not block if @p mbox is empty, but signal this fact through its return value.
 *
 * @param[in] mbox      The message box to get the message from
 * @param[out] msg      The message received.
 *
 * @return  0, on success.
 * @return  -EAGAIN, if @p mbox was empty.
 * @return  -ECANCELED, when @p mbox was destroyed during execution.
 * @return  -EOVERFLOW, if one of the mbox::not_full semaphores would overflow.
 */
int mbox_tryget(mbox_t *mbox, msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* MBOX_H_ */
/** @} */
