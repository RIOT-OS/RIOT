/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  posix
 * @{
 */

/**
 * @file    fork.h
 * @brief   POSIX-compliant forking capabilities
 *
 * @author  Jakob Pfender <jakob.pfender@fu-berlin.de>
 */
#ifndef _FORK_H
#define _FORK_H

#include <stdint.h>

#define FORK_MAX_STACKSIZE	KERNEL_CONF_STACKSIZE_PRINTF
#define FORK_MAXTHREADS		MAXTHREADS / 2

/**
 * @brief initialize stack pool for forked threads
 * @details fork_stacks holds a number of stacks equal to
 *          FORK_MAXTHREADS that are all initially marked as unused.
 *          Stacks are allocated by simply marking them as used and
 *          freed by marking them as unused
 */
void fork_init(void);

/**
 * @brief fork the current thread
 * @details Creates a new thread, copies current stackframe and stack
 * 	    pointer to the new thread and begins instruction at the same
 * 	    point as parent thread.
 * @see
 * @return the PID of the child thread in the parent thread, 0 in the
 *         child thread
 */
pid_t _fork(void);

/**
 * @brief free a thread's stack
 * @details Simply marks the given thread's stack as unused.
 * @see
 * @return 0 on success
 */
int fork_free(void);

/**
 * @}
 */
#endif /* _FORK_H */
