/*
 * Copyright (C) 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     cpu_native
 * @{
 *
 * @file
 * @brief       Multiple asynchronus read on file descriptors
 *
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */
#ifndef ASYNC_READ_H
#define ASYNC_READ_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of file descriptors
 */
#ifndef ASYNC_READ_NUMOF
#define ASYNC_READ_NUMOF 2
#endif

/**
 * @brief   asynchronus read callback type
 */
typedef void (*native_async_read_callback_t)(int fd, void *arg);

/**
 * @brief   initialize asynchronus read system
 *
 * This registers SIGIO signal handler.
 */
void native_async_read_setup(void);

/**
 * @brief   shutdown asynchronus read system
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

#ifdef __cplusplus
}
#endif

#endif /* ASYNC_READ_H */
/** @} */
