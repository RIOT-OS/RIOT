/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
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
 * @brief       Test application for gnrc_udp network device driver
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/gnrc/nettest.h"
#include "embUnit.h"
#include "msg.h"

#include "udp-netapi-tests.h"

#define MAIN_MSG_QUEUE_SIZE (1) /* UDP does not fragment so this should suffice */

static msg_t msg_queue[MAIN_MSG_QUEUE_SIZE];

int main(void)
{
    if (gnrc_nettest_init() < 0) {
        puts("Error initializing nettest thread");
        return 1;
    }

    if (udp_netapi_tests_init() < 0) {
        return 1;
    }

    msg_init_queue(msg_queue, MAIN_MSG_QUEUE_SIZE);

    TESTS_START();
    TESTS_RUN(udp_netapi_tests());
    TESTS_END();

    return 0;
}
