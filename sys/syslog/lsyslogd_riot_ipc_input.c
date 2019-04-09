/*
 * Copyright (C) 2019 Robin Lösch <robin@chilio.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_syslog
 * @{
 *
 * @file
 * @brief       Syslog RIOT IPC Wrapper for lsyslogd pkg
 *
 * @author      Robin Lösch <robin@chilio.net>
 *
 * @}
 */
#ifdef LSYSLOGD_FEATURE_INPUT_RIOT_IPC

#include <string.h>
#include "lsyslogd.h"
#include "msg.h"
#include "thread.h"
#include "syslog/syslog.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static char rcv_stack[THREAD_STACKSIZE_DEFAULT];
static msg_t rcv_queue[RCV_QUEUE_SIZE];
extern kernel_pid_t syslog_pid;

static int syslog_riot_ipc_read(char *buf, int len)
{
    msg_t msg;
    msg_receive(&msg);
    syslog_msg_t *syslog_msg = msg.content.ptr;
    size_t min = ((size_t)(len - 1) < syslog_msg->len) ? (size_t)(len - 1) : syslog_msg->len;
    DEBUG("syslog_riot_ipc_read: Received syslog msg with %d bytes buf size is %d\n", syslog_msg->len, len);
    strncpy(buf, syslog_msg->ptr, min);
    buf[min] = 0;
    return min;
}

static void *thread(void *opt) {
    (void)opt;
    lsyslogd_run();
    return NULL;
}

static int syslog_riot_ipc_init(char *opt) {
    (void)opt;
    syslog_pid = thread_create(rcv_stack, sizeof(rcv_stack),
                            THREAD_PRIORITY_MAIN - 1, 0, thread, NULL, "syslog_riot_ipc");
    msg_init_queue(rcv_queue, RCV_QUEUE_SIZE);
    DEBUG("syslog_riot_ipc_init: Thread launced with pid %d\n", syslog_pid);
    return 0;
}

syslog_input_t lsyslogd_riot_ipc_input = {._read = syslog_riot_ipc_read, ._init = syslog_riot_ipc_init};
#else
typedef int dont_be_pedantic;
#endif //LSYSLOGD_FEATURE_INPUT_RIOT_IPC
