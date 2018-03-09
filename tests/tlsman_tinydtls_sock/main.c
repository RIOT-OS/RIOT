/*
 * Copyright (C) 2018 Inria
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
 * @brief       tlsman test application
 *
 * Small test for TLSMAN. Many definitions defined here are also available at
 * sock_secure (and are intended to be used in standard applications)
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

 #include <stdio.h>

 #include "msg.h"
 #include "net/tlsman.h"

#define CALL(fn)            puts("Calling " # fn); fn;

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220 /* DTLS default port */
#endif

/* List of acceptable cipher suites (T) */
/* NOTE: For now, only CoAP Secure candidates (RFC 7252 9.1.3) */
#define SECURE_CIPHER_PSK_IDS (0xC0A8)
#define SECURE_CIPHER_RPK_IDS (0xC0AE)
#define SECURE_CIPHER_LIST { SECURE_CIPHER_PSK_IDS, SECURE_CIPHER_RPK_IDS }

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern void client_side(void);

static void test_load_stack(void)
{

    /* The Cipher(s) the application must use (Hardcoded) */
    int cipers[] = SECURE_CIPHER_LIST;

    assert(0 ==
           tlsman_load_stack(cipers, sizeof(cipers), TLSMAN_FLAG_STACK_DTLS));

}

int main(void)
{


    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("TLSMAN Module testing implementation");

    /*
     * First step: Init the (D)TLS stack. Either server or client
     * NOTE: This must be called once time only.
     */
    CALL(test_load_stack());

    client_side();

    return 0;
}
