/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_common
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
#include "net/cord/common.h"

#define ENABLE_DEBUG 0
#include "debug.h"


#ifdef CONFIG_CORD_EP
#define BUFSIZE         (sizeof(CONFIG_CORD_EP))   /* contains \0 termination char */
#else
#define PREFIX_LEN      (sizeof(CORD_EP_PREFIX))        /* contains \0 char */
#define BUFSIZE         (PREFIX_LEN + CORD_EP_SUFFIX_LEN)
#endif

char cord_common_ep[BUFSIZE];

void cord_common_init(void)
{
#ifdef CONFIG_CORD_EP
    memcpy(cord_common_ep, CONFIG_CORD_EP, BUFSIZE);
#else
    uint8_t luid[CORD_EP_SUFFIX_LEN / 2];

    if (PREFIX_LEN > 1) {
        memcpy(cord_common_ep, CORD_EP_PREFIX, (PREFIX_LEN - 1));
    }

    luid_get(luid, sizeof(luid));
    fmt_bytes_hex(&cord_common_ep[PREFIX_LEN - 1], luid, sizeof(luid));
    cord_common_ep[BUFSIZE - 1] = '\0';
#endif
}

int cord_common_add_qstring(coap_pkt_t *pkt)
{
    /* extend the url with some query string options */
    int res = coap_opt_add_uri_query(pkt, "ep", cord_common_ep);
    if (res < 0) {
        return res;
    }

    /* [optional] set the lifetime parameter */
#if CONFIG_CORD_LT
    char lt[11];
    lt[fmt_u32_dec(lt, CONFIG_CORD_LT)] = '\0';
    res = coap_opt_add_uri_query(pkt, "lt", lt);
    if (res < 0) {
        return res;
    }
#endif

    /* [optional] set the domain parameter */
#ifdef CORD_D
    res = coap_opt_add_uri_query(pkt, "d", CORD_D);
    if (res < 0) {
        return res;
    }
#endif

    return 0;
}
