/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Demonstrating the sending and receiving of data via the TFTP server
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#include "thread.h"
#include "net/gnrc/tftp.h"

/* the message queues */
#define TFTP_QUEUE_SIZE     (4)
static msg_t _tftp_msg_queue[TFTP_QUEUE_SIZE];

/* allocate the stack */
char _tftp_stack[THREAD_STACKSIZE_MAIN + THREAD_EXTRA_STACKSIZE_PRINTF];

/* default server text which can be received */
static const char _tftp_server_hello[] = "Hello world,\n"
                                         "\n"
                                         "Welcome to the wonderful world of IoT and embedded systems.\n"
                                         "\n"
                                         "Enjoy the RIOT-OS\n";

static tftp_action_t _tftp_action;

/**
 * @brief called at every transcation start
 */
static bool _tftp_server_start_cb(tftp_action_t action, tftp_mode_t mode,
                                  const char *file_name, size_t *len)
{
    /* translate the mode */
    const char *str_mode = "ascii";

    if (mode == TTM_OCTET) {
        str_mode = "bin";
    }
    else if (mode == TTM_MAIL) {
        str_mode = "mail";
    }

    /* translate the action */
    const char *str_action = "read";
    if (action == TFTP_WRITE) {
        str_action = "write";
    }

    /* display the action being performed */
    printf("tftp_server: %s %s %s:%lu\n", str_mode, str_action, file_name, (unsigned long)*len);

    /* return the length of the text, if this is an read action */
    if (action == TFTP_READ) {
        *len = sizeof(_tftp_server_hello);
    }

    /* remember the action of the current transfer */
    _tftp_action = action;

    /* we accept the transfer to take place so we return true */
    return true;
}

/**
 * @brief called to get or put data, depending on the mode received by `_tftp_start_cb(action, ...)`
 */
static int _tftp_server_data_cb(uint32_t offset, void *data, size_t data_len)
{
    char *c = (char *) data;

    /* if we are reading return the part of the data that is being requested */
    if (_tftp_action == TFTP_READ) {
        /* calculate the length of the data block to transfer */
        if (offset + data_len > sizeof(_tftp_server_hello)) {
            data_len -= (offset + data_len) - sizeof(_tftp_server_hello);
        }

        /* copy the block to the output buffer */
        memcpy(data, _tftp_server_hello + offset, data_len);
    }
    else {
        /* we received a data block which we output to the console */
        printf("\n -- SERVER DATA --\n%.*s\n -- SERVER DATA --\n", (int)data_len, c);
    }

    /* return the length of the data block */
    return data_len;
}

/**
 * @brief the transfer has stopped, see the event argument to determined if it was successful
 *        or not.
 */
static void _tftp_server_stop_cb(tftp_event_t event, const char *msg)
{
    /* decode the stop event received */
    const char *cause = "UNKOWN";

    if (event == TFTP_SUCCESS) {
        cause = "SUCCESS";
    }
    else if (event == TFTP_PEER_ERROR) {
        cause = "ERROR From Client";
    }
    else if (event == TFTP_INTERN_ERROR) {
        cause = "ERROR Internal Server Error";
    }

    /* print the transfer result to the console */
    printf("tftp_server: %s: %s\n", cause, (msg == NULL) ? "NULL" : msg);
}

/**
 * @brief the TFTP server thread
 */
void *tftp_server_wrapper(void *arg)
{
    (void)arg;

    /* A message queue is needed to register for incoming packets */
    msg_init_queue(_tftp_msg_queue, TFTP_QUEUE_SIZE);

    /* inform the user */
    puts("tftp_server: Starting TFTP service at port 69");

    /* run the TFTP server */
    gnrc_tftp_server(_tftp_server_data_cb, _tftp_server_start_cb, _tftp_server_stop_cb, true);

    /* the TFTP server has been stopped */
    puts("tftp_server: Stopped TFTP service");

    return NULL;
}

/**
 * @brief start the TFTP server by creating a thread
 */
void tftp_server_start(void)
{
    thread_create(_tftp_stack, sizeof(_tftp_stack),
                  1, THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                  tftp_server_wrapper, NULL, "TFTP Server");
}

/**
 * @brief stop the TFTP server by sending a message to the thread
 */
void tftp_server_stop(void)
{
    gnrc_tftp_server_stop();
}

int tftp_server_cmd(int argc, char * *argv)
{
    switch (argc) {
        case 2:
            if (strcmp(argv[1], "start") == 0) {
                tftp_server_start();
                return 0;
            }
            else if (strcmp(argv[1], "stop") == 0) {
                tftp_server_stop();
                return 0;
            }
        /* no break */

        default:
            printf("usage: %s [start|stop]\n", argv[0]);
            return 0;
    }

    return 0;
}
