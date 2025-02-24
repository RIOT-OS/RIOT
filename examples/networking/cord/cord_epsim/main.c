/*
 * Copyright (C) 2017-2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Test application demonstrating the simple registration
 *              process to a CoRE RD using gcoap and cord_epsim
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "net/gcoap.h"
#include "net/cord/epsim.h"
#include "net/cord/common.h"
#include "net/gnrc/netif.h"
#include "net/sock/util.h"
#include "net/ipv6/addr.h"
#include "xtimer.h"

#define BUFSIZE             (64U)
#define STARTUP_DELAY       (3U)    /* wait 3s before sending first request*/

static char riot_info[BUFSIZE];

/* define some dummy CoAP resources */
static ssize_t text_resp(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                         const char *text, unsigned format)
{
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, format);
    ssize_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
    size_t slen = strlen(text);
    memcpy(pdu->payload, text, slen);
    return resp_len + slen;
}

static ssize_t handler_info(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)ctx;
    return text_resp(pdu, buf, len, riot_info, COAP_FORMAT_JSON);
}

static ssize_t handler_text(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    return text_resp(pdu, buf, len, coap_request_ctx_get_context(ctx), COAP_FORMAT_TEXT);
}

static const coap_resource_t resources[] = {
    { "/riot/bar",  COAP_GET, handler_text, "foo" },
    { "/riot/foo",  COAP_GET, handler_text, "bar" },
    { "/riot/info", COAP_GET, handler_info, NULL }
};

static gcoap_listener_t listener = {
    .resources     = &resources[0],
    .resources_len = ARRAY_SIZE(resources),
    .next          = NULL
};

int main(void)
{
    char ep_str[CONFIG_SOCK_URLPATH_MAXLEN];
    uint16_t ep_port;

    puts("Simplified CoRE RD registration example\n");

    /* fill riot info */
    sprintf(riot_info, "{\"ep\":\"%s\",\"lt\":%u}",
            cord_common_get_ep(), CONFIG_CORD_LT);

    /* parse RD address information */
    sock_udp_ep_t rd_ep;

    if (sock_udp_name2ep(&rd_ep, RD_ADDR) < 0) {
        puts("error: unable to parse RD address from RD_ADDR variable");
        return 1;
    }

    /* if netif is not specified in addr and it's link local */
    if ((rd_ep.netif == SOCK_ADDR_ANY_NETIF) &&
         ipv6_addr_is_link_local((ipv6_addr_t *) &rd_ep.addr.ipv6)) {
        /* if there is only one interface we use that */
        if (gnrc_netif_numof() == 1) {
            rd_ep.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        /* if there are many it's an error */
        else {
            puts("error: must specify an interface for a link local address");
            return 1;
        }
    }

    if (rd_ep.port == 0) {
        rd_ep.port = COAP_PORT;
    }

    sock_udp_ep_fmt(&rd_ep, ep_str, &ep_port);

    /* register resource handlers with gcoap */
    gcoap_register_listener(&listener);

    /* print RD client information */
    puts("epsim configuration:");
    printf("         ep: %s\n", cord_common_get_ep());
    printf("         lt: %is\n", (int)CONFIG_CORD_LT);
    printf(" RD address: [%s]:%u\n\n", ep_str, ep_port);

    xtimer_sleep(STARTUP_DELAY);

    while (1) {
        int res = cord_epsim_state();
        switch (res) {
            case CORD_EPSIM_OK:
                puts("state: registration active");
                break;
            case CORD_EPSIM_BUSY:
                puts("state: registration in progress");
                break;
            case CORD_EPSIM_ERROR:
            default:
                puts("state: not registered");
                break;
        }

        printf("updating registration with RD [%s]:%u\n", ep_str, ep_port);
        res = cord_epsim_register(&rd_ep);
        if (res == CORD_EPSIM_BUSY) {
            puts("warning: registration already in progress");
        }
        else if (res == CORD_EPSIM_ERROR) {
            puts("error: unable to trigger simple registration process");
        }
        xtimer_sleep(CONFIG_CORD_UPDATE_INTERVAL);
    }

    return 0;
}
