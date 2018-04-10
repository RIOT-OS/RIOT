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

#include "net/gcoap.h"
#include "net/rdcli_common.h"

#define ENABLE_DEBUG (0)
#include "debug.h"


#ifdef RDCLI_EP
#define BUFSIZE         (sizeof(RDCLI_EP))  /* contains \0 termination char */
#else
#define PREFIX_LEN      (sizeof(RDCLI_EP_PREFIX))       /* contains \0 char */
#define BUFSIZE         (PREFIX_LEN + RDCLI_EP_SUFFIX_LEN)
#endif

char rdcli_ep[BUFSIZE];

void rdcli_common_init(void)
{
#ifdef RDCLI_EP
    memcpy(rdcli_ep, RDCLI_EP, BUFSIZE);
#else
    uint8_t luid[RDCLI_EP_SUFFIX_LEN / 2];

    if (PREFIX_LEN > 1) {
        memcpy(rdcli_ep, RDCLI_EP_PREFIX, (PREFIX_LEN - 1));
    }

    luid_get(luid, sizeof(luid));
    fmt_bytes_hex(&rdcli_ep[PREFIX_LEN - 1], luid, sizeof(luid));
    rdcli_ep[BUFSIZE - 1] = '\0';
#endif
}

int rdcli_common_add_qstring(coap_pkt_t *pkt)
{
    /* extend the url with some query string options */
    int res = gcoap_add_qstring(pkt, "ep", rdcli_ep);
    if (res < 0) {
        return res;
    }

    /* [optional] set the lifetime parameter */
#if RDCLI_LT
    char lt[11];
    lt[fmt_u32_dec(lt, RDCLI_LT)] = '\0';
    res = gcoap_add_qstring(pkt, "lt", lt);
    if (res < 0) {
        return res;
    }
#endif

    /* [optional] set the domain parameter */
#ifdef RDCLI_D
    res = gcoap_add_qstring(pkt, "d", RDCLI_D);
    if (res < 0) {
        return res;
    }
#endif

    return 0;
}
