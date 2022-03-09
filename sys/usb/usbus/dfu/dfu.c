/*
 * Copyright (C) 2020 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usbus_dfu
 * @{
 * @file USBUS implementation for device firmware upgrade
 *
 *
 * @author  Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#define USB_H_USER_IS_RIOT_INTERNAL

#include "usb/dfu.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/control.h"
#include "usb/usbus/dfu.h"
#include "riotboot/magic.h"
#include "riotboot/usb_dfu.h"
#ifdef MODULE_RIOTBOOT_USB_DFU
#include "ztimer.h"
#endif
#include "periph/pm.h"

#include "riotboot/slot.h"

#include <string.h>

#define ENABLE_DEBUG    0
#include "debug.h"

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_event_usb_t event);
static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup);
static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event);
static void _init(usbus_t *usbus, usbus_handler_t *handler);

#ifdef MODULE_RIOTBOOT_USB_DFU
static void _reboot(void *arg);
static ztimer_t scheduled_reboot = { .callback=_reboot };
#define REBOOT_DELAY 2
#endif

#define DEFAULT_XFER_SIZE 64

static size_t _gen_dfu_descriptor(usbus_t *usbus, void *arg)
{
    (void)arg;
    usb_desc_if_dfu_t if_desc;

    /* functional dfu descriptor */
    if_desc.length = sizeof(usb_desc_if_dfu_t);
    if_desc.type = USB_IF_DESCRIPTOR_DFU;
    if_desc.attribute = USB_DFU_WILL_DETACH | USB_DFU_CAN_DOWNLOAD;
    if_desc.detach_timeout = USB_DFU_DETACH_TIMEOUT_MS;
    if_desc.xfer_size = DEFAULT_XFER_SIZE;
    if_desc.bcd_dfu = USB_DFU_VERSION_BCD;

    usbus_control_slicer_put_bytes(usbus, (uint8_t *)&if_desc, sizeof(if_desc));
    return sizeof(usb_desc_if_dfu_t);
}

static const usbus_handler_driver_t dfu_driver = {
    .init = _init,
    .event_handler = _event_handler,
    .transfer_handler = _transfer_handler,
    .control_handler = _control_handler,
};

/* Descriptors */
static const usbus_descr_gen_funcs_t _dfu_descriptor = {
    .fmt_post_descriptor = _gen_dfu_descriptor,
    .fmt_pre_descriptor = NULL,
    .len = {
        .fixed_len =  sizeof(usb_desc_if_dfu_t),
    },
    .len_type = USBUS_DESCR_LEN_FIXED,
};

#ifdef MODULE_RIOTBOOT_USB_DFU
static void _reboot(void *arg)
{
    (void)arg;
    pm_reboot();
}
#endif

void usbus_dfu_init(usbus_t *usbus, usbus_dfu_device_t *handler, unsigned mode)
{
    DEBUG("DFU: initialization\n");
    assert(usbus);
    assert(handler);
    assert((SLOT0_OFFSET % FLASHPAGE_SIZE) == 0);
    memset(handler, 0, sizeof(usbus_dfu_device_t));
    handler->usbus = usbus;
    handler->handler_ctrl.driver = &dfu_driver;
    handler->mode = mode;
    handler->selected_slot = UINT32_MAX;
    handler->skip_signature = true;
    handler->dfu_state = (handler->mode == USB_DFU_PROTOCOL_DFU_MODE) ?
                USB_DFU_STATE_DFU_IDLE : USB_DFU_STATE_APP_IDLE;

    usbus_register_event_handler(usbus, (usbus_handler_t *)handler);
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    usbus_dfu_device_t *dfu = container_of(handler, usbus_dfu_device_t, handler_ctrl);
    /* Set up descriptor generators */
    dfu->dfu_descr.next = NULL;
    dfu->dfu_descr.funcs = &_dfu_descriptor;
    dfu->dfu_descr.arg = dfu;

    /* Configure Interface 0 as control interface */
    dfu->iface.class = USB_DFU_INTERFACE;
    dfu->iface.subclass = USB_DFU_SUBCLASS_DFU;

    dfu->iface.protocol = dfu->mode;

    dfu->iface.descr_gen = &dfu->dfu_descr;
    dfu->iface.handler = handler;

    /* Create needed string descriptor for the interface and its alternate settings */
    if (IS_ACTIVE(MODULE_RIOTBOOT_USB_DFU)) {
        usbus_add_string_descriptor(usbus, &dfu->slot0_str, USB_DFU_MODE_SLOT0_NAME);
    }
    else {
        usbus_add_string_descriptor(usbus, &dfu->slot0_str, USB_APP_MODE_SLOT_NAME);
    }

    /* Add string descriptor to the interface */
    dfu->iface.descr = &dfu->slot0_str;

#ifdef MODULE_RIOTBOOT_USB_DFU
    /* Create needed string descriptor for the alternate settings */
    usbus_add_string_descriptor(usbus, &dfu->slot1_str, USB_DFU_MODE_SLOT1_NAME);

    /* Add string descriptor to the alternate settings */
    dfu->iface_alt_slot1.descr = &dfu->slot1_str;

    /* attached alternate settings to their interface */
    usbus_add_interface_alt(&dfu->iface, &dfu->iface_alt_slot1);
#endif
    /* Add interface to the stack */
    usbus_add_interface(usbus, &dfu->iface);
    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);
}

