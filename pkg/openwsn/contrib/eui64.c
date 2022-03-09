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
#include "net/netdev.h"
#include "net/netopt.h"
#include "net/ieee802154.h"

extern openwsn_radio_t openwsn_radio;

#ifdef MODULE_OPENWSN_RADIO_HAL
/* HACK: temporary hack while eui_provider still depends on netdev */
static eui64_t _eui64;
static bool _eui64_is_set = false;
#endif

void eui64_get(uint8_t *addressToWrite)
{
#ifdef MODULE_OPENWSN_RADIO_HAL
    if (!_eui64_is_set) {
        luid_get_eui64(&_eui64);
        _eui64_is_set = true;
    }
    memcpy(addressToWrite, _eui64.uint8, sizeof(_eui64.uint8));
#elif MODULE_OPENWSN_RADIO_NETDEV
    eui64_t eui64;

    if (openwsn_radio.dev->driver->get(openwsn_radio.dev, NETOPT_ADDRESS_LONG,
                                       &eui64,
                                       sizeof(eui64_t)) == sizeof(eui64_t)) {
        memcpy(addressToWrite, eui64.uint8, sizeof(eui64.uint8));
    }
    else {
        luid_get_eui64((eui64_t *) addressToWrite);
    }
#else
    memset(addressToWrite, 0, sizeof(eui64_t));
#endif
}
