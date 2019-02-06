/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup sys_auto_init_usb
 * @{
 *
 * @file
 * @brief   Auto initialization for atsam USB interface
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 */

#ifdef MODULE_SAM_USB
#include "sam_usb.h"

static sam0_common_usb_t usbdev;

usbdev_t *get_context_sam(void)
{
    sam_usbdev_setup(&usbdev);
    return &usbdev.usbdev;
}

#endif /* MODULE_SAM_USB */
