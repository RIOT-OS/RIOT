/*
 * SPDX-FileCopyrightText: 2019 Koen Zandberg <koen@bergzand.net>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests USB interaction with USBUS
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "mutex.h"
#include "periph/usbdev.h"
#include "embUnit.h"

#include "usbdev_mock.h"
#include "usb/descriptor.h"
#include "usb/usbus.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief Test case states
 */
typedef enum {
    TESTPHASE_START,        /**< Initial start */
    TESTPHASE_RESET,        /**< Signal USB RESET */
    TESTPHASE_REQ_DEV_INIT, /**< Request the first part of the device descriptor */
    TESTPHASE_RESET2,       /**< Second reset */
    TESTPHASE_SET_ADDRESS,  /**< Configure Address */
    TESTPHASE_REQ_DEV_FULL, /**< Request the full device descriptor */
    TESTPHASE_FINAL,        /**< Testing done */
} testphase_t;

/**
 * @brief Test control request phases
 */
typedef enum {
    TEST_REQ_PHASE_IDLE,
    TEST_REQ_PHASE_START_GET,
    TEST_REQ_PHASE_START_SET,
    TEST_REQ_PHASE_OUTDATA,
    TEST_REQ_PHASE_INDATA,
    TEST_REQ_PHASE_OUTACK,
    TEST_REQ_PHASE_INACK,
} test_req_phase_t;

testphase_t phase = TESTPHASE_START;
test_req_phase_t req_phase = TEST_REQ_PHASE_IDLE;

static usbus_t usbus;
static char _stack[USBUS_STACKSIZE];

static mutex_t waiter = MUTEX_INIT;

static char *_state2str(test_req_phase_t state)
{
    switch (state) {
        case TEST_REQ_PHASE_IDLE:
            return "idle";
        case TEST_REQ_PHASE_START_GET:
            return "start get";
        case TEST_REQ_PHASE_START_SET:
            return "start set";
        case TEST_REQ_PHASE_OUTDATA:
            return "OUT data";
        case TEST_REQ_PHASE_INDATA:
            return "IN data";
        case TEST_REQ_PHASE_OUTACK:
            return "OUT Ack";
        case TEST_REQ_PHASE_INACK:
            return "IN Ack";
    }
    return "unknown control request state";
}

static void _build_conf_req(uint8_t *buf, size_t reqlen)
{
    usb_setup_t *setup = (usb_setup_t *)buf;

    memset(setup, 0, sizeof(usb_setup_t));
    setup->type = USB_SETUP_REQUEST_DEVICE2HOST;
    setup->request = USB_SETUP_REQ_GET_DESCRIPTOR;
    setup->value = USB_TYPE_DESCRIPTOR_DEVICE << 8;
    setup->length = reqlen;
}

static void _build_set_addr(uint8_t *buf)
{
    usb_setup_t *setup = (usb_setup_t *)buf;

    memset(setup, 0, sizeof(usb_setup_t));
    setup->request = USB_SETUP_REQ_SET_ADDRESS;
    setup->value = 42;
}

static void _start_setup_get(usbdev_mock_t *dev)
{
    req_phase = TEST_REQ_PHASE_START_GET;
    dev->in[0].available = 0;
    /* Trigger the OUT request at the USBUS stack */
    dev->usbdev.epcb(&dev->out[0].ep, USBDEV_EVENT_ESR);
}

static void _start_setup_set(usbdev_mock_t *dev)
{
    req_phase = TEST_REQ_PHASE_START_SET;
    dev->in[0].available = 0;
    dev->usbdev.epcb(&dev->out[0].ep, USBDEV_EVENT_ESR);
}

/* Validates the first 8 bytes of the device descriptor */
static void _validate_device_desc_init(usb_descriptor_device_t *desc)
{
    TEST_ASSERT_EQUAL_INT(sizeof(usb_descriptor_device_t), desc->length);
    TEST_ASSERT_EQUAL_INT(USB_TYPE_DESCRIPTOR_DEVICE, desc->type);
}

