/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Tests for sixlowpan_iphc_cbuf module
 *
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */
#include <stdio.h>

#include "vtimer.h"

#include "sixlowpan/iphc_cbuf.h"

int main(void)
{
    sixlowpan_iphc_cbuf_t *c;

    vtimer_init();
    puts("Add context with 1 minute lifetime");
    c = sixlowpan_iphc_cbuf_update(RAND_CID, RAND_IP, RAND_IP_LEN, 1);

    if (c == NULL) {
        puts("Adding the context failed");
        return 1;
    }

    c = sixlowpan_iphc_cbuf_lookup_cid(RAND_CID);

    if (c == NULL) {
        puts("Checking the context failed");
        return 1;
    }

    puts("Now wait 1.5 minutes");
    vtimer_usleep(1500000 * 60);
    c = sixlowpan_iphc_cbuf_lookup_cid(RAND_CID);

    puts("Context should not be available anymore");

    if (c == NULL) {
        puts("SUCCESS");
    }
    else {
        puts("Context did not time out");
        return 1;
    }

    return 0;
}
