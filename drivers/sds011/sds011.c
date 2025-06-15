/*
 * SPDX-FileCopyrightText: 2018 HAW-Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sds011
 * @{
 *
 * @file
 * @brief       SDS011 Laser Dust Sensor driver implementation
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "assert.h"
#include "sds011.h"
#include "periph/uart.h"

/**
 * @brief             UART receive callback
 *
 * @param[in]   arg   Context value previously handed to the uart_init call
 * @param[in]   data  single byte received over UART
 */
static void _rx_cb(void *arg, uint8_t data)
{
    sds011_t *dev = (sds011_t*)arg;

    /* frame MUST start with HEAD byte and the buffer must be cleared
       before writing to it again */
    if (((dev->pos == 0) && (data != SDS011_FRAME_HEAD)) ||
         (dev->pos == SDS011_FRAME_RECV_LEN)) {
        return;
    }

    dev->rx_mem[dev->pos] = data;

    if ((dev->pos >= SDS011_DB1_IDX) &&
        (dev->pos < SDS011_FRAME_RECV_CSUM_IDX)) {
        dev->checksum += data;
    }
    else if ((dev->pos == SDS011_FRAME_RECV_LEN - 1) &&
             (dev->rx_mem[SDS011_FRAME_HEAD_IDX] == SDS011_FRAME_HEAD) &&
             (dev->rx_mem[SDS011_FRAME_RECV_TAIL_IDX] == SDS011_FRAME_TAIL)) {

        dev->checksum &= SDS011_FRAME_CSUM_MSK;
        if (dev->rx_mem[SDS011_FRAME_RECV_CSUM_IDX] == dev->checksum) {

            if ((dev->cb != NULL) &&
                (dev->rx_mem[SDS011_CMDID_IDX] == SDS011_RCMDID_DATA)) {
                    sds011_data_t measure;
                    measure.pm_2_5 = dev->rx_mem[SDS011_DB1_IDX] |
                                    (dev->rx_mem[SDS011_DB2_IDX] << 8);
                    measure.pm_10  = dev->rx_mem[SDS011_DB3_IDX] |
                                    (dev->rx_mem[SDS011_DB4_IDX] << 8);
                    dev->cb(&measure, dev->cbctx);
                    dev->pos = -1;
            }
        }
        else {
            dev->pos = -1;
        }

        dev->checksum = 0;

        /* unlock the mutex for the calling function */
        mutex_unlock(&dev->cb_lock);
    }

    dev->pos++;
}

/**
 * @brief             send command and wait for first replied message
 *
 * @param[in]   dev         SDS011 device the command is sent to
 * @param[in]   data_bytes  data bytes to send within the command
 * @param[in]   len         number of data bytes
 * @param[out]  recv_frm    pointer where the received frame will be stored
 *                          must at least provide SDS011_FRAME_RECV_LEN bytes
 */
int _send_recv_cmd(sds011_t *dev, uint8_t *data_bytes, size_t len, uint8_t *recv_frm)
{
    uint8_t cmd[SDS011_FRAME_SEND_LEN] = {0};
    int checksum = 0;
    int res = SDS011_ERROR;

    cmd[SDS011_FRAME_HEAD_IDX]  = SDS011_FRAME_HEAD;
    cmd[SDS011_CMDID_IDX] = SDS011_CMDID_QUERY;

    for (unsigned i = 0; i < len; i++) {
        cmd[SDS011_DB1_IDX + i] = data_bytes[i];
        checksum += data_bytes[i];
    }

    cmd[SDS011_DEVID1_IDX] = (dev->params.dev_id >> 8) & 0xFF;
    checksum += cmd[SDS011_DEVID1_IDX];
    cmd[SDS011_DEVID2_IDX] = dev->params.dev_id & 0xFF;
    checksum += cmd[SDS011_DEVID2_IDX];

    cmd[SDS011_FRAME_SEND_LEN - 2] = checksum & SDS011_FRAME_CSUM_MSK;
    cmd[SDS011_FRAME_SEND_TAIL_IDX] = SDS011_FRAME_TAIL;

    mutex_lock(&dev->dev_lock);

    dev->pos = 0;
    dev->checksum = 0;

    mutex_lock(&dev->cb_lock);

    /* if no active reporting callback is registered, UART must be enabled first */
    if((dev->cb == NULL) &&
       (uart_init(dev->params.uart, SDS011_UART_BAUDRATE, _rx_cb, dev) != 0)) {
        mutex_unlock(&dev->cb_lock);
        mutex_unlock(&dev->dev_lock);
        return SDS011_ERROR;
    }

    uart_write(dev->params.uart, cmd, SDS011_FRAME_SEND_LEN);

    /* wait for the isr callback to unlock the mutex */
    mutex_lock(&dev->cb_lock);

    /* only copy data when checksum was valid */
    if (dev->pos != 0) {
        memcpy(recv_frm, dev->rx_mem, SDS011_FRAME_RECV_LEN);
        /* mark the recv buffer as free */
        dev->pos = 0;
        dev->checksum = 0;

        /* check if we received a valid response for the cmd sent*/
        if(((recv_frm[SDS011_CMDID_IDX]  == SDS011_RCMDID_REPLY) &&
            (cmd[SDS011_DB1_IDX] == recv_frm[SDS011_DB1_IDX]))
           || ((recv_frm[SDS011_CMDID_IDX] == SDS011_RCMDID_DATA)
            && (cmd[SDS011_DB1_IDX] == SDS011_CMD_DB1_QUERY_DATA))) {
            res = SDS011_OK;
        }
        else {
            res = SDS011_INVALID_RESPONSE;
        }
    }
    else {
        res = SDS011_INVALID_CHKSUM;
    }

    /* reset mutex state */
    mutex_unlock(&dev->cb_lock);

    /* if no active reporting callback is registered, UART can be disabled */
    if((dev->cb == NULL) &&
       (uart_init(dev->params.uart, SDS011_UART_BAUDRATE, NULL, NULL) != 0)) {
        res = SDS011_ERROR;
    }

    /* release device */
    mutex_unlock(&dev->dev_lock);

    return res;
}

