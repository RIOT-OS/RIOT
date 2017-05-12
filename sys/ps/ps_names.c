/**
 * Print thread information.
 *
 * Copyright (C) 2013, INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup ps
 * @{
 * @file
 * @brief   UNIX like ps command
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "thread.h"
#include "sched.h"
#include "thread.h"
#include "kernel_types.h"

/* list of states copied from tcb.h */
const char *state_names[] = {
    [STATUS_RUNNING] = "running",
    [STATUS_PENDING] = "pending",
    [STATUS_STOPPED] = "stopped",
    [STATUS_SLEEPING] = "sleeping",
    [STATUS_MUTEX_BLOCKED] = "bl mutex",
    [STATUS_RECEIVE_BLOCKED] = "bl rx",
    [STATUS_SEND_BLOCKED] = "bl send",
    [STATUS_REPLY_BLOCKED] = "bl reply",
    [STATUS_FLAG_BLOCKED_ANY] = "bl anyfl",
    [STATUS_FLAG_BLOCKED_ALL] = "bl allfl"
};
