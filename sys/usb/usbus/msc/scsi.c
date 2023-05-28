/*
 * Copyright (C) 2019-2022 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup usbus_msc
 * @{
 *
 * @note    SCSI is a big endian protocol and
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

#include <string.h>

#define ENABLE_DEBUG 0
#include "debug.h"

static void _rx_ready(usbus_msc_device_t *msc)
{
    usbus_event_post(msc->usbus, &msc->rx_event);
}

static void _scsi_test_unit_ready(usbus_handler_t *handler,  msc_cbw_buf_t *cbw)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);

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

static void _scsi_write10(usbus_handler_t *handler, msc_cbw_buf_t *cbw, uint8_t lun)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_cbw_rw10_pkt_t *pkt = (msc_cbw_rw10_pkt_t *)cbw->cb;
    mtd_dev_t *mtd = msc->lun_dev[lun].mtd;
    uint32_t mtd_sector_size = mtd->page_size * mtd->pages_per_sector;

    /* Convert big endian data from SCSI to little endian */
    /* Get first block number to read from */
    if (mtd_sector_size < 512) {
        msc->block = byteorder_ntohl(pkt->blk_addr) * (512 / mtd_sector_size);
    }
    else if (mtd_sector_size > 4096) {
        /* TODO: add support */
        assert(0);
    }
    else {
        msc->block = byteorder_ntohl(pkt->blk_addr);
    }

    /* Get number of blocks to transfer */
    msc->block_nb = byteorder_ntohs(pkt->xfer_len);
    msc->cmd.len = cbw->data_len;
    msc->state = DATA_TRANSFER_OUT;
}

static void _scsi_read10(usbus_handler_t *handler, msc_cbw_buf_t *cbw, uint8_t lun)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_cbw_rw10_pkt_t *pkt = (msc_cbw_rw10_pkt_t *)cbw->cb;
    mtd_dev_t *mtd = msc->lun_dev[lun].mtd;
    uint32_t mtd_sector_size = mtd->page_size * mtd->pages_per_sector;
    /* Convert big endian data from SCSI to little endian */
    /* Get first block number to read from */
    if (mtd_sector_size < 512) {
        /* If MTD sector size is less than 512 (e.g for EEPROM),
         * apply some logic and let host believe this is a 512
         * sector size device and let RIOT write several sectors in
         * a row to reach 512 bytes so everyone is happy */
        msc->block = byteorder_ntohl(pkt->blk_addr) * (512 / mtd_sector_size);
    }
    else if (mtd_sector_size > 4096) {
        /* If MTD sector size is above 4096 bytes (e.g some MCU internal flash)
         * limits the reported block size to 4096 bytes and add some logic to
         * write less than a sector (using read-modify-write method). This will
         * reduce the overall performance !*/
        msc->block = byteorder_ntohl(pkt->blk_addr) / (mtd_sector_size / 4096);
    }
    else {
        msc->block = byteorder_ntohl(pkt->blk_addr);
    }

    /* Get number of blocks to transfer */
    msc->block_nb = byteorder_ntohs(pkt->xfer_len);
    msc->cmd.len = cbw->data_len;
    msc->state = DATA_TRANSFER_IN;

    if ((cbw->flags & USB_MSC_CBW_FLAG_IN) != 0) {
        _rx_ready(msc);
    }
}

static void _scsi_inquiry(usbus_handler_t *handler)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_inquiry_pkt_t *pkt = (msc_inquiry_pkt_t *)msc->in_buf;
    size_t len = sizeof(msc_inquiry_pkt_t);

    /* Clear the exact amount of bytes needed for this transfer */
    memset(pkt, 0, len);

    /* prepare pkt response */
    pkt->type = 0;
    pkt->removable = 1;
    pkt->version = 0x01;
    /* Report the size of the additional data */
    pkt->length = len - 4;

    memcpy(&pkt->vendor_id, USBUS_MSC_VENDOR_ID, sizeof(pkt->vendor_id));
    memcpy(&pkt->product_id, USBUS_MSC_PRODUCT_ID, sizeof(pkt->product_id));
    memcpy(&pkt->product_rev, USBUS_MSC_PRODUCT_REV, sizeof(pkt->product_rev));

    /* copy into ep buffer */
    usbdev_ep_xmit(msc->ep_in->ep, (uint8_t *)pkt, len);
    msc->state = WAIT_FOR_TRANSFER;
    return;
}