/**
 * @brief       shorthand to get a single byte property with _send_recv_cmd
 *
 * @param[in]   dev         SDS011 device the command is sent to
 * @param[in]   data_bytes  data bytes to send within the command
 * @param[in]   len         number of data bytes
 * @param[out]  p           pointer for storing single data byte of the reply
 * @param[out]  p_idx       index of data byte we want to read
 */
static int _get_property(sds011_t *dev, uint8_t *data_bytes, size_t len,
                         uint8_t *p, uint8_t p_idx)
{
    uint8_t recv[SDS011_FRAME_RECV_LEN];
    int res = _send_recv_cmd(dev, data_bytes, len, recv);

    if (res == SDS011_OK) {
        *p = recv[p_idx];
    }

    return res;
}

int sds011_init(sds011_t *dev, const sds011_params_t *params)
{
    assert((dev != NULL) && (params != NULL) && (params->uart < UART_NUMOF));

    if ((gpio_is_valid(params->pwr_pin)) &&
        (gpio_init(params->pwr_pin, GPIO_OUT) != 0)) {
        return SDS011_ERROR;
    }

    memcpy(&dev->params, params, sizeof(sds011_params_t));

    mutex_init(&dev->dev_lock);
    mutex_init(&dev->cb_lock);

    dev->cb = NULL;

    sds011_power_on(dev);

    return SDS011_OK;
}

int sds011_register_callback(sds011_t *dev, sds011_callback_t cb, void *ctx)
{
    assert(dev != NULL);
    mutex_lock(&dev->dev_lock);
    dev->cbctx = ctx;
    dev->cb = cb;

    /* either register un unregister the uart callback */
    if (uart_init(dev->params.uart, SDS011_UART_BAUDRATE,
                  cb == NULL ? NULL : _rx_cb,
                  cb == NULL ? NULL : dev) != 0) {
        mutex_unlock(&dev->dev_lock);
        return SDS011_ERROR;
    }
    mutex_unlock(&dev->dev_lock);
    return SDS011_OK;
}

void sds011_power_on(const sds011_t *dev)
{
    assert(dev != NULL);
    if(gpio_is_valid(dev->params.pwr_pin)) {
        gpio_write(dev->params.pwr_pin, dev->params.pwr_ah);
    }
}

void sds011_power_off(const sds011_t *dev)
{
    assert(dev != NULL);
    if(gpio_is_valid(dev->params.pwr_pin)) {
        gpio_write(dev->params.pwr_pin, !dev->params.pwr_ah);
    }
}

int sds011_get_reporting_mode(sds011_t *dev, sds011_reporting_mode_t *mode)
{
    assert(dev != NULL);
    uint8_t cmd[] = {SDS011_CMD_DB1_SET_DR_MODE, SDS011_CMD_OPT_QUERY};
    uint8_t prop = 0;
    int res = _get_property(dev, cmd, sizeof(cmd), &prop, SDS011_DB3_IDX);
    *mode = ((prop == 0) ? SDS011_RMODE_ACTIVE : SDS011_RMODE_QUERY);
    return res;
}

int sds011_set_reporting_mode(sds011_t *dev, sds011_reporting_mode_t mode)
{
    assert(dev != NULL);
    uint8_t cmd[] = {SDS011_CMD_DB1_SET_DR_MODE, SDS011_CMD_OPT_SET, mode};

    uint8_t recv[SDS011_FRAME_RECV_LEN];
    int res = _send_recv_cmd(dev, cmd, sizeof(cmd), recv);

    if (res == SDS011_OK) {
        if ((recv[SDS011_DB2_IDX] == SDS011_CMD_OPT_SET) &&
            (recv[SDS011_DB3_IDX] == mode)) {
            return SDS011_OK;
        }

        return SDS011_ERROR;
    }

    return res;
}

