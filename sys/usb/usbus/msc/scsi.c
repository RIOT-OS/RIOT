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
 * @note    Keep in mind that SCSI is a big endian protocol and
 *          USB transfer data as little endian
 *
 * @author  Dylan Laduranty <dylan.laduranty@mesotic.com>
 * @}
 */

#include "usb/usbus.h"
#include "usb/msc.h"
#include "usb/usbus/msc.h"
#include "usb/usbus/msc/scsi.h"
#include "board.h"
#include "byteorder.h"

#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"

#define HOST_MINIMAL_PAGE_SIZE  512

static void _rx_ready(usbus_msc_device_t *msc)
{
    usbus_event_post(msc->usbus, &msc->rx_event);
}

void _scsi_test_unit_ready(usbus_handler_t *handler,  msc_cbw_buf_t *cbw)
{
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    if (cbw->data_len != 0) {
        static const usbopt_enable_t enable = USBOPT_ENABLE;

        if ((cbw->flags & USB_MSC_CBW_FLAG_IN) != 0) {
            usbdev_ep_set(msc->ep_in->ep, USBOPT_EP_STALL, &enable,
                          sizeof(usbopt_enable_t));
        }
        else {
            usbdev_ep_set(msc->ep_out->ep, USBOPT_EP_STALL, &enable,
                          sizeof(usbopt_enable_t));
        }
    }
    msc->state = GEN_CSW;
}

void _scsi_write10(usbus_handler_t *handler, msc_cbw_buf_t *cbw)
{
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    msc_cbw_rw10_pkt_t *pkt = (msc_cbw_rw10_pkt_t *)cbw->cb;

    /* Convert big endian data from SCSI to little endian */
    /* Get first block number to read from */
    msc->block = byteorder_swapl(pkt->blk_addr);

    /* Get number of blocks to transfer */
    msc->block_nb = byteorder_swaps(pkt->xfer_len);

    msc->cmd.len = cbw->data_len;
    msc->state = DATA_TRANSFER;
}

void _scsi_read10(usbus_handler_t *handler, msc_cbw_buf_t *cbw)
{
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    msc_cbw_rw10_pkt_t *pkt = (msc_cbw_rw10_pkt_t *)cbw->cb;

    /* Convert big endian data from SCSI to little endian */
    /* Get first block number to read from */
    msc->block = byteorder_swapl(pkt->blk_addr);

    /* Get number of blocks to transfer */
    msc->block_nb = byteorder_swaps(pkt->xfer_len);

    msc->cmd.len = cbw->data_len;
    msc->state = DATA_TRANSFER;

    if ((cbw->flags & USB_MSC_CBW_FLAG_IN) != 0) {
            _rx_ready(msc);
    }
}

void _scsi_inquiry(usbus_handler_t *handler)
{
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    msc_inquiry_pkt_t pkt;
    size_t len = sizeof(msc_inquiry_pkt_t);
    memset(&pkt, 0, len);
    /* prepare pkt response */
    pkt.type = 0;
    pkt.removable = 1;
    pkt.version = 0x01;
    pkt.length = len - 4;
    pkt.flags[0] = 0x80;

    memcpy(&pkt.vendor_id, USBUS_MSC_VENDOR_ID, sizeof(pkt.vendor_id));
    memcpy(&pkt.product_id, USBUS_MSC_PRODUCT_ID, sizeof(pkt.product_id));
    memcpy(&pkt.product_rev, USBUS_MSC_PRODUCT_REV, sizeof(pkt.product_rev));

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, &pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);
    msc->state = WAIT_FOR_TRANSFER;
    return;
}

void _scsi_read_capacity(usbus_handler_t *handler)
{
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    msc_read_capa_pkt_t pkt;
    uint32_t blk_nb = (mtd0->sector_count * mtd0->pages_per_sector);
    size_t len = sizeof(msc_read_capa_pkt_t);

    assert(mtd0->page_size <= HOST_MINIMAL_PAGE_SIZE);

    /* Host side expects to have at least 512 bytes per page, otherwise it will reject
       the setup. Thus add some logic here to be able to use smaller page size mtd device */

    pkt.blk_len = byteorder_swapl(HOST_MINIMAL_PAGE_SIZE);
    msc->pages_per_vpage = HOST_MINIMAL_PAGE_SIZE / mtd0->page_size;
    pkt.last_blk = byteorder_swapl(blk_nb - 1);

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, &pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);
    msc->state = WAIT_FOR_TRANSFER;
}

