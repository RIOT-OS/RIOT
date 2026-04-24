/*
 * SPDX-FileCopyrightText: 2020 Nalys
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for the candev abstraction
 *
 * @author      Wouter Symons <wosym@airsantelmo.com>
 * @author      Toon Stegen <tstegen@nalys-group.com>
 *
 * @}
 */

#include <isrpipe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "candev_mcp2515.h"
#include "periph/can.h"
#include "shell.h"
#include "test_utils/expect.h"

/* The params header is only in the include path when the module is used */
#if MODULE_MCP2515
#  include "mcp2515_params.h"
#endif

#if MODULE_PERIPH_CAN
#  include "can_params.h"
#  include "periph_conf.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/* Default is not using loopback test mode */
#ifndef CONFIG_USE_LOOPBACK_MODE
#  define CONFIG_USE_LOOPBACK_MODE  0
#endif

#ifndef CONFIG_CAN_DEV
#  define CONFIG_CAN_DEV 0
#endif

/**
 * @brief Standard Filter Number
 *
 * Default value is 4, but it can be increased up
 * to the maximum supported by the CAN controller.
 */
#ifndef CONFIG_FDCAN_STD_FILTERS_NUM
#  define CONFIG_FDCAN_STD_FILTERS_NUM              4
#endif

#define RX_RINGBUFFER_SIZE 128      /* Needs to be a power of 2! */
static isrpipe_t rxbuf;
static uint8_t rx_ringbuf[RX_RINGBUFFER_SIZE];

static candev_t *candev = NULL;

/* Only one of them is actually used, depending on the driver selected.
 * We rely on the compiler to garbage collect the unused */
static can_t periph_dev;
static candev_mcp2515_t mcp2515_dev;

/* The params header is only in the include path when the module is used,
 * so we fall back to a NULL ptr if not */
#if MODULE_MCP2515
static const candev_mcp2515_conf_t *mcp2515_conf = &candev_mcp2515_conf[CONFIG_CAN_DEV];
#else
static const candev_mcp2515_conf_t *mcp2515_conf = NULL;
#endif

#if MODULE_PERIPH_CAN
static const can_conf_t *periph_can_conf = &(candev_conf[CONFIG_CAN_DEV]);
#else
static const can_conf_t *periph_can_conf = NULL;
#endif

/**
 * @brief Convert a @ref can_t into an @ref candev_t
 * @param[in]   dev     Peripheral CAN to get the candev_t of
 * @return              The corresponding @ref candev_t
 *
 * @details     Either @ref can_t is an alias to @ref candev_t, or `HAVE_CAN_T`
 *              is defined `can_t` has a member `candev_t candev`.
 */
static candev_t *_can_t2candev_t(can_t *dev);

#ifdef HAVE_CAN_T
static candev_t *_can_t2candev_t(can_t *dev)
{
    return &dev->candev;
}
#else
static candev_t *_can_t2candev_t(can_t *dev)
{
    return dev;
}
#endif

static int _send(int argc, char **argv)
{
    int ret;

    can_frame_t frame = { 0 };   /* zero everything */

    if (argc < 2) {
        puts("Usage: send <can_id> [data bytes...]");
        return -1;
    }

    frame.can_id = strtol(argv[1], NULL, 0);

    for (int i = 2; i < argc && frame.len < CAN_MAX_DLEN; i++) {
        frame.data[frame.len++] = atoi(argv[i]);
    }

    ret = candev->driver->send(candev, &frame);
    if (ret < 0) {
        puts("Failed to send CAN-message!");
    }
    else {
        DEBUG("sent using mailbox: %d\n", ret);
    }

    return 0;
}

static int _receive(int argc, char **argv)
{
    int n = 1;

    if (argc > 1) {
        n = strtol(argv[1], NULL, 10);
        if (n < 1) {
            puts("Usage: receive <number>");
            return -1;
        }
    }

    for (int i = 0; i < n; i++) {
        can_frame_t frame;

        puts("Reading from Rxbuf...");
        isrpipe_read(&rxbuf, (uint8_t *)&(frame.can_id), sizeof(frame.can_id));
        frame.can_id &= 0x1FFFFFFF; /* clear invalid bits */
        isrpipe_read(&rxbuf, (uint8_t *)&(frame.len), 1);
        printf("id: %" PRIx32 " dlc: %" PRIx8, frame.can_id, frame.len);
        if (frame.len > 0) {
            printf(" data: ");
            isrpipe_read(&rxbuf, frame.data, frame.len); /* data */
            for (int i = 0; i < frame.len; i++) {
                printf("0x%X ", frame.data[i]);
            }
        }
        putchar('\n');
    }

    return 0;
}

