/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for gnrc_tapnet network device driver
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "net/gnrc.h"
#include "net/gnrc/pktdump.h"

/**
 * @brief   Maybe you are a golfer?!
 */
int main(void)
{
    puts("SLIP test");

    /* register pktdump */
    if (gnrc_pktdump_pid <= KERNEL_PID_UNDEF) {
        puts("Error starting pktdump thread");
        return -1;
    }

    static gnrc_netreg_entry_t dump;
    gnrc_netreg_entry_init_pid(&dump, GNRC_NETREG_DEMUX_CTX_ALL, gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    puts("Initialization OK, starting shell now");
    /* shell starts implicitly after returning */
    return 0;
}
