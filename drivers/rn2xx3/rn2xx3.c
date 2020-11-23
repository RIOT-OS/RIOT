/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_rn2xx3
 * @{
 *
 * @file
 * @brief       Driver implementation for the RN2483/RN2903 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <errno.h>

#include "assert.h"
#include "xtimer.h"
#include "fmt.h"
#include "kernel_defines.h"

#include "rn2xx3_params.h"
#include "rn2xx3.h"
#include "rn2xx3_internal.h"

#define ENABLE_DEBUG    0
/* Warning: to correctly display the debug message from sleep timer callback,,
   add CFLAGS+=-DTHREAD_STACKSIZE_IDLE=THREAD_STACKSIZE_DEFAULT to the build
   command.
*/
#include "debug.h"

/**
 * @brief   Delay when resetting the device, 10ms
 */
#define RESET_DELAY     (10UL * US_PER_MS)

/*
 * Interrupt callbacks
 */
static void _rx_cb(void *arg, uint8_t c)
{
    rn2xx3_t *dev = (rn2xx3_t *)arg;
    netdev_t *netdev = (netdev_t *)dev;

    /* Avoid overflow of module response buffer */
    if (dev->resp_size >= RN2XX3_MAX_BUF) {
        return;
    }

    /* The device closes the response with \r\n when done */
    if (c == '\r') {
        return;
    }
    else if (c == '\n') { /* response end delimiter */
        dev->resp_buf[dev->resp_size++] = '\0';
        if (dev->int_state == RN2XX3_INT_STATE_MAC_RX_MESSAGE) {
            /* RX state: closing RX buffer */
            dev->rx_buf[(dev->rx_size + 1) / 2] = 0;
            netdev_trigger_event_isr(netdev);
        }
        else if (dev->int_state == RN2XX3_INT_STATE_MAC_TX) {
            /* still in TX state: transmission complete but no data received */
            netdev_trigger_event_isr(netdev);
        }
        dev->resp_size = 0;
        dev->rx_size = 0;
        dev->resp_done = 1;
        mutex_unlock(&(dev->resp_lock));
    }
    else {
        switch (dev->int_state) {
            /* A successful mac TX command expects 2 kinds of replies:
                 * - mac_tx_ok: transmission done, no data received
                 * - mac_rx <port> <data>: transmission done, some data
                 *                         received on port <port>. */
            case RN2XX3_INT_STATE_MAC_TX:
                /* some data are available */
                dev->resp_buf[dev->resp_size++] = c;
                /* if module response ends with 'rx ' after 8 received chars,
                   the module starts receiving data */
                if (dev->resp_size == 8 && dev->resp_buf[4] == 'r'
                    && dev->resp_buf[5] == 'x' && dev->resp_buf[6] == ' ') {
                    /* next received chars correspond to the port number */
                    dev->int_state = RN2XX3_INT_STATE_MAC_RX_PORT;
                }
                break;

            case RN2XX3_INT_STATE_MAC_RX_PORT:
                dev->resp_buf[dev->resp_size++] = c;
                if (c == ' ') {
                    dev->int_state = RN2XX3_INT_STATE_MAC_RX_MESSAGE;
                }
                break;

            case RN2XX3_INT_STATE_MAC_RX_MESSAGE:
                /* read and convert RX data (received in hex chars) */
                if (c == ' ') {
                    dev->resp_buf[dev->resp_size++] = c;
                    break;
                }
                dev->rx_tmp_buf[dev->rx_size % 2] = c;

                /* We convert pairs of hex character to bytes on the fly to
                   save space in memory */
                if (dev->rx_size != 0 && dev->rx_size % 2) {
                    rn2xx3_hex_to_bytes(dev->rx_tmp_buf,
                                        &dev->rx_buf[(dev->rx_size - 1) / 2]);
                }
                dev->rx_size++;
                break;

            default:
                dev->resp_buf[dev->resp_size++] = c;
                break;
        }
    }
}

static void _sleep_timer_cb(void *arg)
{
    DEBUG("[rn2xx3] exit sleep\n");
    rn2xx3_t *dev = (rn2xx3_t *)arg;
    dev->int_state = RN2XX3_INT_STATE_IDLE;
}

/*
 * Driver's "public" functions
 */
void rn2xx3_setup(rn2xx3_t *dev, const rn2xx3_params_t *params)
{
    assert(dev && (params->uart < UART_NUMOF));

    /* initialize device parameters */
    dev->p = *params;

    /* initialize pins and perform hardware reset */
    if (gpio_is_valid(dev->p.pin_reset)) {
        gpio_init(dev->p.pin_reset, GPIO_OUT);
        gpio_set(dev->p.pin_reset);
    }
    /* UART is initialized later, since interrupts cannot be handled yet */
}

int rn2xx3_init(rn2xx3_t *dev)
{
    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_RESET);

    /* initialize buffers and locks*/
    dev->resp_size = 0;
    dev->cmd_buf[0] = 0;

    /* initialize UART and GPIO pins */
    if (uart_init(dev->p.uart, dev->p.baudrate, _rx_cb, dev) != UART_OK) {
        DEBUG("[rn2xx3] init: error initializing UART\n");
        return -ENXIO;
    }

    /* if reset pin is connected, do a hardware reset */
    if (gpio_is_valid(dev->p.pin_reset)) {
        gpio_clear(dev->p.pin_reset);
        xtimer_usleep(RESET_DELAY);
        gpio_set(dev->p.pin_reset);
    }

    dev->sleep_timer.callback = _sleep_timer_cb;
    dev->sleep_timer.arg = dev;

    rn2xx3_sys_set_sleep_duration(dev, CONFIG_RN2XX3_DEFAULT_SLEEP);

    /* sending empty command to clear uart buffer */
    if (rn2xx3_write_cmd(dev) == RN2XX3_TIMEOUT) {
        DEBUG("[rn2xx3] init: initialization failed\n");
        return RN2XX3_TIMEOUT;
    }

    if (rn2xx3_mac_init(dev) != RN2XX3_OK) {
        DEBUG("[rn2xx3] mac: initialization failed\n");
        return RN2XX3_ERR_MAC_INIT;
    }

    DEBUG("[rn2xx3] init: initialization successful\n");
    return RN2XX3_OK;
}

