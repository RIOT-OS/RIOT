/**
 * Logging daemon 
 *
 * Copyright (C) 2009-2013 Freie Universitaet Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file        logd.c
 * @brief		Simple logging demon implementation
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: logd.c 3854 2011-12-06 15:27:01Z hwill $
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
/* core */
#include "msg.h"
#include "flags.h"
#include "mutex.h"
#include "thread.h"
#include "kernel.h"
/* system */
#include "logd.h"
#include "list.h"

typedef struct {
    list_node_t listnode;
    char *str;
    int str_len;
} log_queue_t;

static volatile int log_pid = -1;
static int logd_stack_size = LOGD_STACK_SIZE_NORMAL;
static FILE *fh = NULL;
static int log_count = 0;
static mutex_t log_mutex;
static list_t log_msg_queue;
static volatile bool exit_flag = false;
static volatile bool echo_on = false;

/*---------------------------------------------------------------------------*/

static void close_file_handle(void)
{
    if(fh != NULL) {
        fclose(fh);
        fh = NULL;
    }
}

static void write_to_file(char *str, int str_len)
{
    if(fh != NULL && str_len > 0) {
        if(fwrite(str, sizeof(char), str_len, fh) != str_len) {
            if(echo_on && logd_stack_size >= LOGD_STACK_SIZE_CONSOLE) {
                printf("LOGD [WARN]: file write failed, closing file\n");
            }

            close_file_handle();
            return;
        }

        if(fflush(fh) == EOF) {
            if(echo_on && logd_stack_size >= LOGD_STACK_SIZE_CONSOLE) {
                printf("LOGD [WARN]: file write failed, closing file\n");
            }

            close_file_handle();
            return;
        }
    }
    else {
        fh = fopen("/LOGD.LOG", "w");

        if(!fh) {
            if(echo_on && logd_stack_size >= LOGD_STACK_SIZE_CONSOLE) {
                printf("LOGD [WARN]: file reopen failed, damn!\n");
            }
        }
        else {
            write_to_file(str, str_len);
        }
    }
}

static void logd_process(void)
{
    msg m;
    log_queue_t *node;

    do {
        if(!exit_flag) {
            msg_receive(&m);
        }

        mutex_lock(&log_mutex);

        while((node = (log_queue_t *) list_remove_head(&log_msg_queue)) != NULL) {
            write_to_file(node->str, node->str_len);

            if(echo_on && logd_stack_size >= LOGD_STACK_SIZE_CONSOLE) {
                printf("%s", node->str);
            }

            log_count++;
            free(node->str);
            free(node);
        }

        mutex_unlock(&log_mutex, 0);
    }
    while(m.type != MSG_EXIT && !exit_flag);

    /* Logging thread is terminating, close log file */
    close_file_handle();
}

/*---------------------------------------------------------------------------*/

static void logd_init0(void)
{
    fh = fopen("/LOGD.LOG", "w");

    if(!fh) {
        return;
    }

    log_pid = thread_create(logd_stack_size, PRIORITY_LOGD, CREATE_STACKTEST, logd_process, "logd");
}

void logd_init(int stack_size)
{
    logd_stack_size = stack_size;
    mutex_init(&log_mutex);
    list_init(&log_msg_queue);
    logd_init0();
}

void logd_set_console_enabled(bool enabled)
{
    echo_on = enabled;
}

bool logd_log(char *str, int str_len)
{
    msg m;

    /* Test if logd process was created */
    if(log_pid == -1) {
        /* no logd created, because fopen() on log file failed. So try again */
        logd_init0();

        if(log_pid == -1) {
            /* Still errors opening log file, exit now */
            return false;
        }
    }

    log_queue_t *lq = malloc(sizeof(*lq));

    if(lq == NULL) {
        return false;
    }

    lq->str = malloc(sizeof(char) * str_len + 1); /* 1 byte for string termination char */

    if(lq->str == NULL) {
        free(lq);
        return false;
    }

    strncpy(lq->str, str, str_len);
    lq->str_len = str_len;
    lq->str[str_len] = '\0';	/* add string termination char at end of buffer */
    mutex_lock(&log_mutex);
    list_append(&log_msg_queue, (list_node_t *) lq);
    mutex_unlock(&log_mutex, 0);
    m.type = MSG_POLL;
    m.content.ptr = NULL;
    msg_send(&m, log_pid, false);
    return true;
}

void logd_exit(void)
{
    msg m;

    /* Test if logd process was created */
    if(log_pid == -1) {
        return;
    }

    exit_flag = true;
    m.type = MSG_EXIT;
    m.content.ptr = NULL;
    msg_send(&m, log_pid, false);
}
