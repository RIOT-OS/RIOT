/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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
