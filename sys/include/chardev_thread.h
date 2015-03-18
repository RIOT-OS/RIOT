/*
 * Copyright (C) 2010 Kaspar Schleiser
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_chardevthread Chardev Thread
 * @ingroup     sys
 * @brief       Chardev thread
 * @{
 *
 * @file
 * @brief       Handles I/O from a char devices
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __CHARDEV_THREAD_H
#define __CHARDEV_THREAD_H

#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

void chardev_loop(ringbuffer_t *rb);
void *chardev_thread_entry(void *rb_);

#ifdef __cplusplus
}
#endif

#endif /* __CHARDEV_THREAD_H */
/** @} */
