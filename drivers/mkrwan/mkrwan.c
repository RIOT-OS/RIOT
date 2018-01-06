/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mkrwan
 * @{
 *
 * @file
 * @brief       Device driver implementation for the  LoRa radio.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "at.h"
#include "fmt.h"

#include "net/loramac.h"

#include "periph/gpio.h"

#include "mkrwan.h"
#include "mkrwan_params.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

static char tmp_buf[MKRWAN_TMP_BUF_SIZE];

void mkrwan_setup(mkrwan_t *dev, const mkrwan_params_t *params)
{
    assert(dev && (params->uart < UART_NUMOF));
    dev->params = *params;
}

int mkrwan_init(mkrwan_t *dev)
{
    DEBUG("[mkrwan] Initializing UART %d at baudrate: %u\n",
          dev->params.uart, (unsigned int)dev->params.baudrate);

    DEBUG("[mkrwan] hardware reset of the module\n");
    gpio_init(dev->params.boot0_pin, GPIO_OUT);
    gpio_clear(dev->params.boot0_pin);

    gpio_init(dev->params.reset_pin, GPIO_OUT);
    gpio_set(dev->params.reset_pin);
    xtimer_usleep(200 * US_PER_MS);
    gpio_clear(dev->params.reset_pin);
    xtimer_usleep(200 * US_PER_MS);
    gpio_set(dev->params.reset_pin);

    DEBUG("[mkrwan] initializing bus\n");

    at_dev_init(&dev->at_dev,
                dev->params.uart, dev->params.baudrate,
                dev->buf, sizeof(dev->buf));

    xtimer_usleep(200 * US_PER_MS);

    if (at_send_cmd_wait_ok(&dev->at_dev, "AT", MKRWAN_TIMEOUT) < 0) {
        DEBUG("[mkrwan] AT device not initialized correctly\n");
        return -MKRWAN_ERR;
    }

    /* set default parameters */
    mkrwan_set_tx_port(dev, LORAMAC_DEFAULT_TX_PORT);

    DEBUG("[mkrwan] AT device initialized with success\n");

    return MKRWAN_OK;
}

int mkrwan_reset(mkrwan_t *dev)
{
    DEBUG("[mkrwan] resetting device\n");

    int res = at_send_cmd(&dev->at_dev, "AT+REBOOT", MKRWAN_TIMEOUT);
    if (res < 0) {
        return -MKRWAN_ERR;
    }

    return MKRWAN_OK;
}

void mkrwan_version(mkrwan_t *dev, char *version)
{
    at_send_cmd(&dev->at_dev, "AT+DEV?", MKRWAN_TIMEOUT);
    at_readline(&dev->at_dev, tmp_buf, sizeof(tmp_buf), false, MKRWAN_TIMEOUT);
    DEBUG("[mkrwan] version: %s-", &tmp_buf[4]);
    size_t len_dev = strlen(&tmp_buf[4]);
    memcpy(version, &tmp_buf[4], len_dev);
    char *pos = &version[len_dev];
    pos += fmt_str(pos, "-");
    at_send_cmd(&dev->at_dev, "AT+VER?", MKRWAN_TIMEOUT);
    at_readline(&dev->at_dev, tmp_buf, sizeof(tmp_buf), false, MKRWAN_TIMEOUT);
    DEBUG("%s\n", &tmp_buf[4]);
    size_t len_ver = strlen(&tmp_buf[4]);
    memcpy(pos, &tmp_buf[4], len_ver);
    pos += len_ver;
    *pos = '\0';
}

int mkrwan_join(mkrwan_t *dev, uint8_t mode)
{
    DEBUG("[mkrwan] join to network using ");
    switch (mode) {
        case LORAMAC_JOIN_OTAA:
            DEBUG("OTAA\n");
            break;
        case LORAMAC_JOIN_ABP:
            DEBUG("ABP\n");
            break;
        default:
            break;
    }

    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+MODE=");
    pos += fmt_u16_dec(pos, 1 - mode);
    *pos = '\0';

    DEBUG("[mkrwan] sending join command %s\n", tmp_buf);

    int res = at_send_cmd_wait_ok(&dev->at_dev, tmp_buf, MKRWAN_TIMEOUT);

    if (res < 0) {
        DEBUG("[mkrwan] failed setting join mode\n");
        return -MKRWAN_ERR;
    }

    res = at_send_cmd(&dev->at_dev, "AT+JOIN", MKRWAN_JOIN_TIMEOUT);
    at_readline(&dev->at_dev, tmp_buf, sizeof(tmp_buf),
                false, MKRWAN_JOIN_TIMEOUT);
    DEBUG("[mkrwan] join response: %s\n", tmp_buf);
    if (res < 0) {
        DEBUG("[mkrwan] join error\n");
        return -MKRWAN_ERR;
    }

    pos = tmp_buf;
    pos += fmt_str(pos, "AT+NJS?");
    *pos = '\0';
    res = at_send_cmd_get_resp(&dev->at_dev, tmp_buf,
                               tmp_buf, sizeof(tmp_buf),
                               MKRWAN_TIMEOUT);
    if (res < 0) {
        DEBUG("[mkrwan] failed to get network status\n");
        return -MKRWAN_ERR;
    }

    return MKRWAN_OK;
}

int mkrwan_send(mkrwan_t *dev, uint8_t *data, uint8_t len, uint8_t mode)
{
    DEBUG("[mkrwan] sending data\n");

    int res;
    DEBUG("[mkrwan] check if network is joined\n");

    char *pos = tmp_buf;
    pos += fmt_str(pos, "AT+NJS?");
    *pos = '\0';
    res = at_send_cmd_get_resp(&dev->at_dev, tmp_buf,
                               tmp_buf, sizeof(tmp_buf),
                               MKRWAN_TIMEOUT);
    if (res < 0) {
        DEBUG("[mkrwan] failed to get network status\n");
        return -MKRWAN_ERR;
    }

    if (atoi(&tmp_buf[4]) != 1) {
        DEBUG("[mkrwan] network is not joined!\n");
        return -MKRWAN_ERR;
    }

    pos = tmp_buf;
    if (mode == LORAMAC_TX_CNF) {
        pos += fmt_str(pos, "AT+CTX ");
    }
    else {
        pos += fmt_str(pos, "AT+UTX ");
    }

    pos += fmt_u16_dec(pos, len);
    *pos = '\0';

    DEBUG("[mkrwan] sending with command %s\n", tmp_buf);

    res = at_send_cmd(&dev->at_dev, tmp_buf, MKRWAN_TIMEOUT);
    if (res < 0) {
        DEBUG("[mkrwan] send preparation failed!\n");
        return -MKRWAN_ERR;
    }

    at_send_bytes(&dev->at_dev, (char *)data, len);

    res = at_send_cmd_wait_ok(&dev->at_dev, "AT", MKRWAN_TIMEOUT);
    if (res < 0) {
        DEBUG("[mkrwan] failed to send data!\n");
        return -MKRWAN_ERR;
    }

    return MKRWAN_OK;
}
