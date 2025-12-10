/*
 * SPDX-FileCopyrightText: 2020 HAW
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Compile test for gnrc_rpl
 *
 * @author      Cenk Gündoğan <mail+dev@gundogan.net>
 *
 * @}
 */

#include "shell.h"
#include "msg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
