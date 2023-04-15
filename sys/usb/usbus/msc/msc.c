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
#include <errno.h>

#define ENABLE_DEBUG    0
#include "debug.h"

#include "mtd.h"

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
    uint8_t lun = msc->cmd.lun;
    uint16_t block_size = msc->lun_dev[lun].block_size;
    uint32_t sector_count;
    mtd_dev_t *mtd = msc->lun_dev[lun].mtd;
    size_t len;
    int ret;

    /* Check if we have a block to read and transfer */
    if (!msc->block_nb) {
        return;
    }

    /* Retrieve incoming data */
    usbdev_ep_get(msc->ep_out->ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));

    /* Update offset for page buffer */
    msc->block_offset += len;
    /* Decrement whole len */
    msc->cmd.len -= len;
    /* buffer is full, write it and point to new block if any */
    if (msc->block_offset >= block_size) {
        /* Specific case for MTD devices with sector size < 512 bytes
           Since USB report 512 bytes per sector, we need to erase
           several sectors at the same time */
        if (mtd->page_size * mtd->pages_per_sector < 512) {
            sector_count = (512 / (mtd->page_size * mtd->pages_per_sector));
        } else {
            sector_count = 1;
        }

        /* Write one or multiple sectors */
        ret = mtd_write_page(mtd, msc->buffer,
                             msc->block * sector_count * mtd->pages_per_sector,
                             0, block_size);
        if (ret != 0) {
            /* Failure occurs during write operation, stall the operation */
            DEBUG("[msc]: Write fail with error: %d\n", ret);
            /* Stall IN bulk endpoint and signal error to host through
             * CSW command with command failed status */
            static const usbopt_enable_t enable = USBOPT_ENABLE;
            usbdev_ep_set(msc->ep_in->ep, USBOPT_EP_STALL, &enable,
                          sizeof(usbopt_enable_t));
            msc->state = GEN_CSW;
            msc->cmd.status = USB_MSC_CSW_STATUS_COMMAND_FAILED;
        }
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

static void _read_xfer(usbus_msc_device_t *msc)
{
    uint8_t lun = msc->cmd.lun;
    mtd_dev_t *mtd = msc->lun_dev[lun].mtd;
    uint32_t block_size = msc->lun_dev[lun].block_size;
    int ret;
    /* Check if we have a block to read and transfer */
    if (msc->block_nb) {
        /* read buffer from mtd device */
        if (msc->block_offset == 0) {
            ret = mtd_read(mtd, msc->buffer, msc->block * block_size, block_size);
            if (ret != 0) {
                DEBUG("[msc]: Read operation failed with error:%d\n", ret);
                /* Stall the current operation, and signal it to host */
                static const usbopt_enable_t enable = USBOPT_ENABLE;
                usbdev_ep_set(msc->ep_in->ep, USBOPT_EP_STALL, &enable,
                              sizeof(usbopt_enable_t));
                msc->state = GEN_CSW;
                msc->cmd.status = USB_MSC_CSW_STATUS_COMMAND_FAILED;

            }
        }
        /* Data prepared, signal ready to usbus */
        usbdev_ep_xmit(msc->ep_in->ep, &msc->buffer[msc->block_offset],
                       msc->ep_in->ep->len);
        /* Update offset for page buffer */
        msc->block_offset += msc->ep_in->ep->len;
        /* Decrement whole len */
        msc->cmd.len -= msc->ep_in->ep->len;
        /* whole buffer is empty, point to new block if any */
        if (msc->block_offset >= block_size) {
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

    _read_xfer(msc);
}

static usbus_msc_device_t *_get_msc_handler(usbus_t *usbus)
{
    for (usbus_interface_t *iface = usbus->iface; iface; iface = iface->next) {
        if (iface->class == USB_CLASS_MASS_STORAGE) {
            return (usbus_msc_device_t *)iface->handler;
        }
    }
    return NULL;
}

static unsigned _get_lun(usbus_msc_device_t *msc)
{
    unsigned count = 0;

    /* Count only registered MTD devices as USB LUN, (using usbus_msc_add_lun)
     * not every MTD devices available on board */
    for (int i = 0; i < USBUS_MSC_EXPORTED_NUMOF; i++) {
        if (msc->lun_dev[i].mtd != NULL) {
            count++;
        }
    }
    return count;
}

int usbus_msc_add_lun(usbus_t *usbus, mtd_dev_t *dev)
{
    uint32_t block_size;
    /* Loop through all interfaces to find the MSC one */
    usbus_msc_device_t *msc = _get_msc_handler(usbus);

    if (!dev) {
        return -ENODEV;
    }

    /* Check if MTD isn't already registered */
    for (int i = 0; i < USBUS_MSC_EXPORTED_NUMOF; i++) {
        if (dev == msc->lun_dev[i].mtd) {
            return -EBUSY;
        }
    }
    /* Store new MTD device in first slot available
       Also re alloc internal buffer if needed */
    for (int i = 0; i < USBUS_MSC_EXPORTED_NUMOF; i++) {
        if (msc->lun_dev[i].mtd == NULL) {
            int ret = mtd_init(dev);
            if (ret != 0) {
                DEBUG_PUTS("[msc]: cannot initialize MTD device");
                return -ENODEV;
            }
            block_size = dev->page_size * dev->pages_per_sector;
            /* Ensure MTD device can be exported */
            if (block_size * dev->sector_count < 512) {
                DEBUG("page size:%" PRIu32 " pages per sector:%" PRIu32 " \
                       sector_count:%" PRIu32 "\n", dev->page_size,
                      dev->pages_per_sector, dev->sector_count);
                return -ENOMEM;
            }
            if (msc->buffer != NULL) {
                /* If new registered MTD device need more memory than the
                   previous, realloc a new buffer */
                if (block_size > msc->buffer_size) {
                    msc->buffer = realloc(msc->buffer, block_size);
                }
            } else {
                /* No buffer allocated yet, so allocate one */
                msc->buffer = malloc(block_size);
            }

            if (!msc->buffer) {
                DEBUG_PUTS("[msc]: Failed to realloc new buffer\n");
                return -ENOMEM;
            }
            msc->buffer_size = block_size;
            msc->lun_dev[i].mtd = dev;
            return 0;
        }
    }

    /* No more slots available */
    return -ENFILE;
}

int usbus_msc_remove_lun(usbus_t *usbus, mtd_dev_t *dev)
{
    /* Loop through all interfaces to find the MSC one */
    usbus_msc_device_t *msc = _get_msc_handler(usbus);

    /* Identify the LUN to unexport */
    for (int i = 0; i < USBUS_MSC_EXPORTED_NUMOF; i++) {
        if (dev == msc->lun_dev[i].mtd) {
            /* Wait for any pending transaction to end */

            /* Clear relevant information */
            msc->lun_dev[i].mtd = NULL;
            msc->lun_dev[i].block_size = 0;
            msc->lun_dev[i].block_nb = 0;
            return 0;
        }
    }

    /* No more slots available */
    free(msc->buffer);
    msc->buffer = NULL;
    return -ENODEV;
}

int usbus_msc_init(usbus_t *usbus, usbus_msc_device_t *handler)
{
    assert(usbus);
    assert(handler);
    memset(handler, 0, sizeof(usbus_msc_device_t));
    handler->usbus = usbus;
    handler->handler_ctrl.driver = &msc_driver;
    usbus_register_event_handler(usbus, (usbus_handler_t *)handler);
    return 0;
}

static void _init(usbus_t *usbus, usbus_handler_t *handler)
{
    DEBUG_PUTS("[msc]: initialization\n");
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);

    msc->msc_descr.next = NULL;
    msc->msc_descr.funcs = &_msc_descriptor;
    msc->msc_descr.arg = msc;
    msc->block_offset = 0;
    /* Buffer will be initialized later */
    msc->buffer = NULL;
    msc->buffer_size = 0;
    msc->block_nb = 0;
    msc->block = 0;
    msc->state = WAITING;

    for (int i = 0; i < USBUS_MSC_EXPORTED_NUMOF; i++) {
        msc->lun_dev[i].mtd = NULL;
        msc->lun_dev[i].block_size = 0;
        msc->lun_dev[i].block_nb = 0;
    }

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
    assert(msc->ep_in);
    msc->ep_in->interval = 0;
    msc->ep_out = usbus_add_endpoint(usbus, &msc->iface, USB_EP_TYPE_BULK,
                                     USB_EP_DIR_OUT, CONFIG_USBUS_EP0_SIZE);
    assert(msc->ep_out);
    msc->ep_out->interval = 0;

    /* Add interfaces to the stack */
    usbus_add_interface(usbus, &msc->iface);

    usbus_enable_endpoint(msc->ep_in);
    usbus_enable_endpoint(msc->ep_out);

    usbus_handler_set_flag(handler, USBUS_HANDLER_FLAG_RESET);

    /* Auto-configure all MTD devices */
    if (CONFIG_USBUS_MSC_AUTO_MTD) {
        for (int i = 0; i < USBUS_MSC_EXPORTED_NUMOF; i++) {
            usbus_msc_add_lun(usbus, mtd_default_get_dev(i));
        }
    }
}

static int _control_handler(usbus_t *usbus, usbus_handler_t *handler,
                            usbus_control_request_state_t state,
                            usb_setup_t *setup)
{
    (void)usbus;
    (void)state;

    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);

    uint8_t data;
    switch (setup->request) {
    case USB_MSC_SETUP_REQ_GML:
        data = _get_lun(msc);
        if (data > 0) {
            data -= 1;
        }
        /* Return the number of MTD devices available on the board */
        usbus_control_slicer_put_bytes(usbus, &data, sizeof(data));
        /* Prepare to receive first bytes from Host */
        usbdev_ep_xmit(msc->ep_out->ep, msc->out_buf, CONFIG_USBUS_EP0_SIZE);
        break;
    case USB_MSC_SETUP_REQ_BOMSR:
        DEBUG_PUTS("[msc]: TODO: implement reset setup request");
        break;
    default:
        DEBUG("[msc]: unsupported handle setup request:0x%x\n", setup->request);
        return -1;
    }

    return 1;
}

static void _transfer_handler(usbus_t *usbus, usbus_handler_t *handler,
                              usbdev_ep_t *ep, usbus_event_transfer_t event)
{
    (void)usbus;
    (void)event;

    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);

    if (ep->dir == USB_EP_DIR_OUT) {
        size_t len = 0;
        /* Previous transfer is sent, send the next one */
        if (msc->state == DATA_TRANSFER_OUT) {
            _write_xfer(msc);
        }
        else {
            /* Retrieve incoming data */
            usbdev_ep_get(ep, USBOPT_EP_AVAILABLE, &len, sizeof(size_t));
            if (len > 0) {
                /* Process incoming endpoint buffer */
                usbus_msc_scsi_process_cmd(usbus, handler, ep, len);
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
            _read_xfer(msc);
        }
        /* CBW answer was sent, prepare CSW */
        else if (msc->state == WAIT_FOR_TRANSFER) {
            msc->state = GEN_CSW;
        }
    }

    if (msc->cmd.tag && msc->cmd.len == 0 && msc->state == GEN_CSW) {
        DEBUG_PUTS("[msc]: Generate CSW");
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
        DEBUG_PUTS("[msc]: host reset event");
        break;

    default:
        DEBUG("[msc]: Unhandled event :0x%x\n", event);
        break;
    }
}
