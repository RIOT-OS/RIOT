/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       EDHOC handhshake over COAP using EDHOC-C
 *
 * @author      Timothy Claeys <timothy.claeys@inria.fr>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <stdio.h>

#include "net/nanocoap_sock.h"
#include "shell.h"
#include "thread.h"

#include "edhoc/edhoc.h"
#include "edhoc_keys.h"

#define MAIN_QUEUE_SIZE     (4)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#if IS_ACTIVE(CONFIG_RESPONDER)
static char _nanocoap_server_stack[THREAD_STACKSIZE_MAIN];
#define NANOCOAP_SERVER_QUEUE_SIZE     (4)
static msg_t _nanocoap_server_msg_queue[NANOCOAP_SERVER_QUEUE_SIZE];
#define NANOCOAP_BUF_SIZE   (512U)
extern int responder_cli_init(void);
extern int responder_cmd(int argc, char **argv);
#endif

#if IS_ACTIVE(CONFIG_INITIATOR)
extern int initiator_cmd(int argc, char **argv);
extern int initiator_cli_init(void);
#endif

static const shell_command_t shell_commands[] = {

#if IS_ACTIVE(CONFIG_INITIATOR)
    { "init", "EDHOC Initiator cli", initiator_cmd },
#endif
#if IS_ACTIVE(CONFIG_RESPONDER)
    { "resp", "EDHOC Responder cli", responder_cmd },
#endif
    { NULL, NULL, NULL }
};

#if IS_ACTIVE(CONFIG_RESPONDER)
static void *_nanocoap_server_thread(void *arg)
{
    (void)arg;

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_nanocoap_server_msg_queue, NANOCOAP_SERVER_QUEUE_SIZE);

    /* initialize nanocoap server instance */
    uint8_t buf[NANOCOAP_BUF_SIZE];
    sock_udp_ep_t local = { .port = COAP_PORT, .family = AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));

    return NULL;
}
#endif

int main(void)
{
#if IS_ACTIVE(CONFIG_INITIATOR)
    if (initiator_cli_init()) {
        return -1;
    }
#endif
#if IS_ACTIVE(CONFIG_RESPONDER)
    if (responder_cli_init()) {
        return -1;
    }

    /* start nanocoap server thread */
    thread_create(_nanocoap_server_stack, sizeof(_nanocoap_server_stack),
                  THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST,
                  _nanocoap_server_thread, NULL, "nanocoap server");
#endif

    /* the shell contains commands that receive packets via GNRC and thus
       needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Starting the shell");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
