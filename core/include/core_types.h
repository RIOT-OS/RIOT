/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_types  RIOT's core types
 * @ingroup     core
 * @brief       RIOT's core types
 *
 * @{
 *
 * @file
 * @brief       RIOT's core types
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CORE_TYPES_H
#define CORE_TYPES_H

#include <stdint.h>

#include "cib.h"
#include "clist.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(DEVELHELP) && !defined(CONFIG_THREAD_NAMES)) || DOXYGEN
/**
 * @brief   This global macro enable storage of thread names to help developers.
 *
 *          To activate it set environment variable `THREAD_NAMES=1`, or use Kconfig.
 *          It is automatically enabled if `DEVELHELP` is.
 */
#define CONFIG_THREAD_NAMES
#endif

/**
 * Unique process identifier
 */
typedef int16_t kernel_pid_t;

/**
 * @brief Prototype for a thread entry function
 */
typedef void *(*thread_task_func_t)(void *arg);

/**
 * @name Thread states supported by RIOT
 *
 *       Keep in sync with OpenOCD src/rtos/riot.c
 * @{
 */
typedef enum {
    STATUS_STOPPED,                 /**< has terminated                           */
    STATUS_ZOMBIE,                  /**< has terminated & keeps thread's thread_t */
    STATUS_SLEEPING,                /**< sleeping                                 */
    STATUS_MUTEX_BLOCKED,           /**< waiting for a locked mutex               */
    STATUS_RECEIVE_BLOCKED,         /**< waiting for a message                    */
    STATUS_SEND_BLOCKED,            /**< waiting for message to be delivered      */
    STATUS_REPLY_BLOCKED,           /**< waiting for a message response           */
    STATUS_FLAG_BLOCKED_ANY,        /**< waiting for any flag from flag_mask      */
    STATUS_FLAG_BLOCKED_ALL,        /**< waiting for all flags in flag_mask       */
    STATUS_MBOX_BLOCKED,            /**< waiting for get/put on mbox              */
    STATUS_COND_BLOCKED,            /**< waiting for a condition variable         */
    STATUS_RUNNING,                 /**< currently running                        */
    STATUS_PENDING,                 /**< waiting to be scheduled to run           */
    STATUS_NUMOF                    /**< number of supported thread states        */
} thread_status_t;
/** @} */

/**
 * @brief Type definition of thread_flags_t
 */
typedef uint16_t thread_flags_t;

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
 * @brief @c thread_t holds thread's context data.
 */
typedef struct {
    char *sp;                       /**< thread's stack pointer         */
    thread_status_t status;         /**< thread's status                */
    uint8_t priority;               /**< thread's priority              */

    kernel_pid_t pid;               /**< thread's process id            */

#if defined(MODULE_CORE_THREAD_FLAGS) || defined(DOXYGEN)
    thread_flags_t flags;           /**< currently set flags            */
#endif

    clist_node_t rq_entry;          /**< run queue entry                */

#if defined(MODULE_CORE_MSG) || defined(MODULE_CORE_THREAD_FLAGS) \
    || defined(MODULE_CORE_MBOX) || defined(DOXYGEN)
    void *wait_data;                /**< used by msg, mbox and thread
                                         flags                          */
#endif
#if defined(MODULE_CORE_MSG) || defined(DOXYGEN)
    list_node_t msg_waiters;        /**< threads waiting for their message
                                         to be delivered to this thread
                                         (i.e. all blocked sends)       */
    cib_t msg_queue;                /**< index of this [thread's message queue]
                                         (thread_t::msg_array), if any  */
    msg_t *msg_array;               /**< memory holding messages sent
                                         to this thread's message queue */
#endif
#if defined(DEVELHELP) || defined(SCHED_TEST_STACK) \
    || defined(MODULE_MPU_STACK_GUARD) || defined(DOXYGEN)
    char *stack_start;              /**< thread's stack start address   */
#endif
#if defined(CONFIG_THREAD_NAMES) || defined(DOXYGEN)
    const char *name;               /**< thread's name                  */
#endif
#if defined(DEVELHELP) || defined(DOXYGEN)
    int stack_size;                 /**< thread's stack size            */
#endif
/* enable TLS only when Picolibc is compiled with TLS enabled */
#ifdef PICOLIBC_TLS
    void *tls;                      /**< thread local storage ptr */
#endif
#ifdef HAVE_THREAD_ARCH_T
    thread_arch_t arch;             /**< architecture dependent part    */
#endif
} thread_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CORE_TYPES_H */
