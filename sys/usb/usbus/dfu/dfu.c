/*
 * Copyright (C) 2020 Dylan Laduranty
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usb_dfu
 * @{
 * @file USBUS implementation for device firmware upgrade
 *
 * @note This feature is currently experimental as it needs a proper riotboot
 * or another bootloader implementation to be fully functional
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
#include "xtimer.h"

#include "periph/flashpage.h"
#include "periph/pm.h"

#include <string.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_event_usb_t event);
static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup);
static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event);
static void _init(usbus_t *usbus, usbus_handler_t *handler);

static usbus_dfu_state_t dfu_state;
static uint32_t offset = 0;
static uint8_t page_mem[FLASHPAGE_SIZE];
/* TODO: properly integrate this part to riotboot */
static usbus_dfu_device_t dfu2;
static char _stack[USBUS_STACKSIZE];
static usbus_t usbus2;

static size_t _gen_dfu_descriptor(usbus_t *usbus, void *arg)
{
    usb_desc_if_dfu_t if_desc;
    (void) arg;

    /* functional dfu descriptor */
    if_desc.length = sizeof(usb_desc_if_dfu_t);
    if_desc.type = USB_IF_DESCRIPTOR_DFU;
    if_desc.attribute = USB_DFU_WILL_DETACH | USB_DFU_CAN_DOWNLOAD;
    if_desc.detach_timeout = USB_DFU_DETACH_TIMEOUT_MS;
    if_desc.xfer_size = USB_DFU_TRANSFER_SIZE;
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
        .fixed_len = sizeof(usb_desc_if_dfu_t),
    },
    .len_type = USBUS_DESCR_LEN_FIXED,
};

static void _flashpage(void) {
    static int page_cpt = 0;
    /* reset offset value for next transfer */
    offset = 0;
    /* Erase page content */
    flashpage_write(page_cpt, NULL);
    /* Write buffer into flash */
    int ret = flashpage_write_and_verify(page_cpt, page_mem);
    page_cpt++;
    if (ret != FLASHPAGE_OK) {
         /* TODO: Set DFU status variable accordingly */
         return;
    }
    /* Clear buffer for next transfer */
    memset(page_mem, 0, sizeof(page_mem));
}
void usbus_dfu_init(usbus_t *usbus, usbus_dfu_device_t *handler, usbus_dfu_startup_mode_t mode)
{
    DEBUG_PUTS("DFU: initializations");
    assert(usbus);
    assert(handler);
    memset(handler, 0, sizeof(usbus_dfu_device_t));
    handler->usbus = usbus;
    handler->handler_ctrl.driver = &dfu_driver;
    handler->mode = mode;
    dfu_state = (handler->mode) ? USBUS_DFU_STATE_DFU_IDLE : USBUS_DFU_STATE_APP_IDLE;
    usbus_register_event_handler(usbus, (usbus_handler_t *)handler);
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    usbus_dfu_device_t *dfu = (usbus_dfu_device_t*) handler;

    /* Set up descriptor generators */
    dfu->dfu_descr.next = NULL;
    dfu->dfu_descr.funcs = &_dfu_descriptor;
    dfu->dfu_descr.arg = dfu;

    /* Configure Interface 0 as control interface */
    dfu->iface.class = USB_DFU_INTERFACE;
    dfu->iface.subclass = USB_DFU_SUBCLASS_DFU;

    dfu->iface.protocol = ((dfu->mode) ? USB_DFU_PROTOCOL_DFU_MODE :
                                USB_DFU_PROTOCOL_RUNTIME_MODE);

    dfu->iface.descr_gen = &dfu->dfu_descr;
    dfu->iface.handler = handler;
    dfu->iface.alts = NULL;

    /* Add interfaces to the stack */
    usbus_add_interface(usbus, &dfu->iface);
    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);
}

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_control_request_state_t state,
                          usb_setup_t *setup)
{
    (void)usbus;
    (void)state;
    (void)handler;
    (void)setup;

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

void dfu_control_req(usbus_t *usbus, usbus_control_handler_t *handler)
{
    usb_setup_t *pkt = &handler->setup;
    usbopt_enable_t enable = USBOPT_DISABLE;

    DEBUG("DFU control request:%d\n", pkt->request);
    switch(pkt->request) {
        case DFU_DETACH:
            /* Detach USB bus */
            usbdev_set(usbus->dev, USBOPT_ATTACH, &enable, sizeof(usbopt_enable_t));
            /* EXPERIMENTAL: Start a new usbus instance with a single interface */
            /* TODO: properly integrate this part to riotboot */
            usbus_init(&usbus2, usbdev_get_ctx(0));
            usbus_dfu_init(&usbus2, &dfu2, USBUS_DFU_MODE);
            usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus2);
            break;
        case DFU_DOWNLOAD:
            /* Host indicates end of firmware download */
            if (pkt->length == 0) {
                /* Set DFU to manifest sync */
                dfu_state = USBUS_DFU_STATE_DFU_MANIFEST_SYNC;
                /* Flash pending buffer */
                if (offset) {
                    _flashpage();
                }
            }
            else if (dfu_state != USBUS_DFU_STATE_DFU_DL_SYNC) {
                dfu_state = USBUS_DFU_STATE_DFU_DL_SYNC;
            }
            else {
                /* Retrieve firmware data */
                size_t len = 0;
                usbdev_ep_get(handler->out, USBOPT_EP_AVAILABLE,
                              &len, sizeof(size_t));

                memcpy(&page_mem[handler->received_len + offset - len],
                       handler->out->buf, len);

                if (handler->received_len == pkt->length) {
                    /* Received a whole page, flash it */
                    if (handler->received_len + offset == FLASHPAGE_SIZE) {
                        /* DFU 1.1 doesn't support firmware addressing */
                        _flashpage();
                    }
                    else if (handler->received_len + offset < FLASHPAGE_SIZE){
                        /* If the transferred block is smaller than a flashpage
                           size, then we wait a new block to fill the buffer.
                           Block size is determined by the host despite the
                           given value during USB enumeration */
                           offset += handler->received_len;
                    }
                }
            }
            break;
        case DFU_GET_STATUS:
        {
            dfu_get_status_pkt_t buf;
            if (dfu_state == USBUS_DFU_STATE_DFU_DL_SYNC) {
                dfu_state = USBUS_DFU_STATE_DFU_DL_IDLE;
            }
            else if (dfu_state == USBUS_DFU_STATE_DFU_MANIFEST_SYNC) {
                pm_reboot();
            }

            memset(&buf, 0, sizeof(buf));
            buf.status = 0;
            buf.timeout = USB_DFU_DETACH_TIMEOUT_MS;
            buf.state = dfu_state;
            /* Send answer to host */
            usbus_control_slicer_put_bytes(usbus, (uint8_t*)&buf, sizeof(buf));
            break;
        }
        case DFU_CLR_STATUS:
            DEBUG("CLRSTATUS To be implemented\n");
            break;
        default:
            DEBUG("Unhandled DFU control request:%d\n", pkt->request);
    }
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
