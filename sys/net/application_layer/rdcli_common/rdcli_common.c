/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_rdcli_common
 * @{
 *
 * @file
 * @brief       Implementation of common functions for CoRE RD clients
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "fmt.h"
#include "luid.h"

#include "net/rdcli_common.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#ifdef RDCLI_EP
#define EP_LEN          (sizeof(RDCLI_EP))
#define BUFSIZE         (EP_LEN + 1)
#else
#define PREFIX_LEN      (sizeof(RDCLI_EP_PREFIX))
#define BUFSIZE         (PREFIX_LEN + RDCLI_EP_SUFFIX_LEN + 1)
#endif

char rdcli_ep[BUFSIZE];

void rdcli_common_init(void)
{
    size_t pos = 0;

#ifdef RDCLI_EP
    memcpy(rdcli_ep, RDCLI_EP, EP_LEN);
    pos += EP_LEN;
#else
    uint8_t luid[RDCLI_EP_SUFFIX_LEN / 2];

    if (PREFIX_LEN) {
        memcpy(rdcli_ep, RDCLI_EP_PREFIX, PREFIX_LEN);
        pos += PREFIX_LEN - 1;
    }

    luid_get(luid, sizeof(luid));
    fmt_bytes_hex(&rdcli_ep[pos], luid, sizeof(luid));
#endif

    rdcli_ep[pos] = '\0';
}
