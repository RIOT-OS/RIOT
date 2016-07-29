/*
 * Copyright (C) 2016 José Ignacio Alamos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#include "msg.h"
#include <stdio.h>
#include "ot.h"
#include "xtimer.h"
#include "msg.h"
#include "string.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static char c;
/* init and run OpeanThread's Command Line Interface */
void openthread_uart_run(void)
{
	msg_t msg;
	msg.type = OPENTHREAD_SERIAL_MSG_TYPE_EVENT;
	msg.content.ptr = &c;

	while(1)
	{
        c = getchar();
		msg_send(&msg, openthread_get_pid());
	}
}
/** @} */
