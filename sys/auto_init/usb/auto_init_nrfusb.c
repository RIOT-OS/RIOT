/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup sys_auto_init_usb
 * @{
 *
 * @file
 * @brief   Auto initialization for nordic nrf52 USB interface
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 */

#ifdef MODULE_NRFUSB
#include "nrfusb.h"

static nrfusb_t usbdev;

usbdev_t *get_context_nrfusb(void)
{
    nrf_usbdev_setup(&usbdev);
    return &usbdev.usbdev;
}

#endif /* MODULE_NRFUSB */