static int _set_bit_rate(int argc, char **argv)
{
    uint32_t bitrate = 250000;
    uint32_t sample_point = 875;
    int res = 0;

    if (argc == 2) {
        bitrate = atoi(argv[1]);
        if (!bitrate) {
            puts("Usage error: Invalid bitrate");
            return -1;
        }
    }
    if (argc == 3) {
        bitrate = atoi(argv[1]);
        sample_point = atoi(argv[2]);
        if (!bitrate || !sample_point) {
            puts("Usage error: Invalid bitrate or sample point");
            return -2;
        }
    }

    struct can_bittiming bit_timing = {
        .bitrate = bitrate,
        .sample_point = sample_point,
    };

    res = candev->driver->set(candev, CANOPT_BITTIMING, &bit_timing, sizeof(bit_timing));

    return res;
}

static int _set_can_filter(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: set_filter <can_id> [mask] [mailbox]");
        return -1;
    }

    static struct can_filter filter; // static!
    filter.can_id = strtoul(argv[1], NULL, 0);
    filter.can_mask = (argc >= 3) ? strtoul(argv[2], NULL, 0) : 0x7FF;

#if (MODULE_CAN_RX_MAILBOX)
    filter.target_mailbox = (argc >= 4) ? atoi(argv[3]) : 0;
#endif

    int res = candev->driver->set_filter(candev, &filter);
    if (res < 0) {
        puts("Failed to set CAN filter");
    }
    else {
        printf("CAN filter set: ID 0x%" PRIx32 " Mask 0x%" PRIx32 "\n",
               filter.can_id, filter.can_mask);
    }
    return res;
}

static int _remove_can_filter(int argc, char **argv)
{
    if (argc < 2) {
        puts("Usage: remove_filter <can_id>");
        return -1;
    }

    struct can_filter filter;

    /* Only CAN ID is required */
    filter.can_id = strtoul(argv[1], NULL, 0);

    /* Mask is ignored by remove_filter in most drivers,
     * but initialize defensively */
    filter.can_mask = 0x7FF;

#if (MODULE_CAN_RX_MAILBOX)
    filter.target_mailbox = 0;
#endif

    int res = candev->driver->remove_filter(candev, &filter);
    if (res < 0) {
        puts("Failed to remove CAN filter");
        return res;
    }

    printf("CAN filter removed: ID 0x%" PRIx32 "\n", filter.can_id);
    return 0;
}

static int _list_filters(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    struct can_filter filters[CONFIG_FDCAN_STD_FILTERS_NUM]; /* Max filters supported */
    int res = candev->driver->get(candev, CANOPT_RX_FILTERS, &filters, sizeof(filters));
    if (res < 0) {
        puts("Failed to read CAN filters");
        return res;
    }
    else if (res == 0) {
        puts("No CAN filters configured");
        return 0;
    }
    else {
        for (int i = 0; i < res; i++) {
            printf("[%d] CAN Filter ID: 0x%" PRIx32 " Mask: 0x%" PRIx32 "\n",
                i, filters[i].can_id, filters[i].can_mask);
        }
    }
    return 0;
}

static int _power_off(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res = 0;

    canopt_state_t state = CANOPT_STATE_OFF;
    res = candev->driver->set(candev, CANOPT_STATE, &state, sizeof(canopt_state_t));

    return res;
}

static int _power_on(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    int res = 0;

    canopt_state_t state = CANOPT_STATE_ON;
    res = candev->driver->set(candev, CANOPT_STATE, &state, sizeof(canopt_state_t));

    return res;
}

static const shell_command_t shell_commands[] = {
    { "on", "Turn on the CAN controller", _power_on},
    { "off", "Turn off the CAN controller", _power_off},
    { "set_filter", "set CAN filters", _set_can_filter},
    { "set_bit_rate", "set CAN bit rate", _set_bit_rate},
    { "send", "send some data", _send },
    { "remove_filter", "remove CAN filter by ID", _remove_can_filter},
    { "list_filters", "list RX filters", _list_filters},
    { "receive", "receive some data", _receive },
    { NULL, NULL, NULL }
};

