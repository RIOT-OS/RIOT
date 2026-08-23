/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc/pktbuf.h"
#include "shell.h"

static int _gnrc_pktbuf_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    gnrc_pktbuf_stats();
    return 0;
}

SHELL_COMMAND(pktbuf, "prints internal stats of the packet buffer", _gnrc_pktbuf_cmd);

/** @} */
