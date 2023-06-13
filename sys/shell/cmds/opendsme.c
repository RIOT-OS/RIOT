/*
 * Copyright 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Shell command implementation for openDSME
 *
 * @author      José I. Álamos <jose.alamos@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "opendsme/opendsme.h"

#include "net/gnrc.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc/netreg.h"

#include "fmt.h"
#include "shell.h"

#if IS_ACTIVE(CONFIG_IEEE802154_DSME_STATIC_GTS)
static int _opendsme_gts_cmd(int argc, char **argv)
{
    (void) argc;
    ieee802154_dsme_alloc_t alloc;
    if (argc < 7) {
        printf("Usage: %s <iface> <target_addr> <tx> <superframe_id> <slot_id> <channel_id>\n",argv[0]);
        return 1;
    }
    memset(&alloc, 0, sizeof(alloc));
    kernel_pid_t iface = scn_u32_dec(argv[1],1);
    l2util_addr_from_str(argv[2], (uint8_t*) &alloc.addr);

    /* Whether slot is TX or RX */
    alloc.tx = scn_u32_dec(argv[3],1);

    /* Set the superframe ID of the current slot. Valid superframe IDs are
     * ({0..n-1}, with `n` the number of superframes per multisuperframe) */
    alloc.superframe_id = scn_u32_dec(argv[4],1);

    /* Set the slot ID. Valid slot IDs are:
     * {0..7} if superframe_id == 0,
     * {8..14} if (superframe_id != 0 && CONFIG_IEEE802154_DSME_CAP_REDUCTION=1)
     * {0..14} otherwise
     */
    alloc.slot_id = scn_u32_dec(argv[5],1);

    /* Set the channel offset. Valid values for O-QPSK are {0..15} */
    alloc.channel_id = scn_u32_dec(argv[6], 1);

    gnrc_netapi_set(iface, NETOPT_GTS_ALLOC, 0, &alloc, sizeof(alloc));

    return 0;
}

SHELL_COMMAND(gts, "Allocate a static GTS with a neighbour device", _opendsme_gts_cmd);
#else
typedef int dont_be_pedantic;
#endif
