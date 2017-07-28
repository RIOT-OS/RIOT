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
 * @brief       gcoap endpoint for ps stats
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "ps.h"
#include "net/gcoap.h"

ssize_t _gcoap_ps_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len)
{
    (void) buf;
    (void) len;
    (void) pdu;

    ps();
    return 0;
}

