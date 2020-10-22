/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys_auto_init
 * @{
 * @file
 * @brief       initializes USBUS, usb devices and handlers
 *
 * This auto initialization for USBUS is designed to cover the common use case
 * of a single USB peripheral. An USBUS instance is started with USB function
 * handlers based on which module is compiled in.
 *
 * If this doesn't suit your use case, a different initialization function can
 * to be created based on this initialization sequence.
 *
 * @author  Koen Zandberg <koen@bergzand.net>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include <assert.h>

#include "usb/usbus.h"

#ifdef MODULE_USBUS_CDC_ECM
#include "usb/usbus/cdc/ecm.h"
usbus_cdcecm_device_t cdcecm;
#endif
#ifdef MODULE_USBUS_CDC_ACM
#include "usb/usbus/cdc/acm.h"
#endif

static char _stack[USBUS_STACKSIZE];
static usbus_t usbus;

void auto_init_usb(void)
{
    /* Get driver context */
    usbdev_t *usbdev = usbdev_get_ctx(0);
    assert(usbdev);

    /* Initialize basic usbus struct, don't start the thread yet */
    usbus_init(&usbus, usbdev);

    /* USBUS function handlers initialization */
#ifdef MODULE_STDIO_CDC_ACM
    void usb_cdc_acm_stdio_init(usbus_t *usbus);
    usb_cdc_acm_stdio_init(&usbus);
#endif

#ifdef MODULE_USBUS_CDC_ECM
    usbus_cdcecm_init(&usbus, &cdcecm);
#endif

    /* Finally initialize USBUS thread */
    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);
}
