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
 */

#ifndef __CHARDEV_THREAD_H
#define __CHARDEV_THREAD_H

#include "ringbuffer.h"

void chardev_loop(ringbuffer_t *rb);
void *chardev_thread_entry(void *rb_);

#endif /* __CHARDEV_THREAD_H */