static void _test_sequence(usbdev_mock_t *dev)
{
    testphase_t next_phase = TESTPHASE_FINAL;

    /* Global sequence control */
    /* Called as part of the ESR handling */
    switch (phase) {
        case TESTPHASE_START:
            next_phase = TESTPHASE_RESET;
            /* Signal reset condition */
            DEBUG("[test]: Signalling USB reset condition\n");
            dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_RESET);
            dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_ESR);
            break;
        case TESTPHASE_RESET:
            next_phase = TESTPHASE_REQ_DEV_INIT;
            DEBUG("[test]: Requesting device descriptor\n");
            _build_conf_req(dev->out[0].buf, 8); /* initial config request */
            dev->req_len = 8;
            dev->out[0].state = EP_STATE_DATA_AVAILABLE;
            dev->out[0].available = 8;
            _start_setup_get(dev);
            break;
        case TESTPHASE_REQ_DEV_INIT:
            next_phase = TESTPHASE_RESET2;
            DEBUG("[test]: validating device descriptor\n");
            TEST_ASSERT_EQUAL_INT(dev->in[0].available, 8);
            _validate_device_desc_init(
                (usb_descriptor_device_t *)dev->in[0].buf);
            /* Reset device */
            DEBUG("[test]: Signalling second USB reset condition\n");
            dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_RESET);
            dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_ESR);
            break;
        case TESTPHASE_RESET2:
            next_phase = TESTPHASE_SET_ADDRESS;
            DEBUG("[test]: Set USB address\n");
            _build_set_addr(dev->out[0].buf);
            dev->req_len = 0;
            dev->out[0].state = EP_STATE_DATA_AVAILABLE;
            dev->out[0].available = 8;
            _start_setup_set(dev);
            break;
        case TESTPHASE_SET_ADDRESS:
            next_phase = TESTPHASE_REQ_DEV_FULL;
            DEBUG("[test]: Requesting full device descriptor\n");
            _build_conf_req(dev->out[0].buf,
                            sizeof(usb_descriptor_device_t));
            dev->req_len = sizeof(usb_descriptor_device_t);
            dev->out[0].state = EP_STATE_DATA_AVAILABLE;
            dev->out[0].available = sizeof(usb_setup_t);
            _start_setup_get(dev);
            break;
        case TESTPHASE_REQ_DEV_FULL:
            TEST_ASSERT_EQUAL_INT(dev->in[0].available,
                                  sizeof(usb_descriptor_device_t));
            DEBUG("[test]: Validating full descriptor\n");
            _validate_device_desc_init(
                (usb_descriptor_device_t *)dev->in[0].buf);

            next_phase = TESTPHASE_FINAL;
            dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_ESR);
            break;

        case TESTPHASE_FINAL:
            /* Testing done */
            mutex_unlock(&waiter);
            break;
        default:
            DEBUG("[test]: Unhandled situation in test script\n");
    }
    phase = next_phase;
}

/* Validates the ready call after submitting a setup request */
static void _validate_get_req_start(usbdev_mock_ep_t *ep)
{
    DEBUG("[validation]: get request validation\n");

    /* We should get an IN request with the data */
    TEST_ASSERT_EQUAL_INT(ep->ep.dir, USB_EP_DIR_IN);
    req_phase = TEST_REQ_PHASE_INDATA;
}

static void _validate_out_ack(usbdev_mock_t *dev, usbdev_mock_ep_t *ep)
{
    DEBUG("[validation]: OUT ACK validation\n");

    TEST_ASSERT_EQUAL_INT(ep->ep.dir, USB_EP_DIR_OUT);
    dev->usbdev.epcb(&dev->out[0].ep, USBDEV_EVENT_ESR);
    req_phase = TEST_REQ_PHASE_IDLE;
}

static void _validate_in_ack(usbdev_mock_t *dev, usbdev_mock_ep_t *ep,
                             size_t len)
{
    DEBUG("[validation]: IN ACK validation\n");

    TEST_ASSERT_EQUAL_INT(ep->ep.dir, USB_EP_DIR_IN);
    TEST_ASSERT_EQUAL_INT(len, 0);
    dev->usbdev.epcb(&dev->in[0].ep, USBDEV_EVENT_ESR);
    req_phase = TEST_REQ_PHASE_IDLE;
}

