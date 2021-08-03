/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief
 * @{
 *
 * @brief       Tests for USB HID
 *
 * @author      Nils Ollrogge <nils.ollrogge@fu-berlin.de>
 */

#include <stdio.h>
#include <stdlib.h>

#include "usb/usbus.h"
#include "xtimer.h"
#include "usb/usbus/hid.h"
#include "usb/usbus/hid_io.h"

/*
   this descriptor is used, because the basic usb_hid interface was developed in
   conjunction with FIDO2. Descriptor is taken from CTAP2 specification
   (version 20190130) section 8.1.8.2
 */
static const uint8_t report_desc_ctap[] = {
    0x06, 0xD0, 0xF1,   /* HID_UsagePage ( FIDO_USAGE_PAGE ) */
    0x09, 0x01,         /* HID_Usage ( FIDO_USAGE_CTAPHID ) */
    0xA1, 0x01,         /* HID_Collection ( HID_Application ) */
    0x09, 0x20,         /* HID_Usage ( FIDO_USAGE_DATA_IN ) */
    0x15, 0x00,         /* HID_LogicalMin ( 0 ) */
    0x26, 0xFF, 0x00,   /* HID_LogicalMaxS ( 0xff ) */
    0x75, 0x08,         /* HID_ReportSize ( 8 ) */
    0x95, 0x40,         /* HID_ReportCount ( HID_INPUT_REPORT_BYTES ) */
    0x81, 0x02,         /* HID_Input ( HID_Data | HID_Absolute | HID_Variable ) */
    0x09, 0x21,         /* HID_Usage ( FIDO_USAGE_DATA_OUT ) */
    0x15, 0x00,         /* HID_LogicalMin ( 0 ) */
    0x26, 0xFF, 0x00,   /* HID_LogicalMaxS ( 0xff ) */
    0x75, 0x08,         /* HID_ReportSize ( 8 ) */
    0x95, 0x40,         /* HID_ReportCount ( HID_OUTPUT_REPORT_BYTES ) */
    0x91, 0x02,         /* HID_Output ( HID_Data | HID_Absolute | HID_Variable ) */
    0xC0,               /* HID_EndCollection */
};

static usbus_t usbus;
static char _stack[USBUS_STACKSIZE];
static char test_arg[] = { "Test argument" };

static void rx_cb(void *arg)
{
    printf("USB_HID rx_cb: %s \n", (char *)arg);
}

static void init(void)
{
    usbdev_t *usbdev = usbdev_get_ctx(0);

    usbus_init(&usbus, usbdev);
    usb_hid_io_init(&usbus, report_desc_ctap, sizeof(report_desc_ctap));
    usb_hid_io_set_rx_cb(rx_cb, test_arg);
    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);
}

int main(void)
{
    /* sleep to wait for Pyterm attaching in order to see puts messages */
    xtimer_sleep(3);
    init();
    puts("RIOT USB HID echo test");
    puts(
        "Write input to the hidraw device under /dev/hidrawX, and see if it gets echoed here");

    uint8_t buffer[CONFIG_USBUS_HID_INTERRUPT_EP_SIZE];

    for (;;) {
        ssize_t len =
            usb_hid_io_read(buffer, CONFIG_USBUS_HID_INTERRUPT_EP_SIZE);

        puts("Msg received via USB HID: ");
        for (int i = 0; i < len; i++) {
            putc(buffer[i], stdout);
        }
        puts("");
    }
}