int sds011_read(sds011_t *dev, sds011_data_t *data)
{
    assert((dev != NULL) && (data != NULL));
    uint8_t cmd[] = {SDS011_CMD_DB1_QUERY_DATA};

    uint8_t recv[SDS011_FRAME_RECV_LEN];
    int res = _send_recv_cmd(dev, cmd, sizeof(cmd), recv);

    if (res == SDS011_OK) {
        data->pm_2_5 = recv[SDS011_DB1_IDX] | (recv[SDS011_DB2_IDX] << 8);
        data->pm_10  = recv[SDS011_DB3_IDX] | (recv[SDS011_DB4_IDX] << 8);
    }

    return res;
}

int sds011_set_dev_id(sds011_t *dev, uint16_t sens_dev_id)
{
    assert(dev != NULL);
    uint8_t cmd[13] = {0};
    cmd[0]  = SDS011_CMD_DB1_SET_DEV_ID;
    cmd[11] = (sens_dev_id >> 8) & 0xFF;
    cmd[12] = sens_dev_id & 0xFF;

    uint8_t recv[SDS011_FRAME_RECV_LEN];
    int res = _send_recv_cmd(dev, cmd, sizeof(cmd), recv);

    if ((res == SDS011_OK) &&
        (recv[SDS011_DB5_IDX] == cmd[11]) &&
        (recv[SDS011_DB6_IDX] == cmd[12])) {
        return SDS011_OK;
    }

    return SDS011_ERROR;
}

int sds011_get_working_mode(sds011_t *dev, sds011_working_mode_t *mode)
{
    assert(dev != NULL);
    uint8_t cmd[] = {SDS011_CMD_DB1_SET_SLEEP_WORK, SDS011_CMD_OPT_QUERY};
    uint8_t prop = 0;
    int res = _get_property(dev, cmd, sizeof(cmd), &prop, SDS011_DB3_IDX);
    *mode = ((prop == 0) ? SDS011_WMODE_SLEEP : SDS011_WMODE_WORK);
    return res;
}

int sds011_set_working_mode(sds011_t *dev, sds011_working_mode_t mode)
{
    assert(dev != NULL);
    uint8_t cmd[] = {SDS011_CMD_DB1_SET_SLEEP_WORK, SDS011_CMD_OPT_SET, mode};

    uint8_t recv[SDS011_FRAME_RECV_LEN];
    int res = _send_recv_cmd(dev, cmd, sizeof(cmd), recv);

    if (res == SDS011_OK) {
        if ((recv[SDS011_DB2_IDX] == SDS011_CMD_OPT_SET) &&
            (recv[SDS011_DB3_IDX] == mode)) {
            return SDS011_OK;
        }

        return SDS011_ERROR;
    }

    return res;
}

int sds011_get_working_period(sds011_t *dev, uint8_t *minutes)
{
    assert(dev != NULL);
    uint8_t cmd[] = {SDS011_CMD_DB1_SET_WORK_PERIOD, SDS011_CMD_OPT_QUERY};
    return _get_property(dev, cmd, sizeof(cmd), minutes, SDS011_DB3_IDX);
}

int sds011_set_working_period(sds011_t *dev, uint8_t minutes)
{
    assert(dev != NULL);
    uint8_t cmd[] = {SDS011_CMD_DB1_SET_WORK_PERIOD, SDS011_CMD_OPT_SET, minutes};

    uint8_t recv[SDS011_FRAME_RECV_LEN];
    int res = _send_recv_cmd(dev, cmd, sizeof(cmd), recv);

    if (res == SDS011_OK) {
        if ((recv[SDS011_DB2_IDX] == SDS011_CMD_OPT_SET) &&
            (recv[SDS011_DB3_IDX] == minutes)) {
            return SDS011_OK;
        }

        return SDS011_ERROR;
    }

    return res;
}

int sds011_get_fw_version(sds011_t *dev, uint8_t *year, uint8_t *mon, uint8_t *day)
{
    assert((dev != NULL) && (year != NULL) && (mon != NULL) && (day != NULL));
    uint8_t cmd[] = {SDS011_CMD_DB1_CHECK_FIRMWARE};

    uint8_t recv[SDS011_FRAME_RECV_LEN];
    int res = _send_recv_cmd(dev, cmd, sizeof(cmd), recv);

    if (res == SDS011_OK) {
        *year  = recv[SDS011_DB2_IDX];
        *mon = recv[SDS011_DB3_IDX];
        *day = recv[SDS011_DB4_IDX];
    }

    return res;
}
