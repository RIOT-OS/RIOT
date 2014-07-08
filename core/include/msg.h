/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * There are two ways to use the IPC Messaging system of RIOT. The default is
 * synchronous messaging. In this manner, messages are either dropped when the
 * receiver is not waiting and the message was sent non-blocking, or will be
 * delivered immediately when the receiver calls msg_receive(msg_t* m). To use
 * asynchronous messaging any thread can create its own queue by calling
 * msg_init_queue(msg_t* array, int num). Messages sent to a thread with a non
 * full message queue are never dropped * and the sending never blocks. Threads
 * with a full message queue behaves like in synchronous mode.
 *
 * @{
 *
 * @file        msg.h
 * @brief       Messaging API for inter process communication
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef __MSG_H_
#define __MSG_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Describes a message object which can be sent between threads.
 *
 * User can set type and one of content.ptr and content.value. (content is a union)
 * The meaning of type and the content fields is totally up to the user,
 * the corresponding fields are never read by the kernel.
 *
 */
typedef struct msg {
    uint16_t     sender_pid;    /**< PID of sending thread. Will be filled in
                                     by msg_send. */
    uint16_t     type;          /**< Type field. */
    union {
        char     *ptr;          /**< Pointer content field. */
        uint32_t value;         /**< Value content field. */
    } content;                  /**< Content of the message. */
} msg_t;


/**
 * @brief Send a message.
 *
 * This function sends a message to another thread. The ``msg_t`` structure has
 * to be allocated (e.g. on the stack) before calling the function and can be
 * freed afterwards. If called from an interrupt, this function will never
 * block.
 *
 * @param[in] m             Pointer to preallocated ``msg_t`` structure, must
 *                          not be NULL.
 * @param[in] target_pid    PID of target thread
 * @param[in] block         If not 0 and receiver is not receive-blocked,
 *                          function will block. If not, function returns.
 *
 * @return 1, if sending was successful (message delivered directly or to a
 *         queue)
 * @return 0, if receiver is not waiting or has a full message queue and
 *         ``block == 0``
 * @return -1, on error (invalid PID)
 */
int msg_send(msg_t *m, unsigned int target_pid, bool block);


/**
 * @brief Send a message to the current thread.
 * @details Will work only if the thread has a message queue.
 *
 * Will be automatically chosen instead of @c msg_send
 * if @c target_pid == @c thread_pid.
 * This function never blocks.
 *
 * @param  m pointer to message structure
 *
 * @return 1 if sending was successful
 * @return 0 if the thread's message queue is full (or inexistent)
 */
int msg_send_to_self(msg_t *m);

/**
 * @brief Send message from interrupt.
 *
 * Will be automatically chosen instead of ``msg_sennd()`` if called from an
 * interrupt/ISR.
 *
 * @param[in] m             Pointer to preallocated ``msg_t`` structure, must
 *                          not be NULL.
 * @param[in] target_pid    PID of target thread.
 *
 * @return 1, if sending was successful
 * @return 0, if receiver is not waiting and ``block == 0``
 * @return -1, on error (invalid PID)
 */
int msg_send_int(msg_t *m, unsigned int target_pid);


/**
 * @brief Receive a message.
 *
 * This function blocks until a message was received.
 *
 * @param[out] m    Pointer to preallocated ``msg_t`` structure, must not be
 *                  NULL.
 *
 * @return  1, Function always succeeds or blocks forever.
 */
int msg_receive(msg_t *m);

/**
 * @brief Try to receive a message.
 *
 * This function does not block if no message can be received.
 *
 * @param[out] m    Pointer to preallocated ``msg_t`` structure, must not be
 *                  NULL.
 *
 * @return  1, if a message was received
 * @return  -1, otherwise.
 */
int msg_try_receive(msg_t *m);

/**
 * @brief Send a message, block until reply received.
 *
 * This function sends a message to *target_pid* and then blocks until target
 * has sent a reply which is then stored in *reply*.
 *
 * @note    CAUTION! Use this function only when receiver is already waiting.
 *          If not use simple msg_send()
 * @param[in] m             Pointer to preallocated ``msg_t`` structure with
 *                          the message to send, must not be NULL.
 * @param[out] reply        Pointer to preallocated msg. Reply will be written
 *                          here, must not be NULL.
 * @param[in] target_pid    The PID of the target process
 *
 * @return  1, if successful.
 */
int msg_send_receive(msg_t *m, msg_t *reply, unsigned int target_pid);

/**
 * @brief Replies to a message.
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
 * @brief Initialize the current thread's message queue.
 *
 * @param[in] array Pointer to preallocated array of ``msg_t`` structures, must
 *                  not be NULL.
 * @param[in] num   Number of ``msg_t`` structurs in array.
 *                  **MUST BE POWER OF TWO!**
 *
 * @return 0, if successful
 * @return -1, on error
 */
int msg_init_queue(msg_t *array, int num);

#endif /* __MSG_H_ */
/** @} */