static void _validate_set_req_start(usbdev_mock_t *dev, usbdev_mock_ep_t *ep,
                                    size_t len)
{
    DEBUG("[validation]: set request validation\n");

    TEST_ASSERT_EQUAL_INT(dev->out[0].available, 8);
    /* We should get an IN request with the data */
    TEST_ASSERT_EQUAL_INT(ep->ep.dir, USB_EP_DIR_IN);
    if (dev->req_len == 0) {
        DEBUG("[validation]: No payload, performing IN ACK validation\n");
        /* No data phase, this `ready` is the OUT ACK */
        _validate_in_ack(dev, ep, len);
    }
    else {
        req_phase = TEST_REQ_PHASE_OUTDATA;
    }
}

static void _handle_data(usbdev_mock_t *dev, usbdev_mock_ep_t *ep, size_t len)
{
    if (ep->ep.dir == USB_EP_DIR_IN) {
        /* Zero length IN packet is not valid here */
        DEBUG("[data]: Handling IN data from stack: %" PRIuSIZE "\n", len);
        TEST_ASSERT(len != 0);
        dev->usbdev.epcb(&dev->in[0].ep, USBDEV_EVENT_ESR);
        if (ep->available == dev->req_len) {
            DEBUG("[data]: Full data received from stack\n");
            req_phase = TEST_REQ_PHASE_OUTACK;
        }
        else {
            DEBUG("[data]: Expecting more data from stack: %" PRIuSIZE "/%" PRIuSIZE "\n",
                  ep->available, dev->req_len);
        }
    }
}

static void _ep_esr_validation(usbdev_mock_t *dev, usbdev_mock_ep_t *ep)
{
    DEBUG("[ep esr]: Data available for stack: %u\n", (unsigned)ep->available);
    if (req_phase == TEST_REQ_PHASE_IDLE) {
        DEBUG("[ep esr]: Done with the request\n");
        /* signal USBDEV_EVENT_ESR to call _test_sequence */
        dev->usbdev.cb(&dev->usbdev, USBDEV_EVENT_ESR);
    }
    ep->ep.dev->epcb(&ep->ep, USBDEV_EVENT_TR_COMPLETE);
}

/* Handles Control endpoint validation from the usbdev side. *
 * Called as part of the endpoint ready function.            */
static void _ep0_validation(usbdev_mock_t *dev, usbdev_mock_ep_t *ep,
                            size_t len)
{
    DEBUG("[validation]: req state: %s\n", _state2str(req_phase));
    /* Validate direction and content for the current state */
    switch (req_phase) {
        case TEST_REQ_PHASE_START_GET:
            _validate_get_req_start(ep);
            _handle_data(dev, ep, len);
            break;
        case TEST_REQ_PHASE_START_SET:
            _validate_set_req_start(dev, ep, len);
            break;
        case TEST_REQ_PHASE_OUTACK:
            _validate_out_ack(dev, ep);
            break;
        case TEST_REQ_PHASE_INACK:
            _validate_in_ack(dev, ep, len);
            break;
        default:
            /* Nothing is validated at the moment for the cases of:
             * - TEST_REQ_PHASE_IDLE
             * - TEST_REQ_PHASE_INDATA
             * - TEST_REQ_PHASE_OUTDATA
             */
            break;
    }

}

static void _setup(void)
{
    mutex_lock(&waiter);
}

static void _test(void)
{
    _setup();
    /* Initialize basic usbus struct, don't start the thread yet */
    usbdev_t *usbdev = usbdev_get_ctx(0);
    usbus_init(&usbus, usbdev);
    usbdev_mock_setup(&_test_sequence, &_ep_esr_validation, &_ep0_validation);

    usbus_create(_stack, USBUS_STACKSIZE, USBUS_PRIO, USBUS_TNAME, &usbus);

    /* Try to lock to block until the test thread has finished */
    mutex_lock(&waiter);
}

static Test *tests_usbus(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(_test),
    };
    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);

    return (Test *)&tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_usbus());
    TESTS_END();

    return 0;
}
