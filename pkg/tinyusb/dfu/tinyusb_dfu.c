/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pkg_tinyusb_dfu
 * @{
 * @file    TinyUSB DFU implementation
 *
 * @author  Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include "log.h"
#include "periph/pm.h"
#include "riotboot/magic.h"
#include "riotboot/slot.h"

#ifdef MODULE_RIOTBOOT_TINYUSB_DFU
#include "ztimer.h"
#endif

#include "device/usbd.h"
#include "class/dfu/dfu_device.h"

#include "tinyusb_dfu.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#ifdef MODULE_RIOTBOOT_TINYUSB_DFU
static void _reboot(void *arg);
static ztimer_t scheduled_reboot = { .callback=_reboot };
#endif

/* there is only one instance of DFU devices */
tinyusb_dfu_device_t _tusb_dfu_dev = { .skip_signature = true };

void tinyusb_dfu_init(void)
{
    _tusb_dfu_dev.skip_signature = true;
    _tusb_dfu_dev.slot = 0;
}

#ifdef MODULE_RIOTBOOT_TINYUSB_DFU

uint32_t tud_dfu_get_timeout_cb(uint8_t alt, uint8_t state)
{
    (void)alt;
    (void)state;

    /* Invoked before tud_dfu_download_cb() or tud_dfu_manifest_cb() is called
     * to determine the poll timeout for download and manifest operation */
    return CONFIG_TUSBD_DFU_POLL_TIMEOUT;
}

void tud_dfu_download_cb(uint8_t alt, uint16_t block_num, uint8_t const *data, uint16_t length)
{
    /* Invoked when a DFU_DNLOAD (wLength > 0) followed by a DFU_GETSTATUS
     * (state = DFU_DNBUSY) requests was received. */

    (void)block_num;

    int ret = 0;

    if (_tusb_dfu_dev.skip_signature) {
        /* Avoid underflow condition */
        if (length < RIOTBOOT_FLASHWRITE_SKIPLEN) {
            return;
        }
        riotboot_flashwrite_init(&_tusb_dfu_dev.writer, alt);
        length -= RIOTBOOT_FLASHWRITE_SKIPLEN;
        DEBUG("[tinyusb_dfu] starting the download with %u bytes for slot %u "
              "with block %u\n", length, alt, block_num);
        _tusb_dfu_dev.skip_signature = false;
        _tusb_dfu_dev.slot = alt;
        ret = riotboot_flashwrite_putbytes(&_tusb_dfu_dev.writer,
                                           &data[RIOTBOOT_FLASHWRITE_SKIPLEN],
                                           length, true);
    }
    else {
        assert(alt == _tusb_dfu_dev.slot);

        DEBUG("[tinyusb_dfu] continue the download with %u bytes for slot %u "
              "with block %u\n", length, alt, block_num);
        ret = riotboot_flashwrite_putbytes(&_tusb_dfu_dev.writer,
                                           data, length, true);
    }
    if (ret < 0) {
        LOG_ERROR("[tinyusb_dfu] error on writing %d bytes for slot %u\n",
                  length, alt);
        tud_dfu_finish_flashing(DFU_STATUS_ERR_WRITE);
    }
    else {
        tud_dfu_finish_flashing(DFU_STATUS_OK);
    }
}

void tud_dfu_manifest_cb(uint8_t alt)
{
    /* Invoked when the download process is complete and
     * DFU_DNLOAD (wLength = 0) followed by a DFU_GETSTATUS (state = Manifest)
     * was received. */

    (void)alt;

    assert(alt == _tusb_dfu_dev.slot);

    DEBUG("[tinyusb_dfu] download for slot %u complete, "
          "enter manifestation phase\n", alt);

    /* the host indicates that the download process is complete */
    riotboot_flashwrite_flush(&_tusb_dfu_dev.writer);
    riotboot_flashwrite_finish(&_tusb_dfu_dev.writer);

    /* indicate that flashing is finished */
    tud_dfu_finish_flashing(DFU_STATUS_OK);

    /* scheduled reboot after CONFIG_TUSBD_DFU_RESET_DELAY seconds to give
     * enough time to finish manifestation */
    ztimer_set(ZTIMER_SEC, &scheduled_reboot, CONFIG_TUSBD_DFU_RESET_DELAY);
}

static void _reboot(void *arg)
{
    DEBUG("[tinyusb_dfu] reboot\n");

    (void)arg;
    pm_reboot();
}
#endif /* MODULE_RIOTBOOT_TINYUSB_DFU */

TU_ATTR_WEAK void tud_dfu_detach_cb(void)
{
    /* the host sent a DFU_DETACH request */

    DEBUG("[tinyusb_dfu] DFU_DETACH request received\n");

    uint32_t *reset_addr = (uint32_t *)RIOTBOOT_MAGIC_ADDR;

    *reset_addr = RIOTBOOT_MAGIC_NUMBER;
    pm_reboot();
}

void tud_dfu_runtime_reboot_to_dfu_cb(void)
{
    /* the host sent a DFU_DETACH request */
    tud_dfu_detach_cb();
}