/*TODO: ensure this function is called AFTER _read_capacity*/
static void _scsi_read_format_capacities(usbus_handler_t *handler, uint8_t lun)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_read_fmt_capa_pkt_t *pkt = (msc_read_fmt_capa_pkt_t *)msc->in_buf;
    uint16_t block_size = msc->lun_dev[lun].block_size;
    uint32_t blk_nb = msc->lun_dev[lun].block_nb;
    size_t len = sizeof(msc_read_fmt_capa_pkt_t);

    /* Clear the exact amount of bytes needed for this transfer */
    memset(pkt, 0, len);

    /* Only support one list, size of the whole struct minus the 4 first bytes */
    pkt->list_length = len - 4;
    pkt->blk_nb = byteorder_htonl(blk_nb);
    pkt->type = SCSI_READ_FMT_CAPA_TYPE_FORMATTED;

    /* Manage endianness, bytes 11..9 -> LSB..MSB */
    pkt->blk_len[0] = (block_size >> 16) & 0xFF;
    pkt->blk_len[1] = (block_size >> 8) & 0xFF;
    pkt->blk_len[2] = block_size & 0xFF;

    /* copy into ep buffer */
    usbdev_ep_xmit(msc->ep_in->ep, (uint8_t *)pkt, sizeof(msc_read_fmt_capa_pkt_t));
    msc->state = WAIT_FOR_TRANSFER;
}

static void _scsi_read_capacity(usbus_handler_t *handler, uint8_t lun)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_read_capa_pkt_t *pkt = (msc_read_capa_pkt_t *)msc->in_buf;
    mtd_dev_t *mtd = msc->lun_dev[lun].mtd;
    uint16_t block_size = 0;
    uint32_t mtd_sector_size = 0;
    size_t len = sizeof(msc_read_capa_pkt_t);

    uint32_t blk_nb = mtd->sector_count;

    /* Clear the exact amount of bytes needed for this transfer */
    memset(pkt, 0, len);

    /* Compute the MTD sector size */
    mtd_sector_size = mtd->page_size * mtd->pages_per_sector;

    /* Check mtd sector size boundaries */
    /* Host side expects to have at least 512 bytes per block, otherwise it
     * will reject the setup. Thus, add some logic here to be able to use
     * smaller page size mtd device */
    if (mtd_sector_size < 512) {
        block_size = 512;
        /* Compute the number of block that will be reported to the host
           Since the MTD sector is less than the minimum allowed (512),
           the number of block reported is divided by the number of
           sector that will be written
           Example: if the MTD sector size is 64 bytes and the USB reported
           block is 512, we must use 8 (512 / 64) sectors per USB block */
        blk_nb /= (512 / mtd_sector_size);
    }
    /* setup some logic to manage devices with more than 4096 bytes
     * per page size */
    else if (mtd_sector_size > 4096) {
        /* Force it to the greatest value supported by host */
        block_size = 4096;
        /* This case is the reverse as above, since the size of a sector
           is above the maximum allowed boundary, Read/Modify/Write
           operation is needed to properly write to the MTD device */
           blk_nb *= (mtd_sector_size / 4096);
    }
    /* Standard page size support by Linux host driver (see drivers/scsi/sd.c
     * in Linux kernel source tree) */
    else if (mtd_sector_size == 512  || mtd_sector_size == 1024 ||
             mtd_sector_size == 2048 || mtd_sector_size == 4096) {
        block_size = mtd_sector_size;
    }
    else {
        DEBUG_PUTS("Unsupported pagesize");
        assert(0);
    }
    /* Report the size of the block (mtd sector size) */
    pkt->blk_len.u32 = byteorder_swapl(block_size);
    /* Store the reported block size and number for internal use */
    msc->lun_dev[lun].block_size = block_size;
    msc->lun_dev[lun].block_nb = blk_nb;
    /* Report the last block number */
    pkt->last_blk.u32 = byteorder_swapl(blk_nb - 1);

    /* copy into ep buffer */
    usbdev_ep_xmit(msc->ep_in->ep, (uint8_t *)pkt, len);
    msc->state = WAIT_FOR_TRANSFER;
}

static void _scsi_request_sense(usbus_handler_t *handler)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_request_sense_pkt_t *pkt = (msc_request_sense_pkt_t *)msc->in_buf;
    size_t len = sizeof(msc_request_sense_pkt_t);

    /* Clear the exact amount of bytes needed for this transfer */
    memset(pkt, 0, len);

    pkt->error_code = SCSI_REQUEST_SENSE_ERROR;
    pkt->sense_key = SCSI_SENSE_KEY_ILLEGAL_REQUEST;
    /* Report the size of the struct minus the 7 first bytes */
    pkt->add_len = len - 7;
    /* TODO: prepare a proper mechanism to correctly report all kind of error
             to the host through the sense_key, asc and ascq combination */
    /* Report CANNOT READ MEDIUM - UNKNOWN FORMAT as default value for now */
    pkt->asc = 0x30;
    pkt->ascq = 0x01;

    /* copy into ep buffer */
    usbdev_ep_xmit(msc->ep_in->ep, (uint8_t *)pkt, len);
    /* Wait for the current pkt to transfer before sending CSW */
    msc->state = WAIT_FOR_TRANSFER;
}

