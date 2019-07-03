/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_log_mqueue Log module with threaded IPC backend
 * @ingroup     sys
 * @brief       This module implements an logging module with an IPC backend.
 * @{
 *
 * @file
 * @brief       log_module header
 *
 * @author      Robin Lösch <robin@chilio.net>
 */

#ifndef LOG_MODULE_H
#define LOG_MODULE_H

#include <stdio.h>
#include <stdarg.h>
#include "thread.h"
#include "pipe.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum content size of a single log message.
*/
#ifndef MAX_MSG_SIZE
    #define MAX_MSG_SIZE (512)
#endif

/**
 * @brief Typedef for log handler functions
 *
 * A log handler is a callback used by log_mqueue to dispatch log messages depending on their facility value
 *
 * @param priority  Priority value of the log message
 * @param facility  Facility value of the log message
 * @param msg       Log message content
 * @param msg_len   Length of the message
 * @param pid       Pid of the logging thread
*/
typedef void (*log_mqueue_handler_t)(int priority, int facility, char* msg, int msg_len, kernel_pid_t pid);

/**
 * @brief Pipe used for sending log request to the threaded backend
 *
*/
extern pipe_t log_mqueue_pipe;

/**
 * @brief Wrapper struct for a single log message.
 *
 * This struct is used for IPC from a logging thread to the log_mqueue thread.
 */
typedef struct log_message {
    unsigned len;           /**< Length of the message */
    unsigned level;         /**< Level of the message. level = (priority|facility) */
    kernel_pid_t pid;       /**< Pid of the thread that calls LOG(...) or LOG_*(level,...) */
    char buf[MAX_MSG_SIZE]; /**< Buffer for the actual log content */
} log_msg_t;

/**
 * @brief log_write Function which writes a logging request to an IPC backend (RIOT pipe)
 *
 * This function is used to wrap a log request into a log_msg_t struct.
 * The wrapper struct is written to a RIOT pipe.
 *
 * @param[in] level log level of the message. Value is a facility value ORed with a priority value
 * @param[in] format Format string of the log message
 */
static inline void log_write(unsigned level, const char *format, ...)
{
    log_msg_t log;
    log.pid = thread_getpid();
    log.level = level;
    va_list ap;
    va_start(ap, format);
    vsnprintf(log.buf, MAX_MSG_SIZE, format, ap);
    va_end(ap);
    pipe_write(&log_mqueue_pipe,&log,sizeof(log));
}

#ifdef __cplusplus
}
#endif
/**@}*/
#endif /* LOG_MODULE_H */
