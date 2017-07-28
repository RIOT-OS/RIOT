/*
 * Copyright (C) 2017  Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     gcoap_sys_uri
 * @{
 *
 * @file
 * @brief       Provides prototypes and sets up sys-like gcoap uris
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdlib.h>
#include "net/gcoap.h"

extern ssize_t _gcoap_reboot_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len);

#ifdef MODULE_PS
extern ssize_t _gcoap_ps_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len);
#endif

/* Keep this array ordered by path to adhere to gcoap requirements */
static const coap_resource_t _sys_resources[] = {
  { "/sys/reboot", COAP_POST, _gcoap_reboot_handler },
#ifdef MODULE_PS
  { "/sys/ps", COAP_GET, _gcoap_ps_handler },
#endif
};

static gcoap_listener_t _sys_listener = {
    (coap_resource_t *)&_sys_resources[0],
    sizeof(_sys_resources) / sizeof(_sys_resources[0]),
    NULL
};


void gcoap_sys_register(void)
{
    gcoap_register_listener(&_sys_listener);
}
