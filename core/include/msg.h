/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Kaspar Schleiser
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_msg  Messaging / IPC
 * @ingroup     core
 * @brief       Messaging API for inter process communication
 *
 * RIOT offers two messaging APIs.
 *
 * One is a simple API which can send messages of the type msg_t between two
 * threads. These messages can only contain a uint16 type field and uint32
 * value or pointer.
 * The msg_send_*() / msg_receive() functions are used for this.
 *
 * The second API is a little more complicated, but also more powerful. With
 * it, any buffer can be sent to message node objects. Threads can have
 * multiple message nodes, and message nodes can be passed between threads.
 * This API should be used for applications that
 *  - need to copy data with each message
 *  - need to use multiple message nodes per thread
 *  - might use IPC between physical nodes, e.g., over the network.
 * The bmsg_* functions are used for this API.
 *
 * There are two ways to use the IPC Messaging system of RIOT. The default is synchronous
 * messaging. In this manner, messages are either dropped when the receiver is not waiting and the
 * message was sent non-blocking, or will be delivered immediately when the receiver calls
 * msg_receive(msg_t* m) or bmsg_receive(...).
 * To use asynchronous messaging any thread can create a message queue.
 * Messages sent to a thread with a non full message queue are never dropped
 * and the sending never blocks. Threads with a full message queue behaves like
 * in synchronous mode. Check the msg_queue module for details.
 *
 * @{
 *
 * @file        msg.h
 * @brief       Messaging API for inter process communication
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef __MSG_H_
#define __MSG_H_

#include <stdint.h>
#include <stdbool.h>
#include "kernel_types.h"
#include "priority_queue.h"
#include "ringbuffer.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Bitfield values that can be set / found in msg_hdr_t->flags
 * @{
 */
#define MSG_REPLY_EXPECTED       (1)    /**< The sending thread is REPLY_BLOCKED
                                             and expects a reply for this message */
#define MSG_IS_SIMPLE            (2)    /**< The received msg is of type msg_t */
#define MSG_BLOCK                (4)    /**< The message was sent in blocking mode */
#define MSG_HAS_TARGET           (8)    /**< The message has a target != the target
                                             threads main message node */
/** @} */

/**
 * @brief Describes a message endpoint object
 *
 * These messages endpoints are used internally by the messaging API to
 * send / receive messages.
 *
 */
typedef struct msg_endpoint {
    struct tcb   *owner;          /**< thread that owns this message node   */
    void         *wait_data;      /**< holding current receive data         */
    priority_queue_t msg_waiters; /**< threads waiting to send to this node */

#ifdef MODULE_MSG_QUEUE
    struct msg_queue *msg_queue;  /**< ptr to this node's msg queue         */
#endif
} msg_endpoint_t;

/**
 * @brief Describes a message node object.
 *
 * These are used by the public messaging functions as source and target
 * references for messaging.
 */
typedef struct msg_node {
    msg_endpoint_t *endpoint;     /**< pointer to a msg_endpoint that will be
                                       the source or target of a message
                                       sent or received using this msg_node */
} msg_node_t;

/**
 * @brief Describes a message header object that contains auxillary message data.
 *
 * Pass a pointer to a preallocated msg_hdr_t object
 * to bmsg_receive() to get extra message info.
 *
 * This is also used internally by RIOT's msg subsystem.
 */
typedef struct msg_hdr {
    msg_node_t *sender_node;  /**< Pointer to sender message node */
    msg_node_t *target_node;  /**< Pointer to target message node */
    char            *payload; /**< Pointer to message send buffer (same as supplied to
                                   bmsg_send()) */
    uint16_t        size;     /**< Receive buffer size. */
    uint16_t        flags;    /**< Bitfield to hold several flags */
    char            *rbuf;    /**< Pointer to receive message buffer (same as supplied to
                                   bmsg_receive()) */
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
 * If the target thread is not waiting for a message, but has a message queue,
 * the message will be queued and the function returns.
 *
 * If the target thread is not waiting for a message, the function will block
 * until the target thread tries to receive a message. (see msg_try_receive())
 *
 * The amount of data is the minimum of @c size and the target thread's message
 * buffer as supplied to msg_(try)receive.
 *
 * @c source can be NULL. In this case, the default message node of the sending
 * thread will be set as source message node.
 *
 * @param  target reference to target message node
 * @param  source reference to source message node or NULL
 * @param  buf Pointer to buffer that should be sent
 * @param  size Size of buffer (must be >= sizeof(msg_t))
 *
 * @return >0 amount of data that was actually sent.
 * @return -1 on error (invalid target node)
 */
int bmsg_send(msg_node_t *target, msg_node_t *source, char *buf, uint16_t size);

/**
 * @brief Send a message. (non-blocking)
 *
 * Non-blocking version of msg_send().
 * Will return at once if no message can be delivered without blocking.
 *
 * @param  target reference to target message node
 * @param  source reference to source message node or NULL
 * @param  buf Pointer to buffer that should be sent
 * @param  size Size of buffer
 *
 * @return >0 amount of data that was actually sent.
 * @return 0 if receiver is not waiting or has a full message queue
 * @return -1 on error (invalid PID)
 */
int bmsg_try_send(msg_node_t *target, msg_node_t *source, char *buf, uint16_t size);

/**
 * @brief Send a message from interrupt context. (non-blocking)
 *
 * Non-blocking version of bmsg_send() to be used from ISR context.
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
int bmsg_send_int(msg_node_t *target, char *buf, uint16_t size);

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
 * The amount of data being copied is the minimum of @c size and the sending thread's
 * message buffer as supplied to bmsg_(try)send.
 *
 * This function will receive any message that is being send to any of the thread's
 * message nodes. To find out which message node was the intended target,
 * supply a msg_hdr structure and retrieve it's target_node member.
 *
 * @param  buf Pointer to buffer where the received message should be stored
 * @param  size Size of buffer (must be >= sizeof(msg_t))
 * @param  hdr pointer to structure containing auxillary msg information
 *
 * @return >0 amount of data that was actually received
 * @return -1 on error
 */
int bmsg_receive(char *buf, uint16_t size, msg_hdr_t *hdr);

/**
 * @brief Receive a message. (non-blocking)
 *
 * This function receives a message. (non-blocking version of bmsg_receive())
 * Like msg_receive, but will return if no message can be received without
 * blocking.
 *
 * @param  buf Pointer to buffer where the received message should be stored
 * @param  size Size of buffer (must be >= sizeof(msg_t))
 * @param  hdr pointer to structure containing auxillary msg information
 *
 * @return >0 amount of data that was actually received
 * @return 0 if no message was waiting
 * @return -1 on error
 */
int bmsg_try_receive(char *buf, uint16_t size, msg_hdr_t *hdr);

/**
 * @brief Send a message, expect reply. (blocking)
 *
 * This function send a message and then sets the calling thread to
 * RECEIVE_BLOCKED state, waiting for a reply.
 * If the target retrieves msg_hdr, the flag MSG_EXPECTS_REPLY will be set.
 * Otherwise bmsg_send_receive behaves like a pair of bmsg_send/bmsg_receive calls,
 * but should be more efficient.
 *
 * A message send with bmsg_send_receive *must* be replied to by bmsg_reply.
 *
 * @param  target target messaging node
 * @param  sbuf Pointer to buffer that should be sent
 * @param  ssize Size of send buffer (must be >= sizeof(msg_t))
 * @param  rbuf Pointer to buffer where the reply should be stored
 * @param  rsize Size of receive buffer (must be >= sizeof(msg_t))
 *
 * @return >0 amount of data that was actually received
 * @return -1 on error
 */
int bmsg_send_receive(msg_node_t *target, char *sbuf, uint16_t ssize, char *rbuf, uint16_t rsize);
int bmsg_reply(msg_node_t *target, char *buf, uint16_t size);

/**
 * @brief Describes the content that can be sent within a simple message.
 *
 * See @ref msg_t
 *
 */
typedef union {
    char     *ptr;          /**< Pointer content field. */
    uint32_t value;         /**< Value content field. */
} msg_content_t;

/**
 * @brief Describes a simple message object which can be sent between threads.
 *
 * User can set type and one of content.ptr and content.value. (content is a union)
 * The meaning of type and the content fields is totally up to the user,
 * the corresponding fields are never read by the kernel.
 *
 */
typedef struct msg {
    uint16_t        sender_pid;    /**< */
    uint16_t        type;          /**< Type field. */
    msg_content_t   content;       /**< Content of the message. */
} msg_t;

/**
 * @brief Send a simple message (blocking).
 *
 * This function sends a simple message (a msg_t object)
 * to another thread.
 * The msg structure has to be allocated (e.g. on the stack)
 * before calling the function and can be freed afterwards.
 * If the receiver is not waiting for a message or it's message
 * queue is full, msg_send() will block until the message has
 * been delivered.
 * If called from an interrupt, this function will never block.
 *
 * @param  m Pointer to simple message structure
 * @param  target_pid PID of target thread
 * returns.
 *
 * @return 1 if sending was successful (simple message delivered directly or to a queue)
 * @return -1 on error (invalid PID)
 */
int msg_send(msg_t *m, unsigned int target_pid);

/**
 * @brief Send a simple message (non-blocking).
 *
 * This function sends a simple message (a msg_t object)
 * to another thread.
 * The msg structure has to be allocated (e.g. on the stack)
 * before calling the function and can be freed afterwards.
 * This function will never block.
 *
 * @param  m Pointer to simple message structure
 * @param  target_pid PID of target thread
 * returns.
 *
 * @return 1 if sending was successful (simple message delivered directly or to a queue)
 * @return 0 if receiver is not waiting or has a full message queue
 * @return -1 on error (invalid PID)
 */
int msg_try_send(msg_t *m, unsigned int target_pid);

/**
 * @brief Send a simple message to the current thread.
 * @details Will work only if the thread has a message queue.
 *
 * Will be automatically chosen instead of @c msg_send
 * if @c target_pid == @c thread_pid.
 * This function never blocks.
 *
 * @param  m pointer to simple message structure
 *
 * @return 1 if sending was successful
 * @return 0 if the thread's message queue is full (or inexistent)
 */
int msg_send_to_self(msg_t *m);

/**
 * @brief Send simple message from interrupt.
 *
 * Will be automatically chosen instead of msg_send if inISR() == true
 *
 * @param  m pointer to simple message structure
 * @param  target_pid PID of target thread
 *
 * @return 1 if sending was successful
 * @return 0 if receiver is not waiting and has no message queue
 */
int msg_send_int(msg_t *m, unsigned int target_pid);

/**
 * @brief Receive a simple message.
 *
 * This function blocks until a simple message was received.
 * @param m pointer to preallocated msg simple message
 *
 * @return 1 Function always succeeds or blocks forever.
 */
int msg_receive(msg_t *m);

/**
 * @brief Try to receive a simple message.
 *
 * This function does not block if no simple message can be received.
 * @param m pointer to preallocated msg
 *
 * @return 1 if a message was received, -1 otherwise.
 */
int msg_try_receive(msg_t *m);

/**
 * @brief Send a simple message, block until reply received.
 *
 * This function sends a simple message to target_pid and then blocks until target has sent a reply.
 * @note CAUTION! Use this function only when receiver is already waiting. If not use msg_send()
 * @param m pointer to preallocated msg
 * @param reply pointer to preallocated msg. Reply will be written here.
 * @param target pid the pid of the target process
 * @return 1 if successful
 */
int msg_send_receive(msg_t *m, msg_t *reply, unsigned int target_pid);

/**
 * @brief Replies to a simple message.
 *
 * Sender must have sent the message with msg_send_receive().
 *
 * @param[in] m         message to reply to, must not be NULL.
 * @param[out] reply    message that target will get as reply, must not be
 *                      NULL.
 *
 * @return 1, if successful
 * @return 0, on error
 */
int msg_reply(msg_t *m, msg_t *reply);

/**
 * @brief Initializes a msg_node_t structure.
 *
 * @param[in] msg_node  pointer to preallocated msg_node to be initialized
 *
 * @return This function always succeeds.
 */
void msg_node_init(msg_node_t *msg_node);

/**
 * @brief Convenience macro to calculate needed space for a simple message
 *
 */
#define MSG_SIMPLE_QUEUE_SIZE (sizeof(msg_queue_hdr_t))

/**
 * @brief Convenience macro to calculate needed message queue space
 *
 */
#define MSG_QUEUE_SPACE(N) (N*MSG_SIMPLE_QUEUE_SIZE + sizeof(msg_queue_t))

/**
 * @brief Describes a message queue object
 *
 */
typedef struct msg_queue {
    ringbuffer_t rb;        /**< ringbuffer holding queued messages data */
    int          maxsize;   /**< messages larger then maxsize will be
                                 truncated */
} msg_queue_t;

typedef struct __attribute__ ((__packed__)) msg_queue_hdr {
    char        flags;
    msg_node_t  *sender_node;
    union {
        struct {
            msg_node_t  *target_node;
            unsigned    size;
        } bmsg;
        struct {
            uint16_t type;
            msg_content_t content;
        } msg;
    } fields;
} msg_queue_hdr_t;

/**
 * @brief Sets up a buffer as a thread's message queue
 *
 * This function takes the buffer pointed to by @c buf and sets up the
 * currently running thread's message queue.
 * The queue will truncate messages to maxsize if maxsize is > 0.
 *
 * Each message put into the queue will need (sizeof(msg_hdr_t)) space
 * in addition to the actual message size.
 *
 * One internally used msg_queue_t object will also be allocated from @c buf,
 * keep that in mind when calculating the needed queue buffer size.
 *
 * When using the queue only for messages sent by the simple message API, the
 * needed buffer size can be calculated using @ref MSQ_QUEUE_SPQCE() macro.
 *
 * Example for calculating the needed buffer size:
 * If you want to have space for 20 messages with 128 bytes each, the buffer
 * must be at least (sizeof(msg_queue_t) + 20*(sizeof(msg_hdr_t) + 128)
 * bytes large.
 *
 * @param buf       pointer to buffer that will be used by the message queue
 * @param size      size of buffer
 * @param maxsize   size limit for messages put in this queue (use 0 for unlimited)
 *
 * @return          0 on success, !=0 otherwise
 */
int thread_msg_queue_init(char *buf, int size, int maxsize);

/**
 * @brief Adds a message to a message queue
 *
 * This function copies the message pointed to by @c src into the message queue.
 * If the message payload is larger than the queue's maxsize, it will be
 * truncated to maxsize.
 *
 * If the message doesn't fit, the function returns an error.
 *
 * @param queue     pointer to msg_queue_t object
 * @param src       pointer to msg_hdr_t structure containing source
 *                  buffer information
 *
 * @return          -1 if the message doesn't fit, otherwise the number of payload
 *                  bytes that were copied or 0 if sender is expecting a reply
 */
int msg_queue_add(msg_queue_t *queue, struct msg_hdr *src);

/**
 * @brief Gets a message from a message queue
 *
 * This function copies the first message waiting in the message queue to
 * the buffer pointed to in @c dest.
 *
 * @param queue     pointer to msg_queue_t object
 * @param dest      pointer msg_hdr_t struct containing target buffer information
 *
 * @return          nr of copied message payload bytes (0 if sender is expecting reply or message is a simple message)
 */
int msg_queue_get(msg_queue_t *queue, struct msg_hdr *dest);
#ifdef __cplusplus
}
#endif

#endif /* __MSG_H_ */
/** @} */
