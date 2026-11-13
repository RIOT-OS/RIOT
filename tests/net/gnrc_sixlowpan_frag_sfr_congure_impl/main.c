/*
 * SPDX-FileCopyrightText: 2015-2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    return 0;
}
