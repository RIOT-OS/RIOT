/*
 * Copyright (C) 2019 Daniele Lacamera
 *
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
 * @brief       Example application  for DTLS 1.2 using wolfSSL
 *
 * @author      Daniele Lacamera <daniele@wolfssl.com>
 *
 * @}
 */

#include <wolfssl/ssl.h>

#include "shell.h"
#include "msg.h"
#include "log.h"

#ifdef WITH_RIOT_SOCKETS
#error RIOT-OS is set to use sockets but this DTLS app is configured for socks.
#endif

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int dtls_client(int argc, char **argv);
extern int dtls_server(int argc, char **argv);

#ifdef MODULE_WOLFCRYPT_TEST
extern int wolfcrypt_test(void* args);
static int wolftest(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    wolfcrypt_test(NULL);
    return 0;
}
SHELL_COMMAND(wolftest, "Perform wolfcrypt porting test", wolftest);
#endif

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    LOG(LOG_INFO, "RIOT wolfSSL DTLS testing implementation\n");
    wolfSSL_Init();
    wolfSSL_Debugging_ON();

    /* start shell */
    LOG(LOG_INFO, "All up, running the shell now\n");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
