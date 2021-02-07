/*
 * Copyright (C) 2019-2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usbus_msc
 * @{
 *
 * @author  Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include "usb/descriptor.h"
#include "usb/usbus.h"
#include "usb/usbus/control.h"

#include "usb/msc.h"
#include "usb/usbus/msc.h"
#include "usb/usbus/msc/scsi.h"
#include "board.h"

#include <string.h>

#define ENABLE_DEBUG    0
#include "debug.h"

#include "mtd.h"
extern mtd_dev_t *mtd0;

/* Internal buffer to handle size difference between MTD layer and USB
   endpoint size as some MTD implementation (like mtd_sdcard) doesn't allow
   endpoint size transfer */
static unsigned char buff[USBUS_MSC_BUFFER_SIZE];

/* Internal handler definitions */
static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                           usbus_event_usb_t event);

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup);

static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event);

static void _init(usbus_t *usbus, usbus_handler_t *handler);

static const usbus_handler_driver_t msc_driver = {
    .init = _init,
    .event_handler = _event_handler,
    .control_handler = _control_handler,
    .transfer_handler = _transfer_handler,
};

 static const usbus_descr_gen_funcs_t _msc_descriptor = {
    .fmt_post_descriptor = NULL,
    .fmt_pre_descriptor = NULL,
    .len = {
        .fixed_len =  0,
    },
    .len_type = USBUS_DESCR_LEN_FIXED,
};

static void _write_xfer(usbus_msc_device_t *msc) {
/* Check if we have a block to read and transfer */
    if (msc->block_nb) {
        size_t len;
        /* Retrieve incoming data */
        usbdev_ep_get(msc->ep_out->ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
        if (len > 0) {
            /* Prepare page buffer */
            memcpy(&msc->buffer[msc->block_offset], msc->ep_out->ep->buf, len);
        }
        usbdev_ep_ready(msc->ep_out->ep, 0);
        /* Update offset for page buffer */
        msc->block_offset += len;
        /* Decrement whole len */
        msc->cmd.len -= len;

        /* buffer is full, write it and point to new block if any */
        if (msc->block_offset >= (mtd0->page_size * msc->pages_per_vpage)) {
            mtd_write_page(mtd0, msc->buffer, msc->block * msc->pages_per_vpage,
                           0, mtd0->page_size * msc->pages_per_vpage);
            msc->block_offset = 0;
            msc->block++;
            msc->block_nb--;
        }
        if (msc->cmd.len == 0) {
            /* All blocks have been transferred, send CSW to host */
            if (msc->state == DATA_TRANSFER) {
                msc->state = GEN_CSW;
            }
        }
    }
}
static void _xfer_data( usbus_msc_device_t *msc)
{
    /* Check if we have a block to read and transfer */
    if (msc->block_nb) {
        /* read buffer from mtd device */
        if (msc->block_offset == 0) {
            mtd_read_page(mtd0, msc->buffer, msc->block * msc->pages_per_vpage,
                          0, mtd0->page_size * msc->pages_per_vpage);
        }
        /* Prepare endpoint buffer */
        memcpy(msc->ep_in->ep->buf, &msc->buffer[msc->block_offset], 64);
        /* Data prepared, signal ready to usbus */
        usbdev_ep_ready(msc->ep_in->ep, 64);
        /* Update offset for page buffer */
        msc->block_offset += 64;
        /* Decrement whole len */
        msc->cmd.len -= 64;
        /* whole buffer is empty, point to new block if any */
        if (msc->block_offset >= (mtd0->page_size * msc->pages_per_vpage)) {
            msc->block_offset = 0;
            msc->block++;
            msc->block_nb--;
        }
    }
    else {
        /* All blocks have been transferred, send CSW to host */
        if (msc->state == DATA_TRANSFER) {
            msc->state = GEN_CSW;
        }
    }
}

static void _handle_rx_event(event_t *ev)
{
    usbus_msc_device_t *msc = container_of(ev, usbus_msc_device_t, rx_event);
    _xfer_data(msc);
}

int usbus_msc_init(usbus_t *usbus, usbus_msc_device_t *handler)
{
    assert(usbus);
    assert(handler);
    memset(handler, 0, sizeof(usbus_msc_device_t));
    handler->usbus = usbus;
    handler->handler_ctrl.driver = &msc_driver;
    usbus_register_event_handler(usbus, (usbus_handler_t*)handler);
    printf("[MSC]: MTD init...");
    if (mtd_init(mtd0) != 0) {
        puts("[FAILED]");
        return -1;
    }
    puts("[OK]");
    return 0;
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    DEBUG("MSC: initialization\n");
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    msc->msc_descr.next = NULL;
    msc->msc_descr.funcs = &_msc_descriptor;
    msc->msc_descr.arg = msc;
    msc->block_offset = 0;
    msc->buffer = buff;
    msc->block_nb = 0;
    msc->block = 0;
    msc->state = WAITING;
     /* Add event handlers */
    msc->rx_event.handler = _handle_rx_event;
    /* Instantiate interfaces */
    memset(&msc->iface, 0, sizeof(usbus_interface_t));
    /* Configure Interface 0 as control interface */
    msc->iface.class = USB_CLASS_MASS_STORAGE;
    msc->iface.subclass = USB_MSC_SUBCLASS_SCSI_TCS;
    msc->iface.protocol = USB_MSC_PROTOCOL_BBB;
    msc->iface.descr_gen = &(msc->msc_descr);
    msc->iface.handler = handler;

    /* Create required endpoints */
    msc->ep_in = usbus_add_endpoint(usbus, &msc->iface, USB_EP_TYPE_BULK,
                                    USB_EP_DIR_IN, 64);
    msc->ep_in->interval = 0;
    msc->ep_out = usbus_add_endpoint(usbus, &msc->iface, USB_EP_TYPE_BULK,
                                     USB_EP_DIR_OUT, 64);
    msc->ep_out->interval = 0;

    /* Add interfaces to the stack */
    usbus_add_interface(usbus, &msc->iface);

    usbdev_ep_ready(msc->ep_in->ep, 0);
    usbdev_ep_ready(msc->ep_out->ep, 0);

    usbus_enable_endpoint(msc->ep_in);
    usbus_enable_endpoint(msc->ep_out);

    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);
}

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup)
{
    (void)usbus;
    (void)state;

    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    static usbopt_enable_t enable = USBOPT_ENABLE;

    switch (setup->request) {
        case USB_MSC_SETUP_REQ_GML:
            /* Stall as we don't support this feature */
            usbdev_ep_set(msc->ep_in->ep, USBOPT_EP_STALL, &enable,
                          sizeof(usbopt_enable_t));
            break;
        case USB_MSC_SETUP_REQ_BOMSR:
            DEBUG("TODO: implement reset setup request\n");
            break;
        default:
            DEBUG("default handle setup rqt:0x%x\n", setup->request);
            return -1;
    }
    return 1;
}