static void _can_event_callback(candev_t *dev, candev_event_t event, void *arg)
{
    (void)arg;
    can_frame_t *frame;

    switch (event) {
    case CANDEV_EVENT_ISR:
        DEBUG("_can_event: CANDEV_EVENT_ISR\n");
        dev->driver->isr(candev);
        break;
    case CANDEV_EVENT_WAKE_UP:
        DEBUG("_can_event: CANDEV_EVENT_WAKE_UP\n");
        break;
    case CANDEV_EVENT_TX_CONFIRMATION:
        DEBUG("_can_event: CANDEV_EVENT_TX_CONFIRMATION\n");
        break;
    case CANDEV_EVENT_TX_ERROR:
        DEBUG("_can_event: CANDEV_EVENT_TX_ERROR\n");
        break;
    case CANDEV_EVENT_RX_INDICATION:
        DEBUG("_can_event: CANDEV_EVENT_RX_INDICATION\n");

        frame = (can_frame_t *)arg;

        DEBUG("            id: %" PRIx32 " dlc: %u data: ", frame->can_id & 0x1FFFFFFF,
              frame->len);
        for (uint8_t i = 0; i < frame->len; i++) {
            DEBUG("0x%X ", frame->data[i]);
        }
        DEBUG_PUTS("");

        /* Store in buffer until user requests the data */
        isrpipe_write(&rxbuf, (uint8_t *)&(frame->can_id), sizeof(frame->can_id));
        isrpipe_write_one(&rxbuf, frame->len);
        isrpipe_write(&rxbuf, frame->data, frame->len);

        break;
    case CANDEV_EVENT_RX_ERROR:
        DEBUG("_can_event: CANDEV_EVENT_RX_ERROR\n");
        break;
    case CANDEV_EVENT_BUS_OFF:
        DEBUG("_can_event: CANDEV_EVENT_BUS_OFF\n");
        dev->state = CAN_STATE_BUS_OFF;
        break;
    case CANDEV_EVENT_ERROR_PASSIVE:
        DEBUG("_can_event: CANDEV_EVENT_ERROR_PASSIVE\n");
        dev->state = CAN_STATE_ERROR_PASSIVE;
        break;
    case CANDEV_EVENT_ERROR_WARNING:
        DEBUG("_can_event: CANDEV_EVENT_ERROR_WARNING\n");
        dev->state = CAN_STATE_ERROR_WARNING;
        break;
    default:
        DEBUG("_can_event: unknown event\n");
        break;
    }
}

int main(void)
{
    puts("candev test application\n");

    isrpipe_init(&rxbuf, (uint8_t *)rx_ringbuf, sizeof(rx_ringbuf));
    if (IS_USED(MODULE_PERIPH_CAN)) {
        puts("Initializing CAN periph device");
        can_init(&periph_dev, periph_can_conf);
        candev = _can_t2candev_t(&periph_dev);
    }
    else if (IS_USED(MODULE_MCP2515)) {
        puts("Initializing MCP2515");
        candev_mcp2515_init(&mcp2515_dev, mcp2515_conf);
        candev = &mcp2515_dev.candev;
    }
    else {
        /* No CAN driver is used or used CAN driver is not integrated in this
         * test yet. We use an undefined function name to let this fail at
         * compile time. The conditions above are all compile time constants
         * and the compiler will eliminate the dead branches. So if any of them
         * matched, this function call will not be part of the compiled object
         * file and linking will work. */
        extern void the_can_test_apps_depends_on_a_supported_can_driver_but_none_is_used(void);
        the_can_test_apps_depends_on_a_supported_can_driver_but_none_is_used();
    }

    expect(candev);

    candev->event_callback = _can_event_callback;
    candev->isr_arg = NULL;

    candev->driver->init(candev);

if (IS_ACTIVE(CONFIG_USE_LOOPBACK_MODE)) {
    puts("Switching to loopback mode");
    /* set to loopback test mode */
    canopt_state_t mode = CANOPT_STATE_LOOPBACK;
    candev->driver->set(candev, CANOPT_STATE, &mode, sizeof(mode));
}

/* Depending from the CAN controller used, this test example will provide different results.
- For MCP2515 standalone CAN controller, the last filter won't be applied
as the first reception mailbox supports up to two filters
- For SAMD5x/E5x CAN controller, and with keeping the default parameters in candev_samd5x.h,
the last filter won't be applied as the CAN controller supports up to 3 standard filters
- For SAMD5x/E5x CAN controller, if you increase the maximum capacity of the standard
filters (check Makefile.board.dep), the last filter can be applied correctly. */
#if defined(MODULE_MCP2515)
    if (IS_ACTIVE(MCP2515_RECV_FILTER_EN)) {
#endif
        /* CAN filters examples */
        struct can_filter filter[4];
        filter[0].can_mask = 0x7FF;
        filter[0].can_id = 0x001;
#if (MODULE_CAN_RX_MAILBOX)
        filter[0].target_mailbox = 0;
#endif
        filter[1].can_mask = 0x7FF;
        filter[1].can_id = 0x002;
#if (MODULE_CAN_RX_MAILBOX)
        filter[1].target_mailbox = 1;
#endif
        filter[2].can_mask = 0x7FF;
        filter[2].can_id = 0x003;
#if (MODULE_CAN_RX_MAILBOX)
        filter[2].target_mailbox = 0;
#endif
        filter[3].can_mask = 0x7FF;
        filter[3].can_id = 0x004;
#if (MODULE_CAN_RX_MAILBOX)
        filter[3].target_mailbox = 1;
#endif
        for (uint8_t i = 0; i < 4; i++) {
            candev->driver->set_filter(candev, &filter[i]);
        }
        /* All other messages won't be received */
#if defined(MODULE_MCP2515)
    }
#endif

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
