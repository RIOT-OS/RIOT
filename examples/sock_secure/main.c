/*
 * Copyright (C) 2018 Inria
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
 * @brief       Example application  for sock_secure
 *
 * @author      Raul Fuentes <>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"
#include "net/sock/secure.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#define MAIN_QUEUE_SIZE     (8)

extern int client_cmd(int argc, char **argv);
extern int server_cmd(int argc, char **argv);

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* TODO: Define them on the Makefile  */
#define SECURE_CIPHER_PSK_IDS (0xC0A8)
#define SECURE_CIPHER_RPK_IDS (0xC0AE)
#define SECURE_CIPHER_LIST { SECURE_CIPHER_PSK_IDS, SECURE_CIPHER_RPK_IDS }


static const shell_command_t shell_commands[] = {
    { "client", "Start the testing clientt", client_cmd},
    { "server", "Start the testing server", server_cmd},
    { NULL, NULL, NULL }
};

sock_secure_session_t secure_session = { .flag=0, .cb=NULL};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT sock_secure testing implementation");

    secure_session.flag = TLSMAN_FLAG_STACK_DTLS;

    uint16_t ciphers[] = SECURE_CIPHER_LIST;

    /* To be called only ONCE time */
    ssize_t res =  sock_secure_load_stack(&secure_session, ciphers, sizeof(ciphers));

    if (res) {
      puts("ERROR: Unable to load (D)TLS stack ");
      return -1;
    }

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
