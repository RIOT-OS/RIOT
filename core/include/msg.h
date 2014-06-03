/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Kaspar Schleiser
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    core_msg  Messaging / IPC
 * @ingroup     core
 * @brief       Messaging API for inter process communication
 *
 * There are two ways to use the IPC Messaging system of RIOT. The default is synchronous
 * messaging. In this manner, messages are either dropped when the receiver is not waiting and the
 * message was sent non-blocking, or will be delivered immediately when the receiver calls
 * msg_receive(msg_pulse_t* m). To use asynchronous messaging any thread can create its own queue by
 * calling msg_init_queue(msg_pulse_t* array, int num). Messages sent to a thread with a non full message
 * queue are never dropped and the sending never blocks. Threads with a full message queue behaves
 * like in synchronous mode.
 *
 * @{
 *
 * @file        msg.h
 * @brief       Messaging API for inter process communication
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef __MSG_H
#define __MSG_H

#include <stdint.h>
#include <stdbool.h>
#include "kernel_types.h"
#include "queue.h"

#define MESSAGE_PROCESS_NOT_WAITING 0
#define MESSAGE_SENT                1
#define MESSAGE_PROCESS_UNKNOWN     2

/**
 * @name Bitfield values that can be set / found in msg_hdr_t->flags
 * @{
 */
#define MSG_REPLY_EXPECTED       (1)    /**< The sending thread is REPLY_BLOCKED
                                             and expects a reply */
#define MSG_IS_PULSE             (2)    /**< The received msg is of type msg_pulse_t */
#define MSG_BLOCK                (4)    /**< The message was sent in blocking mode */
/** @} */

/**
 * @brief Describes a message node object
 *
 * These messages nodes are used as messaging endpoints
 * by the messaging API.
 *
 */
typedef struct msg_node {
    struct tcb   *owner;         /**< thread that owns this message node   */
    void         *wait_data;     /**< holding current receive data         */
    queue_node_t msg_waiters;    /**< threads waiting to send to this node */

#ifdef MODULE_MSG_QUEUE
    struct msg_queue *msg_queue; /**< ptr to this node's msg queue         */
#endif
} msg_node_t;

/**
 * @brief Describes a message header object that contains auxillary message data
 *
 * Pass a pointer to msg_hdr_t to msg_receive to get extra message info.
 *
 * This is also used internally my RIOT's msg subsystem.
 */
typedef struct msg_hdr {
    struct msg_node *node;         /**< Reference to sender message node */
    char            *payload;      /**< Pointer to message send buffer (same as supplied to
                                        msg_send()) */
    uint16_t        size;          /**< Receive buffer size. */
    uint16_t        flags;         /**< Bitfield to hold several flags */
    char            *rbuf;         /**< Pointer to receive message buffer (same as supplied to
                                        msg_receive()) */
    uint16_t        rsize;         /**< size of rbuf */
} msg_hdr_t;

/**
 * @brief Send a message. (blocking)
 *
 * This function sends a message to another thread.
 *
 * If the target thread is waiting for a message, the message will be copied
 * and the function returns.
 *
 * If the target thread is not waiting for a message, but has a messgae queue,
 * the message will be queued and the function returns.
 *
 * If the target thread is not waiting for a message, the function will block
 * until the target thread tries to receive a message. (see msg_try_receive())
 *
 * The amount of data is the minimum of @c size and the target thread's message
 * buffer as supplied to msg_(try)receive.
 *
 * @param  target reference to target message node
 * @param  buf Pointer to buffer that should be sent
 * @param  size Size of buffer (must be >= sizeof(msg_pulse_t))
 *
 * @return >0 amount of data that was actually sent.
 * @return -1 on error (invalid target node)
 */
int msg_send(msg_node_t *target, char *buf, uint16_t size);

/**
 * @brief Send a message. (non-blocking)
 *
 * Non-blocking version of msg_send().
 * Will return at once if no message can be delivered without blocking.
 *
 * @param  target_pid PID of target thread
 * @param  buf Pointer to buffer that should be sent
 * @param  size Size of buffer
 *
 * @return >0 amount of data that was actually sent.
 * @return 0 if receiver is not waiting or has a full message queue
 * @return -1 on error (invalid PID)
 */
int msg_try_send(msg_node_t *target, char *buf, uint16_t size);

/**
 * @brief Send a message from interrupt context. (non-blocking)
 *
 * Non-blocking version of msg_send() to be used from ISR context.
 * Will return at once if no message can be delivered without blocking.
 *
 * @param  target_pid PID of target thread
 * @param  buf Pointer to buffer that should be sent
 * @param  size Size of buffer
 *
 * @return >0 amount of data that was actually sent.
 * @return 0 if receiver is not waiting or has a full message queue
 * @return -1 on error (invalid PID)
 */
int msg_send_int(msg_node_t *target, char *buf, uint16_t size);

/**
 * @brief Receive a message. (blocking)
 *
 * This function receives a message.
 *
 * If one or more sender's are trying to send a message to the calling thread,
 * or a message is waiting in a message queue, the message will be copied
 * and the function returns.
 *
 * If no sender or message is waiting, this function will block until a message
 * can be received.
 *
 * The amount of data is the minimum of @c size and the sending thread's
 * message buffer as supplied to msg_(try)send.
 *
 * @param  buf Pointer to buffer where the received message should be stored
 * @param  size Size of buffer (must be >= sizeof(msg_pulse_t))
 * @param  hdr pointer to structure containing auxillary msg information
 *
 * @return >0 amount of data that was actually received
 * @return -1 on error
 */
int msg_receive(char *buf, uint16_t size, msg_hdr_t *hdr);

/**
 * @brief Receive a message. (non-blocking)
 *
 * This function receives a message. (non-blocking version of msg_receive())
 * Like msg_receive, but will return if no message can be received without
 * blocking.
 *
 * @param  buf Pointer to buffer where the received message should be stored
 * @param  size Size of buffer (must be >= sizeof(msg_pulse_t))
 * @param  hdr pointer to structure containing auxillary msg information
 *
 * @return >0 amount of data that was actually received
 * @return 0 if no message was waiting
 * @return -1 on error
 */
int msg_try_receive(char *buf, uint16_t size, msg_hdr_t *hdr);

/**
 * @brief Describes a message pulse object which can be sent between threads.
 *
 * User can set type and one of content.ptr and content.value. (content is a union)
 * The meaning of type and the content fields is totally up to the user,
 * the corresponding fields are never read by the kernel.
 *
 */
typedef struct msg_pulse {
    uint16_t     sender_pid;    /**< */
    uint16_t     type;          /**< Type field. */
    union {
        char     *ptr;          /**< Pointer content field. */
        uint32_t value;         /**< Value content field. */
    } content;                  /**< Content of the message. */
} msg_pulse_t;

/**
 * @brief Send a pulse.
 *
 * A pulse is a short message.
 * This function sends a message pulse to another thread.
 * The msg_pulse structure has to be allocated (e.g. on the stack)
 * before calling the function and can be freed afterwards.
 * If called from an interrupt, this function will never block.
 *
 * @param  m Pointer to message pulse structure
 * @param  target_pid PID of target thread
 * @param  block If true and receiver is not receive-blocked, function will block. If not, function
 * returns.
 *
 * @return 1 if sending was successful (pulse delivered directly or to a queue)
 * @return 0 if receiver is not waiting or has a full message queue and block == false
 * @return -1 on error (invalid PID)
 */
int msg_send_pulse(msg_pulse_t *m, unsigned int target_pid);
int msg_try_send_pulse(msg_pulse_t *m, unsigned int target_pid);

/**
 * @brief Send a message pulse to the current thread.
 * @details Will work only if the thread has a message queue.
 *
 * Will be automatically chosen instead of @c msg_send_pulse
 * if @c target_pid == @c thread_pid.
 * This function never blocks.
 *
 * @param  m pointer to message pulse structure
 *
 * @return 1 if sending was successful
 * @return 0 if the thread's message queue is full (or inexistent)
 */
int msg_send_pulse_to_self(msg_pulse_t *m);

/**
 * @brief Send message pulse from interrupt.
 *
 * Will be automatically chosen instead of msg_send_pulse if inISR() == true
 *
 * @param  m pointer to message pulse structure
 * @param  target_pid PID of target thread
 *
 * @return 1 if sending was successful
 * @return 0 if receiver is not waiting and has no message queue
 */
int msg_send_pulse_int(msg_pulse_t *m, unsigned int target_pid);

/**
 * @brief Receive a message pulse.
 *
 * This function blocks until a message pulse was received.
 * @param m pointer to preallocated msg pulse
 *
 * @return 1 Function always succeeds or blocks forever.
 */
int msg_receive_pulse(msg_pulse_t *m);

/**
 * @brief Try to receive a message pulse.
 *
 * This function does not block if no message pulse can be received.
 * @param m pointer to preallocated msg
 *
 * @return 1 if a message was received, -1 otherwise.
 */
int msg_try_receive_pulse(msg_pulse_t *m);

/**
 * @brief Send a message pulse, block until reply received.
 *
 * This function sends a message pulse to target_pid and then blocks until target has sent a reply.
 * @note CAUTION!Use this function only when receiver is already waiting. If not use simple msg_send_pulse()
 * @param m pointer to preallocated msg
 * @param reply pointer to preallocated msg. Reply will be written here.
 * @param target pid the pid of the target process
 * @return 1 if successful
 */
int msg_send_receive_pulse(msg_pulse_t *m, msg_pulse_t *reply, unsigned int target_pid);

int msg_reply_pulse(msg_pulse_t *m, msg_pulse_t *reply);

/** @} */
#endif /* __MSG_H */
