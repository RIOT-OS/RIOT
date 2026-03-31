/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_rn2xx3
 * @{
 *
 * @file
 * @brief       Internal driver implementation for the RN2483/RN2903 devices
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "fmt.h"
#include "timex.h"
#include "ztimer.h"

#include "rn2xx3_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define RESP_TIMEOUT_SEC            (5U)

static const char *closing_seq = "\r\n";

static void _uart_write_str(rn2xx3_t *dev, const char *str)
{
    size_t len = strlen(str);
    if (len) {
        uart_write(dev->p.uart, (uint8_t *)str, len);
    }
}

static void isr_resp_timeout(void *arg)
{
    rn2xx3_t *dev = (rn2xx3_t *)arg;
    mutex_unlock(&(dev->resp_lock));
}

static bool _wait_reply(rn2xx3_t *dev, uint8_t timeout)
{
    dev->resp_done = 0;
    dev->resp_size = 0;
    dev->resp_buf[0] = 0;

    ztimer_now_t sent_time = ztimer_now(ZTIMER_MSEC);

    ztimer_t resp_timer;
    resp_timer.callback = isr_resp_timeout;
    resp_timer.arg = dev;

    ztimer_set(ZTIMER_MSEC, &resp_timer, (uint32_t)timeout * MS_PER_SEC);

    /* wait for results */
    while ((!dev->resp_done) &&
        ((int32_t)(sent_time + (timeout * MS_PER_SEC) - ztimer_now(ZTIMER_MSEC)) > 0)) {
        mutex_lock(&(dev->resp_lock));
    }

    ztimer_remove(ZTIMER_MSEC, &resp_timer);

    if (dev->resp_done == 0) {
        DEBUG("[rn2xx3] response timeout\n");
        return true;
    }

    return false;
}

void rn2xx3_hex_to_bytes(const char *hex, uint8_t *byte_array)
{
    const uint8_t charmap[] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, /* 01234567 */
        0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 89:;<=>? */
        0x00, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x00, /* @ABCDEFG */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* HIJKLMNO */
    };

    size_t len = strlen(hex);
    for (uint8_t pos = 0; pos < len; pos += 2) {
        uint8_t idx0 = ((uint8_t)hex[pos + 0] & 0x1F) ^ 0x10;
        uint8_t idx1 = ((uint8_t)hex[pos + 1] & 0x1F) ^ 0x10;
        byte_array[pos / 2] = (uint8_t)(charmap[idx0] << 4) | charmap[idx1];
    };
}

void rn2xx3_set_internal_state(rn2xx3_t *dev, uint8_t state)
{
    if ((dev->int_state == RN2XX3_INT_STATE_SLEEP) ||
        (dev->int_state == state)) {
        return;
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        printf("[rn2xx3] new state: ");
        switch(state) {
            case RN2XX3_INT_STATE_CMD:
                puts("CMD");
                break;

            case RN2XX3_INT_STATE_IDLE:
                puts("IDLE");
                break;

            case RN2XX3_INT_STATE_MAC_JOIN:
                puts("JOIN");
                break;

            case RN2XX3_INT_STATE_MAC_RX_MESSAGE:
                puts("RX MSG");
                break;

            case RN2XX3_INT_STATE_MAC_RX_PORT:
                puts("RX PORT");
                break;

            case RN2XX3_INT_STATE_MAC_TX:
                puts("TX");
                break;

            case RN2XX3_INT_STATE_RESET:
                puts("RESET");
                break;

            case RN2XX3_INT_STATE_SLEEP:
                puts("SLEEP");
                break;

            default:
                puts("UNKNOWN");
                break;
        }
    }

    dev->int_state = state;
}

int rn2xx3_write_cmd(rn2xx3_t *dev)
{
    int ret;
    DEBUG("[rn2xx3] CMD: %s\n", dev->cmd_buf);

    if (dev->int_state == RN2XX3_INT_STATE_SLEEP) {
        DEBUG("[rn2xx3] ABORT: device is in sleep mode\n");
        return RN2XX3_ERR_SLEEP_MODE;
    }

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_CMD);

    mutex_lock(&(dev->cmd_lock));
    _uart_write_str(dev, dev->cmd_buf);
    _uart_write_str(dev, closing_seq);

    ret = rn2xx3_wait_response(dev);
    if (ret == RN2XX3_TIMEOUT) {
        mutex_unlock(&(dev->cmd_lock));
        return RN2XX3_TIMEOUT;
    }

    mutex_unlock(&(dev->cmd_lock));

    ret = rn2xx3_process_response(dev);
    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);

    DEBUG("[rn2xx3] RET: %d, RESP: %s\n", ret, dev->resp_buf);

    return ret;
}

int rn2xx3_write_cmd_no_wait(rn2xx3_t *dev)
{
    DEBUG("[rn2xx3] CMD (NO WAIT): %s\n", dev->cmd_buf);

    mutex_lock(&(dev->cmd_lock));
    _uart_write_str(dev, dev->cmd_buf);
    _uart_write_str(dev, closing_seq);
    mutex_unlock(&(dev->cmd_lock));

    return RN2XX3_OK;
}

int rn2xx3_wait_response(rn2xx3_t *dev)
{
    if (_wait_reply(dev, RESP_TIMEOUT_SEC)) {
        return RN2XX3_TIMEOUT;
    }

    DEBUG("[rn2xx3] RESP: %s\n", dev->resp_buf);

    return RN2XX3_OK;
}

