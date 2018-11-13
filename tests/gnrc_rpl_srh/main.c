/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Tests extension header handling of gnrc stack.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/netreg.h"

static char line_buf[SHELL_DEFAULT_BUFSIZE];
static gnrc_netreg_entry_t ip_entry = GNRC_NETREG_ENTRY_INIT_PID(
        0, KERNEL_PID_UNDEF
    );

static inline void _ipreg_usage(char *cmd)
{
    printf("Usage: %s {reg|unreg}", cmd);
}

static int _ipreg(int argc, char **argv)
{
    if (argc < 2) {
        _ipreg_usage(argv[0]);
        return 1;
    }
    else if (strcmp("reg", argv[1]) == 0) {
        if (ip_entry.target.pid != KERNEL_PID_UNDEF) {
            puts("Already registered to protocol number 59");
            return 1;
        }
        gnrc_netreg_entry_init_pid(&ip_entry, PROTNUM_IPV6_NONXT,
                                   gnrc_pktdump_pid);
        gnrc_netreg_register(GNRC_NETTYPE_IPV6, &ip_entry);
        puts("Registered to protocol number 59");
    }
    else if (strcmp("unreg", argv[1]) == 0) {
        puts("Unregistered from protocol number 59");
        gnrc_netreg_unregister(GNRC_NETTYPE_IPV6, &ip_entry);
        gnrc_netreg_entry_init_pid(&ip_entry, 0, KERNEL_PID_UNDEF);
    }
    else {
        _ipreg_usage(argv[0]);
        return 1;
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "ip", "Registers pktdump to protocol number 59 (no next header)", _ipreg },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
