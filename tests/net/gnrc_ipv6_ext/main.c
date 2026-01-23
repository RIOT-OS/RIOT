/*
 * SPDX-FileCopyrightText: 2015-2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Martine S. Lenders <m.lenders@fu-berlin.de>
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
    printf("Usage: %s {reg|unreg} <protnum>", cmd);
}

static int _ipreg(int argc, char **argv)
{
    if ((argc > 2) && (strcmp("reg", argv[1]) == 0)) {
        uint32_t protnum;
        if (ip_entry.target.pid != KERNEL_PID_UNDEF) {
            printf("Already registered to protnum %" PRIu32 "\n",
                   ip_entry.demux_ctx);
            return 1;
        }
        protnum = atoi(argv[2]);
        gnrc_netreg_entry_init_pid(&ip_entry, protnum, gnrc_pktdump_pid);
        gnrc_netreg_register(GNRC_NETTYPE_IPV6, &ip_entry);
        printf("Registered to protocol number %" PRIu32 "\n", protnum);
    }
    else if ((argc > 1) && (strcmp("unreg", argv[1]) == 0)) {
        printf("Unregistered from protocol number %" PRIu32 "\n",
               ip_entry.demux_ctx);
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
    { "ip", "Registers pktdump to a protocol number", _ipreg },
    { NULL, NULL, NULL }
};

int main(void)
{
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
