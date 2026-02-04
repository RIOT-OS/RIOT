/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    core_msg  Messaging / IPC
 * @ingroup     core
 * @brief       Messaging API for inter process communication
 *
 * Messages
 * ========
 * IPC messages consist of a sender PID, a type, and some content. The sender
 * PID will be set by the IPC internally and is not required to be set by the
 * user. The type helps the receiver to multiplex different message types.
 * The content can either be provided as a 32-bit integer or a pointer.
 *
 * Some message types are predefined; for example, @ref
 * GNRC_NETAPI_MSG_TYPE_RCV & co are defined. These are fixed in the sense that
 * registering for a particular set of messages (for the above, e.g. @ref
 * gnrc_netreg_register) will use these message types. Threads that do nothing
 * to receive such messages can safely use the same numbers for other purposes.
 * The predefined types use non-conflicting ranges (e.g. `0x02NN`) to avoid
 * ruling out simultaneous use of different components in the same thread.
 *
 * In general, threads may consider it an error to send them a message they did
 * not request. Best practice is to log (but otherwise ignore) unexpected
 * messages.
 *
 * Blocking vs non-blocking
 * ========================
 * Messages can be sent and received blocking and non-blocking. Both can be
 * used combined: A message send while blocking the sender thread can be
 * received with the non-blocking variant and vice-versa.
 *
 * Blocking IPC
 * ------------
 * For the blocking variant use @ref msg_send() or @ref msg_receive()
 * respectively.
 *
 * Additionally, one can use @ref msg_send_receive() to simultaneously block
 * the sending thread and expect a response from the receiving thread. In this
 * case, the receiving thread must use @ref msg_reply() to reply to the message
 * of the sender thread.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <inttypes.h>
 * #include <stdio.h>
 *
 * #include "msg.h"
 * #include "thread.h"
 *
 * static kernel_pid_t rcv_pid;
 * static char rcv_stack[THREAD_STACKSIZE_DEFAULT];
 *
 * static void *rcv(void *arg)
 * {
 *     msg_t msg_req, msg_resp;
 *
 *     (void)arg;
 *     while (1) {
 *         msg_receive(&msg_req);
 *         msg_resp.content.value = msg_req.content.value + 1;
 *         msg_reply(&msg_req, &msg_resp);
 *     }
 *     return NULL;
 * }
 *
 * int main(void)
 * {
 *     msg_t msg_req, msg_resp;
 *
 *     msg_resp.content.value = 0;
 *     rcv_pid = thread_create(rcv_stack, sizeof(rcv_stack),
 *                             THREAD_PRIORITY_MAIN - 1, 0, rcv, NULL, "rcv");
 *     while (1) {
 *         msg_req.content.value = msg_resp.content.value;
 *         msg_send_receive(&msg_req, &msg_resp, rcv_pid);
 *         printf("Result: %" PRIu32 "\n", msg_resp.content.value);
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Non-blocking IPC
 * ----------------
 * For the non-blocking variant use @ref msg_try_send() or
 * @ref msg_try_receive() respectively. If a message is sent in synchronous
 * mode or the message queue (see below) of the receiving thread is full
 * messages sent this way will be dropped.
 *
 * You can use the example on asynchronous IPC below - but without the queue -
 * to get an impression of how to use non-blocking IPC.
 *
 * Synchronous vs Asynchronous
 * ===========================
 * RIOT's IPC supports both synchronous and asynchronous IPC.
 *
 * Synchronous IPC
 * ---------------
 * Synchronous IPC is the default mode i.e. is active when the receiving thread
 * has no message queue initialized. Messages that can't be delivered when
 * sending non-blocking (because the receiver already received a message) or
 * which are sent when the receiver is not receive-blocked will be dropped.
 *
 * Asynchronous IPC
 * ----------------
 * To use asynchronous IPC one needs to initialize a message queue using
 * @ref msg_init_queue() (note that it **must** be of a size equal to a power of
 * two). Messages sent to a thread with a message queue that isn't full are
 * never dropped and the sending never blocks, even when using @ref msg_send().
 * If the queue is full and the sending thread has a higher priority than the
 * receiving thread the send-behavior is equivalent to synchronous mode.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include <inttypes.h>
 * #include <stdio.h>
 *
 * #include "msg.h"
 * #include "thread.h"
 *
 * #define RCV_QUEUE_SIZE  (8)
 *
 * static kernel_pid_t rcv_pid;
 * static char rcv_stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
 * static msg_t rcv_queue[RCV_QUEUE_SIZE];
 *
 * static void *rcv(void *arg)
 * {
 *     msg_t msg;
 *
 *     (void)arg;
 *     msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);
 *     while (1) {
 *         msg_receive(&msg);
 *         printf("Received %" PRIu32 "\n", msg.content.value);
 *     }
 *     return NULL;
 * }
 *
 * int main(void)
 * {
 *     msg_t msg;
 *
 *     msg.content.value = 0;
 *     rcv_pid = thread_create(rcv_stack, sizeof(rcv_stack),
 *                             THREAD_PRIORITY_MAIN - 1, 0, rcv, NULL, "rcv");
 *     while (1) {
 *         if (msg_try_send(&msg, rcv_pid) == 0) {
 *             printf("Receiver queue full.\n");
 *         }
 *         msg.content.value++;
 *     }
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Timing & messages
 * =================
 * Timing out the reception of a message or sending messages at a certain time
 * is out of scope for the basic IPC provided by the kernel. See the
 * @ref sys_xtimer "xtimer" module on information for these functionalities.
 *
 * @{
 *
 * @file
 * @brief       Messaging API for inter process communication
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#include <stdint.h>
#include <stdbool.h>

#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Describes a message object which can be sent between threads.
 *
 * User can set type and one of content.ptr and content.value. (content is a union)
 * The meaning of type and the content fields is totally up to the user,
 * the corresponding fields are never read by the kernel.
 *
 */
typedef struct {
    kernel_pid_t sender_pid;    /**< PID of sending thread. Will be filled in
                                     by msg_send. */
    uint16_t type;              /**< Type field. */
    union {
        void *ptr;              /**< Pointer content field. */
        uint32_t value;         /**< Value content field. */
    } content;                  /**< Content of the message. */
} msg_t;

/**
 * @brief Send a message (blocking).
 *
 * This function sends a message to another thread. The ``msg_t`` structure has
 * to be allocated (e.g. on the stack) before calling the function and can be
 * freed afterwards. If called from an interrupt, this function will never
 * block.
 *
 * @param[in] m             Pointer to preallocated ``msg_t`` structure, must
 *                          not be NULL.
 * @param[in] target_pid    PID of target thread
 *
 * @return 1, if sending was successful (message delivered directly or to a
 *            queue)
 * @return 0, if called from ISR and receiver cannot receive the message now
 *            (it is not waiting or it's message queue is full)
 * @return -1, on error (invalid PID)
 */
int msg_send(msg_t *m, kernel_pid_t target_pid);

/**
 * @brief Send a message (non-blocking).
 *
 * This function sends a message to another thread. The ``msg_t`` structure has
 * to be allocated (e.g. on the stack) before calling the function and can be
 * freed afterwards. This function will never block.
 *
 * @param[in] m             Pointer to preallocated ``msg_t`` structure, must
 *                          not be NULL.
 * @param[in] target_pid    PID of target thread
 *
 * @return 1, if sending was successful (message delivered directly or to a
 *         queue)
 * @return 0, if receiver is not waiting or has a full message queue
 * @return -1, on error (invalid PID)
 */
int msg_try_send(msg_t *m, kernel_pid_t target_pid);

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
 * Value of msg_t::sender_pid if the sender was an interrupt service routine.
 */
#define KERNEL_PID_ISR (KERNEL_PID_LAST + 1)

/**
 * @brief Send message from interrupt.
 *
 * Will be automatically chosen instead of msg_send() if called from an
 * interrupt/ISR.
 *
 * The value of ``m->sender_pid`` is set to @ref KERNEL_PID_ISR.
 *
 * @see msg_sent_by_int()
 *
 * @param[in] m             Pointer to preallocated @ref msg_t structure, must
 *                          not be NULL.
 * @param[in] target_pid    PID of target thread.
 *
 * @return 1, if sending was successful
 * @return 0, if receiver is not waiting and ``block == 0``
 * @return -1, on error (invalid PID)
 */
int msg_send_int(msg_t *m, kernel_pid_t target_pid);

/**
 * @brief Test if the message was sent inside an ISR.
 * @see msg_send_int()
 * @param[in] m The message in question.
 * @returns `== 0` if *not* sent by an ISR
 * @returns `!= 0` if sent by an ISR
 */
static inline int msg_sent_by_int(const msg_t *m)
{
    return (m->sender_pid == KERNEL_PID_ISR);
}

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
 * has sent a reply which is then stored in *reply*. The responding thread must
 * use @ref msg_reply().
 *
 * Any incoming messages other than the reply are put into the queue (if one is
 * configured), block the sender (if sent with @ref msg_send from a thread), or
 * rejected (if sent with @ref msg_try_send or from an interrupt) -- just like
 * if the thread were blocked on anything different than message reception.
 *
 * @pre     @p target_pid is not the PID of the current thread.
 *
 * @param[in] m             Pointer to preallocated ``msg_t`` structure with
 *                          the message to send, must not be NULL.
 * @param[out] reply        Pointer to preallocated msg. Reply will be written
 *                          here, must not be NULL. Can be identical to @p m.
 * @param[in] target_pid    The PID of the target process
 *
 * @return  1, if successful.
 */
int msg_send_receive(msg_t *m, msg_t *reply, kernel_pid_t target_pid);

/**
 * @brief Replies to a message.
 *
 * Sender must have sent the message with msg_send_receive().
 *
 * @param[in] m         message to reply to, must not be NULL.
 * @param[out] reply    message that target will get as reply, must not be NULL.
 *
 * @return 1, if successful
 * @return -1, on error
 */
int msg_reply(msg_t *m, msg_t *reply);

/**
 * @brief Replies to a message from interrupt.
 *
 * An ISR can obviously not receive messages, however a thread might delegate
 * replying to a message to an ISR.
 *
 * @param[in] m         message to reply to, must not be NULL.
 * @param[out] reply    message that target will get as reply, must not be NULL.
 *
 * @return 1, if successful
 * @return -1, on error
 */
int msg_reply_int(msg_t *m, msg_t *reply);

/**
 * @brief Check how many messages are available (waiting) in the message queue
 *        of a specific thread
 *
 * @param[in] pid    a PID
 *
 * @return  Number of messages available in queue of the thread identified by
 *          PID @p pid
 * @retval  0       The message queue of the thread identified by PID @p pid is
 *                  *not* initialized or PID @p pid does not refer to a running
 *                  thread
 */
unsigned msg_avail_thread(kernel_pid_t pid);

/**
 * @brief Check how many messages are available (waiting) in the message queue
 *
 * @pre     The caller is running in thread context
 *
 * @return  Number of messages available in our queue
 * @retval  0       Caller's message queue is *not* initialized
 */
unsigned msg_avail(void);

/**
 * @brief   Get maximum capacity of a thread's queue length
 *
 * @return  Number of total messages that fit in the queue of @p pid on success
 * @retval  0       Either the thread identified by PID @p pid does not exist or
 *                  has no message queue initialized (yet)
 */
unsigned msg_queue_capacity(kernel_pid_t pid);

/**
 * @brief Initialize the current thread's message queue.
 *
 * @pre @p num **MUST BE A POWER OF TWO!**
 *
 * @param[in] array Pointer to preallocated array of ``msg_t`` structures, must
 *                  not be NULL.
 * @param[in] num   Number of ``msg_t`` structures in array.
 *                  **MUST BE POWER OF TWO!**
 *
 * If array resides on the stack, the containing stack frame must never be
 * left, not even if it is the current thread's entry function.
 */
void msg_init_queue(msg_t *array, int num);

/**
 * @brief Number of messages to be maximally printed through @ref msg_queue_print
 */
#ifndef CONFIG_MSG_QUEUE_PRINT_MAX
#  define CONFIG_MSG_QUEUE_PRINT_MAX 16U
#endif

/**
 * @brief   Prints the message queue of the current thread.
 *
 * @note    Prints at most CONFIG_MSG_QUEUE_PRINT_MAX messages.
 */
void msg_queue_print(void);

#ifdef __cplusplus
}
#endif

/** @} */