void _scsi_sense6(usbus_handler_t *handler)
{
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    msc_mode_parameter_pkt_t pkt;
    size_t len = sizeof(pkt);
    memset(&pkt, 0, len);
    /* Length of the packet minus the first byte */
    pkt.mode_data_len = len - 1;

    /* copy into ep buffer */
    memcpy(msc->ep_in->ep->buf, &pkt, len);
    usbdev_ep_ready(msc->ep_in->ep, len);
    /* Wait for the current pkt to transfer before sending CSW */
    msc->state = WAIT_FOR_TRANSFER;
}

void scsi_process_cmd(usbus_t *usbus, usbus_handler_t *handler,
                      usbdev_ep_t *ep, size_t len)
{
    (void)usbus;
    (void)len;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;

    /* store data into specific struct */
    msc_cbw_buf_t *cbw = (msc_cbw_buf_t*) ep->buf;

    /* Check Command Block signature */
    if (cbw->signature != SCSI_CBW_SIGNATURE) {
        DEBUG("Invalid CBW signature:0x%lx, abort\n", cbw->signature);
        msc->cmd.status = USB_MSC_CSW_STATUS_COMMAND_FAILED;
        msc->state = GEN_CSW;
        return;
    }

    /* Store command for CSW generation */
    msc->cmd.tag = cbw->tag;
    msc->cmd.status = 0;
    msc->cmd.len = 0;

    switch (cbw->cb[0]) {
        case SCSI_TEST_UNIT_READY:
            DEBUG_PUTS("SCSI_TEST_UNIT_READY");
            _scsi_test_unit_ready(handler, cbw);
            break;
        case SCSI_REQUEST_SENSE:
            DEBUG_PUTS("todo:SCSI_REQUEST_SENSE");
            msc->state=GEN_CSW;
            break;
        case SCSI_FORMAT_UNIT:
            DEBUG_PUTS("TODO: SCSI_FORMAT_UNIT");
            msc->state=GEN_CSW;
            break;
        case SCSI_INQUIRY:
            DEBUG_PUTS("SCSI_INQUIRY");
            _scsi_inquiry(handler);
            break;
        case SCSI_START_STOP_UNIT:
            DEBUG_PUTS("TODO: SCSI_START_STOP_UNIT");
            msc->state=GEN_CSW;
            break;
        case SCSI_MEDIA_REMOVAL:
            msc->cmd.status = 1;
            DEBUG_PUTS("SCSI_MEDIA_REMOVAL");
            msc->state=GEN_CSW;
            break;
        case SCSI_MODE_SELECT6:
            DEBUG_PUTS("TODO: SCSI_MODE_SELECT6");
            msc->state=GEN_CSW;
            break;
        case SCSI_MODE_SENSE6:
            DEBUG_PUTS("SCSI_MODE_SENSE6");
            _scsi_sense6(handler);
            break;
        case SCSI_MODE_SELECT10:
            DEBUG_PUTS("TODO: SCSI_MODE_SELECT10");
            msc->state=GEN_CSW;
            break;
        case SCSI_MODE_SENSE10:
            DEBUG_PUTS("TODO: SCSI_MODE_SENSE10");
            msc->state=GEN_CSW;
            break;
        case SCSI_READ_FORMAT_CAPACITIES:
            DEBUG_PUTS("TODO: SCSI_READ_FORMAT_CAPACITIES");
            msc->state=GEN_CSW;
            break;
        case SCSI_READ_CAPACITY:
            DEBUG_PUTS("SCSI_READ_CAPACITY");
            _scsi_read_capacity(handler);
            break;
        case SCSI_READ10:
            DEBUG_PUTS("SCSI_READ10");
            _scsi_read10(handler, cbw);
            break;
        case SCSI_WRITE10:
            DEBUG_PUTS("SCSI_WRITE10");
            _scsi_write10(handler, cbw);
            break;
        case SCSI_VERIFY10:
            DEBUG_PUTS("TODO: SCSI_VERIFY10");
            msc->state=GEN_CSW;
            break;
        default:
            DEBUG("Unhandled SCSI command:0x%x", cbw->cb[0]);
            msc->state=GEN_CSW;
    }
}

void scsi_gen_csw(usbus_handler_t *handler, cbw_info_t cmd)
{
    msc_csw_buf_t csw;
    usbus_msc_device_t *msc = (usbus_msc_device_t*)handler;
    memset(&csw, 0, sizeof(msc_csw_buf_t));
    csw.signature = SCSI_CSW_SIGNATURE;
    csw.tag = cmd.tag;
    csw.data_left = cmd.len;
    csw.status = cmd.status;
    memcpy(msc->ep_in->ep->buf, &csw, sizeof(msc_csw_buf_t));
    usbdev_ep_ready(msc->ep_in->ep, sizeof(msc_csw_buf_t));
}
