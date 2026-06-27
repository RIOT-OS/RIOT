/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption definition of the "eui64" bsp module
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include "string.h"
#include "eui64.h"
#include "openwsn_radio.h"

#include "luid.h"
#include "net/netopt.h"
#include "net/ieee802154.h"

extern openwsn_radio_t openwsn_radio;

static eui64_t _eui64;
static bool _eui64_is_set = false;

void eui64_get(uint8_t *addressToWrite)
{
    if (!_eui64_is_set) {
        luid_get_eui64(&_eui64);
        _eui64_is_set = true;
    }
    memcpy(addressToWrite, _eui64.uint8, sizeof(_eui64.uint8));
}
