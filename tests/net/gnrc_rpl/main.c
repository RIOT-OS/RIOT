/*
 * Copyright (C) 2020 HAW
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
