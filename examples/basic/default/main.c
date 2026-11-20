/*
 * SPDX-FileCopyrightText: 2008, 2009, 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2013 INRIA
 * SPDX-FileCopyrightText: 2013 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Default application that shows a lot of functionality of RIOT
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"

int main(void)
{
#ifdef MODULE_GNRC_PKTDUMP
    static gnrc_netreg_entry_t dump;
    gnrc_netreg_entry_init_pid(&dump, GNRC_NETREG_DEMUX_CTX_ALL, gnrc_pktdump_pid);
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
#endif

    (void) puts("Welcome to RIOT!");

    return 0;
}
