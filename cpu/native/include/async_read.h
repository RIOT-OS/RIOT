/*
 * SPDX-FileCopyrightText: 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  Multiple asynchronous read on file descriptors
 * @author Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */

#include <sys/types.h>
#include <poll.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of file descriptors
 */
#ifndef ASYNC_READ_NUMOF
#  define ASYNC_READ_NUMOF 8
#endif

/**
 * @brief   asynchronous read callback type
 */
typedef void (*native_async_read_callback_t)(int fd, void *arg);

/**
 * @brief    Interrupt callback information structure
 */
typedef struct {
    pid_t child_pid;                    /**< PID of the interrupt listener */
    native_async_read_callback_t cb;    /**< Interrupt callback function */
    void *arg;                          /**< Argument ptr for the callback */
    struct pollfd *fd;                  /**< sysfs gpio fd */
} async_read_t;

/**
 * @brief   initialize asynchronous read system
 *
 * This registers SIGIO signal handler.
 */
void native_async_read_setup(void);

/**
 * @brief   shutdown asynchronous read system
 *
 * This deregisters SIGIO signal handler.
 */
void native_async_read_cleanup(void);

/**
 * @brief   resume monitoring of file descriptors
 *
 * Call this function after reading file descriptors.
 *
 * @param[in] fd  The file descriptor to monitor
 */
void native_async_read_continue(int fd);

/**
 * @brief   start monitoring of file descriptor
 *
 * @param[in] fd       The file descriptor to monitor
 * @param[in] arg      Pointer to be passed as arguments to the callback
 * @param[in] handler  The callback function to be called when the file
 *                     descriptor is ready to read.
 */
void native_async_read_add_handler(int fd, void *arg, native_async_read_callback_t handler);

/**
 * @brief   stop monitoring of file descriptor
 *
 * @param[in] fd The file descriptor to stop monitoring
 */
void native_async_read_remove_handler(int fd);

/**
 * @brief   start monitoring of file descriptor as interrupt
 *
 * @param[in] fd       The file descriptor to monitor
 * @param[in] arg      Pointer to be passed as arguments to the callback
 * @param[in] handler  The callback function to be called when the file
 *                     descriptor is ready to read.
 */
void native_async_read_add_int_handler(int fd, void *arg, native_async_read_callback_t handler);

#ifdef __cplusplus
}
#endif

/** @} */
