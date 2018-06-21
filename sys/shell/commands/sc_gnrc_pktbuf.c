/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include "net/gnrc/pktbuf.h"

int _gnrc_pktbuf_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    gnrc_pktbuf_stats();
    return 0;
}

/** @} */