static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event)
{
       (void)usbus;
       (void)event;

    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    if (ep->dir == USB_EP_DIR_OUT) {
        size_t len;
        /* Previous transfer is sent, send the next one */
        if (msc->state == DATA_TRANSFER) {
            _write_xfer(msc);
        }
        /* USB_EP_DIR_IN */
        else {
            /* Retrieve incoming data */
            usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
            if (len > 0) {
                /* Process incoming endpoint buffer */
                scsi_process_cmd(usbus, handler, ep, len);
            }
            usbdev_ep_ready(ep, 0);
        }
    }
    else {
        /* Data Transfer pending */
        if (msc->state == DATA_TRANSFER) {
            _xfer_data(msc);
        }
        /* CBW answer was sent, prepare CSW */
        else if (msc->state == WAIT_FOR_TRANSFER) {
            msc->state = GEN_CSW;
        }
    }

    if (msc->cmd.tag && msc->cmd.len == 0 && msc->state == GEN_CSW) {
       DEBUG_PUTS("[MSC]: Generate CSW");
        scsi_gen_csw(handler, msc->cmd);
        /* Command has been handled, so clear tag and state machine */
        msc->cmd.tag = 0;
        msc->state = WAITING;
    }
}

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                          usbus_event_usb_t event)
{
    (void) usbus;
    (void) handler;
    switch (event) {
        case USBUS_EVENT_USB_RESET:
            DEBUG_PUTS("EVENT RESET");
            break;

        default:
            DEBUG("Unhandled event :0x%x\n", event);
            break;
    }
}
