/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup fido2_ctap_transport
 * @{
 * @file
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 * @}
 */

#include "fido2/ctap/transport/ctap_transport.h"
#include "fido2/ctap.h"

#if CONFIG_CTAP_HID
#include "usb/usbus.h"
#include "fido2/ctap/transport/hid/ctap_transport_hid.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if CONFIG_CTAP_HID
static char usb_stack[USBUS_STACKSIZE];
static usbus_t usbus;
size_t usb_hid_io_write(const void *buffer, size_t size);
int usb_hid_io_read_timeout(void *buffer, size_t size, uint32_t timeout);
void usb_hid_io_init(usbus_t *usbus, const uint8_t *report_desc,
                     size_t report_desc_size);
#endif

static void create_usb(const void *report_desc, size_t len);
static void work_usb(void);
static void *pkt_loop(void *arg);

static char g_stack[CTAP_TRANSPORT_STACKSIZE];
static kernel_pid_t g_pid;

static void *pkt_loop(void *arg)
{
    (void)arg;
    int ret;

    ret = fido2_ctap_init();

    if (ret < 0) {
        return (void *)0;
    }

#if CONFIG_CTAP_HID
    fido2_ctap_transport_hid_create();
#endif

    DEBUG("fido2_ctap_transport: initialization done. Entering work loop \n");

    for (;;) {
#if CONFIG_CTAP_HID
        work_usb();
#endif
    }

    return (void *)0;
}

static void work_usb(void)
{
    uint8_t buffer[CONFIG_USBUS_HID_INTERRUPT_EP_SIZE];
    int read;

    read = fido2_ctap_transport_read_timeout(CTAP_TRANSPORT_USB, buffer,
                                             CONFIG_USBUS_HID_INTERRUPT_EP_SIZE,
                                             CTAP_HID_TRANSACTION_TIMEOUT);

    if (read == CONFIG_USBUS_HID_INTERRUPT_EP_SIZE) {
        fido2_ctap_transport_hid_handle_packet(buffer, read);
    }

    fido2_ctap_transport_hid_check_timeouts();
}

void fido2_ctap_transport_init(void)
{
    g_pid = thread_create(g_stack, sizeof(g_stack), THREAD_PRIORITY_MAIN,
                          THREAD_CREATE_STACKTEST, pkt_loop, NULL,
                          "fido2_ctap_transport_loop");
}

int fido2_ctap_transport_create(uint8_t type, void *arg, size_t size)
{
    switch (type) {
    case CTAP_TRANSPORT_USB:
        create_usb(arg, size);
        break;
    default:
        return CTAP1_ERR_OTHER;
    }

    return CTAP2_OK;
}

int fido2_ctap_transport_read_timeout(uint8_t type, void *buffer, size_t size,
                                      uint32_t timeout)
{
    switch (type) {
    case CTAP_TRANSPORT_USB:
        return usb_hid_io_read_timeout(buffer, size, timeout);
    default:
        return CTAP1_ERR_OTHER;
    }
}

int fido2_ctap_transport_write(uint8_t type, const void *buffer, size_t size)
{
    switch (type) {
    case CTAP_TRANSPORT_USB:
        usb_hid_io_write(buffer, size);
        return CTAP2_OK;
    default:
        return CTAP1_ERR_OTHER;
    }
}

static void create_usb(const void *report_desc, size_t len)
{
    usbdev_t *usbdev = usbdev_get_ctx(0);

    assert(usbdev);
    usbus_init(&usbus, usbdev);
    usb_hid_io_init(&usbus, report_desc, len);

    usbus_create(usb_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);
}
