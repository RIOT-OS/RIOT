/*
 * SPDX-FileCopyrightText: 2020 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for network device drivers
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include "thread.h"
#include "shell.h"

#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

int main(void)
{
    /* enable pktdump output */
    static gnrc_netreg_entry_t dump;
    gnrc_netreg_entry_init_pid(&dump, GNRC_NETREG_DEMUX_CTX_ALL, gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);

    /* shell starts implicitly after returning */
    return 0;
}