int rn2xx3_wait_reply(rn2xx3_t *dev, uint8_t timeout)
{
    if (_wait_reply(dev, timeout)) {
        return RN2XX3_REPLY_TIMEOUT;
    }

    DEBUG("[rn2xx3] REPLY: %s\n", dev->resp_buf);

    return rn2xx3_process_reply(dev);
}

void rn2xx3_cmd_start(rn2xx3_t *dev)
{
    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_CMD);
    DEBUG("[rn2xx3] CMD: %s", dev->cmd_buf);
    mutex_lock(&(dev->cmd_lock));
    _uart_write_str(dev, dev->cmd_buf);
}

void rn2xx3_cmd_append(rn2xx3_t *dev, const uint8_t *payload, uint8_t payload_len)
{
    char payload_str[3] = { 0 };
    for (unsigned i = 0; i < payload_len; i++) {
        fmt_byte_hex(payload_str, payload[i]);
        DEBUG("%s", payload_str);
        _uart_write_str(dev, payload_str);
    }
}

int rn2xx3_cmd_finalize(rn2xx3_t *dev)
{
    DEBUG("\n");
    _uart_write_str(dev, closing_seq);
    uint8_t ret = rn2xx3_wait_response(dev);

    mutex_unlock(&(dev->cmd_lock));

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_IDLE);

    DEBUG("[rn2xx3] RET: %d, RESP: %s\n", ret, dev->resp_buf);

    return ret;
}

void rn2xx3_mac_tx_start(rn2xx3_t *dev)
{
    snprintf(dev->cmd_buf, sizeof(dev->cmd_buf) - 1, "mac tx %s %d ",
             (dev->loramac.tx_mode == LORAMAC_TX_CNF) ? "cnf" : "uncnf",
             dev->loramac.tx_port);

    rn2xx3_cmd_start(dev);
}

int rn2xx3_mac_tx_finalize(rn2xx3_t *dev)
{
    rn2xx3_cmd_finalize(dev);

    rn2xx3_set_internal_state(dev, RN2XX3_INT_STATE_MAC_TX);

    return rn2xx3_process_response(dev);
}

int rn2xx3_process_response(rn2xx3_t *dev)
{
    uint8_t ret = RN2XX3_DATA;
    if (strcmp(dev->resp_buf, "ok") == 0) {
        DEBUG("[rn2xx3] command succeeded: '%s'\n", dev->cmd_buf);
        ret = RN2XX3_OK;
    }
    else if (strcmp(dev->resp_buf, "invalid_param") == 0) {
        DEBUG("[rn2xx3] invalid command: '%s'\n", dev->cmd_buf);
        ret = RN2XX3_ERR_INVALID_PARAM;
    }
    else if (strcmp(dev->resp_buf, "not_joined") == 0) {
        DEBUG("[rn2xx3] failed: network is not joined\n");
        ret = RN2XX3_ERR_NOT_JOINED;
    }
    else if (strcmp(dev->resp_buf, "no_free_ch") == 0) {
        DEBUG("[rn2xx3] failed: all channels are busy\n");
        ret = RN2XX3_ERR_NO_FREE_CH;
    }
    else if (strcmp(dev->resp_buf, "silent") == 0) {
        DEBUG("[rn2xx3] failed: all channels are busy\n");
        ret = RN2XX3_ERR_SILENT;
    }
    else if (strcmp(dev->resp_buf, "frame_counter_err_rejoin_needed") == 0) {
        DEBUG("[rn2xx3] failed: Frame counter rolled over\n");
        ret = RN2XX3_ERR_FR_CNT_REJOIN_NEEDED;
    }
    else if (strcmp(dev->resp_buf, "busy") == 0) {
        DEBUG("[rn2xx3] failed: MAC state is in an Idle state\n");
        ret = RN2XX3_ERR_BUSY;
    }
    else if (strcmp(dev->resp_buf, "invalid_data_len") == 0) {
        DEBUG("[rn2xx3] failed: payload length too large\n");
        ret = RN2XX3_ERR_INVALID_DATA_LENGTH;
    }

    return ret;
}

int rn2xx3_process_reply(rn2xx3_t *dev)
{
    uint8_t ret;
    if (strcmp(dev->resp_buf, "accepted") == 0) {
        DEBUG("[rn2xx3] join procedure succeeded.\n");
        ret = RN2XX3_REPLY_JOIN_ACCEPTED;
    }
    else if (strcmp(dev->resp_buf, "denied") == 0) {
        DEBUG("[rn2xx3] join procedure failed.\n");
        ret = RN2XX3_REPLY_JOIN_DENIED;
    }
    else if (strcmp(dev->resp_buf, "mac_tx_ok") == 0) {
        DEBUG("[rn2xx3] uplink transmission succeeded.\n");
        ret = RN2XX3_REPLY_TX_MAC_OK;
    }
    else if (strncmp(dev->resp_buf, "mac_rx", 6) == 0) {
        DEBUG("[rn2xx3] received downlink data from server.\n");
        ret = RN2XX3_REPLY_TX_MAC_RX;
    }
    else if (strcmp(dev->resp_buf, "mac_err") == 0) {
        DEBUG("[rn2xx3] uplink transmission failed.\n");
        ret = RN2XX3_REPLY_TX_MAC_ERR;
    }
    else if (strcmp(dev->resp_buf, "invalid_data_len") == 0) {
        DEBUG("[rn2xx3] payload length too large.\n");
        ret = RN2XX3_REPLY_TX_MAC_ERR;
    }
    else {
        DEBUG("[rn2xx3] unknown reply.\n");
        ret = RN2XX3_REPLY_OTHER;
    }

    return ret;
}