static void _scsi_sense6(usbus_handler_t *handler)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_mode_parameter_pkt_t *pkt = (msc_mode_parameter_pkt_t *)msc->in_buf;
    size_t len = sizeof(msc_mode_parameter_pkt_t);

    /* Clear the exact amount of bytes needed for this transfer */
    memset(pkt, 0, len);
    /* Length of the packet minus the first byte */
    pkt->mode_data_len = len - 1;

    /* copy into ep buffer */
    usbdev_ep_xmit(msc->ep_in->ep, (uint8_t *)pkt, len);
    /* Wait for the current pkt to transfer before sending CSW */
    msc->state = WAIT_FOR_TRANSFER;
}

void usbus_msc_scsi_process_cmd(usbus_t *usbus, usbus_handler_t *handler,
                                usbdev_ep_t *ep, size_t len)
{
    (void)usbus;
    (void)ep;
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);

    /* store data into specific struct */
    msc_cbw_buf_t *cbw = (msc_cbw_buf_t *)msc->out_buf;

    /* Check Command Block signature */
    if (cbw->signature != SCSI_CBW_SIGNATURE) {
        DEBUG("Invalid CBW signature:0x%" PRIx32 ", abort\n", cbw->signature);
        msc->cmd.status = USB_MSC_CSW_STATUS_COMMAND_FAILED;
        msc->state = GEN_CSW;
        return;
    }

    /* Store command for CSW generation & some operations */
    msc->cmd.tag = cbw->tag;
    msc->cmd.status = 0;
    msc->cmd.len = 0;
    msc->cmd.lun = cbw->lun;

    switch (cbw->cb[0]) {
    case SCSI_TEST_UNIT_READY:
        DEBUG("SCSI_TEST_UNIT_READY:%d\n", len);
        _scsi_test_unit_ready(handler, cbw);
        break;
    case SCSI_REQUEST_SENSE:
        DEBUG_PUTS("SCSI_REQUEST_SENSE");
        _scsi_request_sense(handler);
        break;
    case SCSI_FORMAT_UNIT:
        DEBUG_PUTS("TODO: SCSI_FORMAT_UNIT");
        msc->state = GEN_CSW;
        break;
    case SCSI_INQUIRY:
        DEBUG_PUTS("SCSI_INQUIRY");
        _scsi_inquiry(handler);
        break;
    case SCSI_START_STOP_UNIT:
        DEBUG_PUTS("TODO: SCSI_START_STOP_UNIT");
        msc->state = GEN_CSW;
        break;
    case SCSI_MEDIA_REMOVAL:
        msc->cmd.status = 1;
        DEBUG_PUTS("SCSI_MEDIA_REMOVAL");
        msc->state = GEN_CSW;
        break;
    case SCSI_MODE_SELECT6:
        DEBUG_PUTS("TODO: SCSI_MODE_SELECT6");
        msc->state = GEN_CSW;
        break;
    case SCSI_MODE_SENSE6:
        DEBUG_PUTS("SCSI_MODE_SENSE6");
        _scsi_sense6(handler);
        break;
    case SCSI_MODE_SELECT10:
        DEBUG_PUTS("TODO: SCSI_MODE_SELECT10");
        msc->state = GEN_CSW;
        break;
    case SCSI_MODE_SENSE10:
        DEBUG_PUTS("TODO: SCSI_MODE_SENSE10");
        msc->state = GEN_CSW;
        break;
    case SCSI_READ_FORMAT_CAPACITIES:
        DEBUG_PUTS("SCSI_READ_FORMAT_CAPACITIES");
        _scsi_read_format_capacities(handler, cbw->lun);
        break;
    case SCSI_READ_CAPACITY:
        DEBUG_PUTS("SCSI_READ_CAPACITY");
        _scsi_read_capacity(handler, cbw->lun);
        break;
    case SCSI_READ10:
        DEBUG("SCSI_READ10:%d\n", len);
        _scsi_read10(handler, cbw, cbw->lun);
        break;
    case SCSI_WRITE10:
        DEBUG("SCSI_WRITE10:%d\n", len);
        _scsi_write10(handler, cbw, cbw->lun);
        break;
    case SCSI_VERIFY10:
        DEBUG_PUTS("TODO: SCSI_VERIFY10");
        msc->state = GEN_CSW;
        break;
    default:
        DEBUG("Unhandled SCSI command:0x%x", cbw->cb[0]);
        msc->state = GEN_CSW;
    }
}

void scsi_gen_csw(usbus_handler_t *handler, cbw_info_t *cmd)
{
    usbus_msc_device_t *msc = container_of(handler, usbus_msc_device_t,
                                           handler_ctrl);
    msc_csw_buf_t *csw = (msc_csw_buf_t *)msc->in_buf;

    memset(csw, 0, sizeof(msc_csw_buf_t));
    csw->signature = SCSI_CSW_SIGNATURE;
    csw->tag = cmd->tag;
    csw->data_left = cmd->len;
    csw->status = cmd->status;
    usbdev_ep_xmit(msc->ep_in->ep, (uint8_t *)csw, sizeof(msc_csw_buf_t));
}