int rn2xx3_sys_reset(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "sys reset");
    dev->cmd_buf[p] = 0;
    int ret = rn2xx3_write_cmd(dev);
    if (ret == RN2XX3_TIMEOUT || ret == RN2XX3_ERR_SLEEP_MODE) {
        DEBUG("[rn2xx3] reset: failed\n");
        return ret;
    }

    return RN2XX3_OK;
}

int rn2xx3_sys_factory_reset(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "sys factoryRESET");
    dev->cmd_buf[p] = 0;
    int ret = rn2xx3_write_cmd(dev);
    if (ret == RN2XX3_TIMEOUT || ret == RN2XX3_ERR_SLEEP_MODE) {
        DEBUG("[rn2xx3] factory reset: failed\n");
        return ret;
    }

    return RN2XX3_OK;
}

int rn2xx3_sys_sleep(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "sys sleep %" PRIu32 "", dev->sleep);
    dev->cmd_buf[p] = 0;

    if (dev->int_state == RN2XX3_INT_STATE_SLEEP) {
        DEBUG("[rn2xx3] sleep: device already in sleep mode\n");
        return RN2XX3_ERR_SLEEP_MODE;
    }

    /* always succeeds */
    rn2xx3_write_cmd_no_wait(dev);

    /* Wait a little to check if the device could go to sleep. No answer means
       it worked. */
    xtimer_msleep(1);

    DEBUG("[rn2xx3] RESP: %s\n", dev->resp_buf);
    if (rn2xx3_process_response(dev) == RN2XX3_ERR_INVALID_PARAM) {
        DEBUG("[rn2xx3] sleep: cannot put module in sleep mode\n");
        return RN2XX3_ERR_INVALID_PARAM;
    }

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_SLEEP);
    xtimer_set(&dev->sleep_timer, dev->sleep * US_PER_MS);

    return RN2XX3_OK;
}

int rn2xx3_mac_init(rn2xx3_t *dev)
{
    rn2xx3_mac_set_dr(dev, CONFIG_LORAMAC_DEFAULT_DR);
    rn2xx3_mac_set_tx_power(dev, CONFIG_LORAMAC_DEFAULT_TX_POWER);
    rn2xx3_mac_set_tx_port(dev, CONFIG_LORAMAC_DEFAULT_TX_PORT);
    rn2xx3_mac_set_tx_mode(dev, CONFIG_LORAMAC_DEFAULT_TX_MODE);
    rn2xx3_mac_set_adr(dev, LORAMAC_DEFAULT_ADR);
    rn2xx3_mac_set_retx(dev, LORAMAC_DEFAULT_RETX);
    rn2xx3_mac_set_linkchk_interval(dev, LORAMAC_DEFAULT_LINKCHK);
    rn2xx3_mac_set_rx1_delay(dev, LORAMAC_DEFAULT_RX1_DELAY);
    rn2xx3_mac_set_ar(dev, IS_ACTIVE(CONFIG_RN2XX3_DEFAULT_AR));
    rn2xx3_mac_set_rx2_dr(dev, LORAMAC_DEFAULT_RX2_DR);
    rn2xx3_mac_set_rx2_freq(dev, LORAMAC_DEFAULT_RX2_FREQ);

    return RN2XX3_OK;
}

int rn2xx3_mac_tx(rn2xx3_t *dev, uint8_t *payload, uint8_t payload_len)
{
    if (dev->int_state == RN2XX3_INT_STATE_SLEEP) {
        DEBUG("[rn2xx3] ABORT: device is in sleep mode\n");
        return RN2XX3_ERR_SLEEP_MODE;
    }

    rn2xx3_mac_tx_start(dev);
    for (unsigned i = 0; i < payload_len; ++i) {
        rn2xx3_cmd_append(dev, &payload[i], 1);
    }

    int ret = rn2xx3_mac_tx_finalize(dev);
    if (ret != RN2XX3_OK) {
        rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);
        DEBUG("[rn2xx3] TX command failed\n");
        return ret;
    }

    ret = rn2xx3_wait_reply(dev, RN2XX3_REPLY_DELAY_TIMEOUT);
    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);

    return ret;
}

int rn2xx3_mac_join_network(rn2xx3_t *dev, loramac_join_mode_t mode)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1,
                        "mac join %s",
                        (mode == LORAMAC_JOIN_OTAA) ? "otaa" : "abp");
    dev->cmd_buf[p] = 0;

    int ret = rn2xx3_write_cmd(dev);
    if (ret != RN2XX3_OK) {
        DEBUG("[rn2xx3] join procedure command failed\n");
        return ret;
    }

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_MAC_JOIN);

    ret = rn2xx3_wait_reply(dev,
                            LORAMAC_DEFAULT_JOIN_DELAY1 + \
                            LORAMAC_DEFAULT_JOIN_DELAY2);

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);

    return ret;
}

int rn2xx3_mac_save(rn2xx3_t *dev)
{
    size_t p = snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "mac save");
    dev->cmd_buf[p] = 0;
    return rn2xx3_write_cmd(dev);
}