static int _dfu_class_control_req(usbus_t *usbus, usbus_dfu_device_t *dfu, usb_setup_t *pkt)
{
    static const usbopt_enable_t disable = USBOPT_DISABLE;
    DEBUG("DFU control request:%x\n", pkt->request);
    switch (pkt->request) {
        case DFU_DETACH:
            /* Detach USB bus */
            usbdev_set(usbus->dev, USBOPT_ATTACH, &disable, sizeof(usbopt_enable_t));
            /* Restart and jump into the bootloader */
            uint32_t *reset_addr = (uint32_t *)RIOTBOOT_MAGIC_ADDR;
            *reset_addr = RIOTBOOT_MAGIC_NUMBER;
            pm_reboot();
            break;
#ifdef MODULE_RIOTBOOT_USB_DFU
        case DFU_DOWNLOAD:
            /* Host indicates end of firmware download */
            if (pkt->length == 0) {
                /* Set DFU to manifest sync */
                dfu->dfu_state = USB_DFU_STATE_DFU_MANIFEST_SYNC;
                riotboot_flashwrite_flush(&dfu->writer);
                riotboot_flashwrite_finish(&dfu->writer);
            }
            else if (dfu->dfu_state != USB_DFU_STATE_DFU_DL_SYNC) {
                dfu->dfu_state = USB_DFU_STATE_DFU_DL_SYNC;
            }
            else {
                /* Retrieve firmware data */
                size_t len = 0;
                int ret = 0;
                uint8_t *data = usbus_control_get_out_data(usbus, &len);
                 /* skip writing the riotboot signature */
                if (dfu->skip_signature) {
                    /* Avoid underflow condition */
                    if (len < RIOTBOOT_FLASHWRITE_SKIPLEN) {
                        dfu->dfu_state = USB_DFU_STATE_DFU_ERROR;
                        return -1;
                    }
                    riotboot_flashwrite_init(&dfu->writer, dfu->selected_slot);
                    len -= RIOTBOOT_FLASHWRITE_SKIPLEN;
                    dfu->skip_signature = false;
                    ret = riotboot_flashwrite_putbytes(&dfu->writer,
                                                 &data[RIOTBOOT_FLASHWRITE_SKIPLEN],
                                                 len, true);
                }
                else {
                    ret = riotboot_flashwrite_putbytes(&dfu->writer, data, len, true);
                }
                if (ret < 0) {
                    /* Error occurs, stall the current transfer */
                    dfu->dfu_state = USB_DFU_STATE_DFU_ERROR;
                    return -1;
                }
            }
            break;
#endif
        case DFU_GET_STATUS:
        {
            dfu_get_status_pkt_t buf;

            if (dfu->dfu_state == USB_DFU_STATE_DFU_DL_SYNC) {
                dfu->dfu_state = USB_DFU_STATE_DFU_DL_IDLE;
                DEBUG("GET STATUS GO TO IDLE\n");
            }
            else if (dfu->dfu_state == USB_DFU_STATE_DFU_MANIFEST_SYNC) {
                /* Scheduled reboot, so we can answer back dfu-util before rebooting */
                dfu->dfu_state = USB_DFU_STATE_DFU_DL_IDLE;
#ifdef MODULE_RIOTBOOT_USB_DFU
                ztimer_set(ZTIMER_SEC, &scheduled_reboot, 1);
#endif
            }
            memset(&buf, 0, sizeof(buf));
            buf.status = 0;
            buf.timeout = USB_DFU_DETACH_TIMEOUT_MS;
            buf.state = dfu->dfu_state;
            /* Send answer to host */
            usbus_control_slicer_put_bytes(usbus, (uint8_t*)&buf, sizeof(buf));
            DEBUG("send answer\n");
            break;
        }
        case DFU_CLR_STATUS:
            if (dfu->dfu_state == USB_DFU_STATE_DFU_ERROR) {
                dfu->dfu_state = USB_DFU_STATE_DFU_IDLE;
            } else {
                DEBUG("CLRSTATUS: unhandled case");
            }
            break;
        default:
            DEBUG("Unhandled DFU control request:%d\n", pkt->request);
    }

    return 0;
}

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_control_request_state_t state,
                          usb_setup_t *setup)
{
    (void)usbus;
    (void)state;

    usbus_dfu_device_t *dfu = container_of(handler, usbus_dfu_device_t, handler_ctrl);
    DEBUG("DFU: Request: 0x%x\n", setup->request);

    /* Process DFU class request */
    if (setup->type & USB_SETUP_REQUEST_TYPE_CLASS) {
        if (_dfu_class_control_req(usbus, dfu, setup) < 0) {
            DEBUG("DFU: control request %u failed\n", setup->request);
            return -1;
        }
    }
    else {
        switch (setup->request) {
            case USB_SETUP_REQ_SET_INTERFACE:
                DEBUG("DFU: Select alt interface %d\n", setup->value);
                dfu->selected_slot = (unsigned)setup->value;
                break;

            default:
                return -1;
        }
    }
    return 1;
}

static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                             usbdev_ep_t *ep, usbus_event_transfer_t event)
{
    (void)event;
    (void)usbus;
    (void)handler;
    (void)ep;
}

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_event_usb_t event)
{
    (void) usbus;
    (void) handler;
    switch (event) {
        default:
            DEBUG("Unhandled event :0x%x\n", event);
            break;
    }
}
