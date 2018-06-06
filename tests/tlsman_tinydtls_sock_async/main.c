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

#define ENABLE_DEBUG (0)
#include "debug.h"

/* List of acceptable cipher suites (T) */
/* NOTE: For now, only CoAP Secure candidates (RFC 7252 9.1.3) */
#define SECURE_CIPHER_PSK_IDS (0xC0A8)
#define SECURE_CIPHER_RPK_IDS (0xC0AE)
#define SECURE_CIPHER_LIST { SECURE_CIPHER_PSK_IDS, SECURE_CIPHER_RPK_IDS }

#define MAIN_QUEUE_SIZE     (8)

extern char client_thread_stack[THREAD_STACKSIZE_MAIN +
                                THREAD_EXTRA_STACKSIZE_PRINTF];
extern void * client_wrapper(void *arg);

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
static kernel_pid_t _client_pid = KERNEL_PID_UNDEF;
tlsman_driver_t tlsman_session;

/* Must match IPH_MSG_* entries! */
#define TOTAL_TESTS_EVENTS (11U)
#define IPH_STARTING_TYPE 0x4000


static void test_load_stack(uint8_t tlsman_flags)
{

    /* The Cipher(s) the application must use (Hardcoded) */
    int cipers[] = SECURE_CIPHER_LIST;

     assert (!tlsman_load_stack(&tlsman_session, cipers, sizeof(cipers),
                                       tlsman_flags));

}

int main(void)
{

    uint8_t tlsman_flags = TLSMAN_FLAG_SIDE_CLIENT     |
                           TLSMAN_FLAG_STACK_DTLS      |
                           TLSMAN_FLAG_NONBLOCKING;
    uint8_t test_event;
    msg_t msg;

    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("TLSMAN Module testing implementation");

    /*
     * First step: Init the (D)TLS stack. Either server or client
     * NOTE: This must be called once time only.
     */
    CALL(test_load_stack(tlsman_flags));

    _client_pid = thread_create(client_thread_stack,
                                     sizeof(client_thread_stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST,
                                     client_wrapper, (void *)&tlsman_flags,
                                     "DTLS_Client");

    assert(!((_client_pid == EINVAL)|| (_client_pid == EOVERFLOW)));

    for (test_event =0; test_event < TOTAL_TESTS_EVENTS; test_event++){
      msg.type = IPH_STARTING_TYPE + test_event;
      DEBUG("Advancing on the test (%04X)\n", msg.type);
      msg_send_receive(&msg, &msg, _client_pid);
      /* give time to the remote peer to answer back (mostly for real data ) */
      xtimer_usleep(777);
    } /* for */

    return 0;
}
