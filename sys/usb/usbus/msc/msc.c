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

#ifndef MTD_MSC
/* TODO: add support for multiple MTD devices */
#define MTD_MSC MTD_0
#endif

/* Internal buffer to handle size difference between MTD layer and USB
   endpoint size as some MTD implementation (like mtd_sdcard) doesn't allow
   endpoint size transfer */
static unsigned char _buff[USBUS_MSC_BUFFER_SIZE];
static unsigned char _ep_out_buf[CONFIG_USBUS_EP0_SIZE];
static unsigned char _ep_in_buf[CONFIG_USBUS_EP0_SIZE];

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

static void _write_xfer(usbus_msc_device_t *msc)
{
    /* Check if we have a block to read and transfer */
    if (!msc->block_nb) {
        return;
    }

    size_t len;
    /* Retrieve incoming data */
    usbdev_ep_get(msc->ep_out->ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));

    /* Update offset for page buffer */
    msc->block_offset += len;
    /* Decrement whole len */
    msc->cmd.len -= len;

    /* buffer is full, write it and point to new block if any */
    if (msc->block_offset >= (MTD_MSC->page_size * msc->pages_per_vpage)) {
        mtd_write_page(MTD_MSC, msc->buffer, msc->block * msc->pages_per_vpage,
                       0, MTD_MSC->page_size * msc->pages_per_vpage);
        msc->block_offset = 0;
        msc->block++;
        msc->block_nb--;
    }

    if (msc->cmd.len == 0) {
        /* All blocks have been transferred, send CSW to host */
        if (msc->state == DATA_TRANSFER_OUT) {
            msc->state = GEN_CSW;
            /* Data was processed, ready next transfer */
            usbdev_ep_xmit(msc->ep_out->ep, msc->out_buf, CONFIG_USBUS_EP0_SIZE);
            return;
        }
    }

    if (len > 0) {
        /* Directly put data incoming on the endpoint to the flashpage buffer */
        usbdev_ep_xmit(msc->ep_out->ep, &msc->buffer[msc->block_offset], len);
    }
}

static void _xfer_data( usbus_msc_device_t *msc)
{
    /* Check if we have a block to read and transfer */
    if (msc->block_nb) {
        /* read buffer from mtd device */
        if (msc->block_offset == 0) {
            mtd_read_page(MTD_MSC, msc->buffer, msc->block * msc->pages_per_vpage,
                          0, MTD_MSC->page_size * msc->pages_per_vpage);
        }
        /* Data prepared, signal ready to usbus */
        usbdev_ep_xmit(msc->ep_in->ep, &msc->buffer[msc->block_offset], msc->ep_in->ep->len);
        /* Update offset for page buffer */
        msc->block_offset += msc->ep_in->ep->len;
        /* Decrement whole len */
        msc->cmd.len -= msc->ep_in->ep->len;
        /* whole buffer is empty, point to new block if any */
        if (msc->block_offset >= (MTD_MSC->page_size * msc->pages_per_vpage)) {
            msc->block_offset = 0;
            msc->block++;
            msc->block_nb--;
        }
    }
    else {
        /* All blocks have been transferred, send CSW to host */
        if (msc->state == DATA_TRANSFER_IN) {
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
    usbus_register_event_handler(usbus, (usbus_handler_t *)handler);
    printf("[MSC]: MTD init...");
    if (mtd_init(MTD_MSC) != 0) {
        puts("[FAILED]");
        return -1;
    }
    puts("[OK]");
    return 0;
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    DEBUG("MSC: initialization\n");
    usbus_msc_device_t *msc = (usbus_msc_device_t *)handler;

    msc->msc_descr.next = NULL;
    msc->msc_descr.funcs = &_msc_descriptor;
    msc->msc_descr.arg = msc;
    msc->block_offset = 0;
    msc->buffer = _buff;
    msc->block_nb = 0;
    msc->block = 0;
    msc->state = WAITING;
    /* Assign internal endpoint buffers */
    msc->out_buf = _ep_out_buf;
    msc->in_buf = _ep_in_buf;
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
                                    USB_EP_DIR_IN, CONFIG_USBUS_EP0_SIZE);
    msc->ep_in->interval = 0;
    msc->ep_out = usbus_add_endpoint(usbus, &msc->iface, USB_EP_TYPE_BULK,
                                     USB_EP_DIR_OUT, CONFIG_USBUS_EP0_SIZE);
    msc->ep_out->interval = 0;

    /* Add interfaces to the stack */
    usbus_add_interface(usbus, &msc->iface);

    usbus_enable_endpoint(msc->ep_in);
    usbus_enable_endpoint(msc->ep_out);

    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);

    /* Prepare to receive first bytes from Host */
    usbdev_ep_xmit(msc->ep_out->ep, msc->out_buf, CONFIG_USBUS_EP0_SIZE);
}

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup)
{
    (void)usbus;
    (void)state;

    usbus_msc_device_t *msc = (usbus_msc_device_t *)handler;
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

    usbus_msc_device_t *msc = (usbus_msc_device_t *)handler;

    if (ep->dir == USB_EP_DIR_OUT) {
        size_t len=0;
        /* Previous transfer is sent, send the next one */
        if (msc->state == DATA_TRANSFER_OUT) {
            _write_xfer(msc);
        }
        else {
            /* Retrieve incoming data */
            usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
            if (len > 0) {
                /* Process incoming endpoint buffer */
                scsi_process_cmd(usbus, handler, ep, len);
            }
            /* Data was processed, ready next transfer */
            if (msc->state == DATA_TRANSFER_OUT) {
                /* If the next pkt is supposed to be a WRITE10 transfer,
                   directly redirect content to appropriate buffer */
                usbdev_ep_xmit(msc->ep_out->ep, &msc->buffer[msc->block_offset], 64);
            }
            else {
                usbdev_ep_xmit(ep, msc->out_buf, CONFIG_USBUS_EP0_SIZE);
            }
        }
    }
    else {
        /* Data Transfer pending */
        if (msc->state == DATA_TRANSFER_IN) {
            _xfer_data(msc);
        }
        /* CBW answer was sent, prepare CSW */
        else if (msc->state == WAIT_FOR_TRANSFER) {
            msc->state = GEN_CSW;
        }
    }

    if (msc->cmd.tag && msc->cmd.len == 0 && msc->state == GEN_CSW) {
        DEBUG_PUTS("[MSC]: Generate CSW");
        scsi_gen_csw(handler, &(msc->cmd));
        /* Command has been handled, so clear tag and state machine */
        msc->cmd.tag = 0;
        msc->state = WAITING;
    }
}

static void _event_handler(usbus_t *usbus, usbus_handler_t *handler,
                           usbus_event_usb_t event)
{
    (void)usbus;
    (void)handler;
    switch (event) {
    case USBUS_EVENT_USB_RESET:
        DEBUG_PUTS("EVENT RESET");
        break;

    default:
        DEBUG("Unhandled event :0x%x\n", event);
        break;
    }
}
