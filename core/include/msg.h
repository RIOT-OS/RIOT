/**
 *  There are two ways to use the IPC Messaging system of RIOT. The default is synchronous
 *  messaging. In this manner, messages are either dropped when the receiver is not waiting and the
 *  message was sent non-blocking, or will be delivered immediately when the receiver calls
 *  msg_receive(msg_t* m). To use asynchronous messaging any thread can create its own queue by
 *  calling msg_init_queue(msg_t* array, int num). Messages sent to a thread with a non full message
 *  queue are never dropped and the sending never blocks. Threads with a full message queue behaves
 *  like in synchronous mode.
 *
 * @defgroup    kernel_msg  Messaging / IPC
 * @ingroup     kernel
 * @{
 */

/**
 * @file
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __MSG_H
#define __MSG_H

#include <stdint.h>
#include <stdbool.h>

#define MESSAGE_SENT 1
#define MESSAGE_PROCESS_NOT_WAITING 0
#define MESSAGE_PROCESS_UNKNOWN 2

/**
 * @brief Describes a message object which can be sent between threads.
 *
 * User can set type and one of content.ptr and content.value. (content is a union)
 * The meaning of type and the content fields is totally up to the user,
 * the corresponding fields are never read by the kernel.
 *
 */
typedef struct msg {
    uint16_t     sender_pid;    ///< PID of sending thread. Will be filled in by msg_send
    uint16_t     type;          ///< Type field.
    union {
        char     *ptr;          ///< pointer content field
        uint32_t value;         ///< value content field
    } content;
} msg_t;


/**
 * @brief Send a message.
 *
 * This function sends a message to another thread.
 * The msg structure has to be allocated (e.g. on the stack)
 * before calling the function and can be freed afterwards.
 * If called from an interrupt, this function will never block.
 *
 * @param  m Pointer to message structure
 * @param  target_pid PID of target thread
 * @param  block If true and receiver is not receive-blocked, function will block. If not, function
 * returns.
 *
 * @return 1 if sending was successfull (message delivered directly or to a queue)
 * @return 0 if receiver is not waiting or has a full message queue and block == false
 * @return -1 on error (invalid PID)
 */
int msg_send(msg_t *m, unsigned int target_pid, bool block);


/**
 * @brief Send message from interrupt.
 *
 * Will be automatically chosen instead of msg_send if inISR() == true
 *
 * @param  m pointer to message structure
 * @param  target_pid PID of target thread
 *
 * @return 1 if sending was successfull
 * @return 0 if receiver is not waiting and block == false
 */
int msg_send_int(msg_t *m, unsigned int target_pid);


/**
 * @brief Receive a message.
 *
 * This function blocks until a message was received.
 * @param m pointer to preallocated msg
 *
 * @return 1 Function always succeeds or blocks forever.
 */
int msg_receive(msg_t *m);

/**
 * @brief Send a message, block until reply received.
 *
 * This function sends a message to target_pid and then blocks until target has sent a reply.
 * @note CAUTION! Use this function only when receiver is already waiting. If not use simple msg_send()
 * @param m pointer to preallocated msg
 * @param reply pointer to preallocated msg. Reply will be written here.
 * @param target pid the pid of the target process
 * @return 1 if successful
 */
int msg_send_receive(msg_t *m, msg_t *reply, unsigned int target_pid);

/**
 * @brief Replies to a message.
 *
 * Sender must have sent the message with msg_send_receive().
 *
 * @param m msg to reply to.
 * @param reply message that target will get as reply
 *
 * @return 1 if succcessful
 * qreturn 0 on error
 */
int msg_reply(msg_t *m, msg_t *reply);

/**
 * @brief Initialize the current thread's message queue.
 *
 * @param array Pointer to preallocated array of msg objects
 * @param num Number of msg objects in array. MUST BE POWER OF TWO!
 */
int msg_init_queue(msg_t *array, int num);

/** @} */
#endif /* __MSG_H */
