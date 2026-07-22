/*
 * Copyright (C) 2022 Samuel Kayode
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_l6470
 * @{
 *
 * @file
 * @brief       Device driver implementation for the L6470 driver
 *
 * @author      Samuel Kayode <samkay014@gmail.com>
 *
 * @}
 */

#include "l6470.h"

#include "l6470_constants.h"
#include "l6470_params.h"

#define ENABLE_DEBUG 1
#include <assert.h>
#include <errno.h>
#include <string.h>

#include "debug.h"
#include "ztimer.h"

#define ASSERT_PARAM(cond)                                  \
    if (!(cond)) {                                            \
        DEBUG("[l6470] %s: %s\n", __func__,                     \
              "parameter condition (" #cond ") not fulfilled"); \
        assert(cond);                                           \
    }

#define DEBUG_DEV(m, d, ...)                                               \
    DEBUG("[l6470] %s dev=%" PRIxPTR ": " m "\n", __func__, (unsigned int)d, \
          ## __VA_ARGS__)

/* SPI configuration*/
#define _SPI_DEV (dev->params.dev)
#define _SPI_CS (dev->params.cs)
#define _SPI_MODE (dev->params.spi_mode)
#define _SPI_CLK (dev->params.clk)

/* fill rx buffer with data received */
static void _chk_for_data(const l6470_t *dev, uint8_t *rx);

int l6470_init(l6470_t *dev, const l6470_params_t *params)
{
    /* Initializes all devices with same configuration */
    ASSERT_PARAM(dev != NULL);
    ASSERT_PARAM(params != NULL);

    int res = L6470_OK;
    int daisy;                    /* daisy SPI devices, 1 if standard SPI */

    int rx_size;
    int tx_size;

    rx_size = sizeof(uint8_t) * L6470_MAX_BYTES_RX * L6470_MAX_DAISY_CHAIN_DEVS;
    tx_size = sizeof(uint8_t) * L6470_MAX_BYTES_TX * L6470_MAX_DAISY_CHAIN_DEVS;

    /* Initialize device data structure */
    dev->params = *params;

#if IS_USED(MODULE_L6470_STBY)
    gpio_init(dev->params.stby, GPIO_OUT);
#endif /* IS_USED(MODULE_L6470_STBY) */

    if (spi_init_cs(_SPI_DEV, _SPI_CS) != SPI_OK) {
        return -L6470_ERROR_SPI;
    }

    /* Initialize private buffers */
    memset(dev->priv.recv, 0, sizeof(uint8_t) * rx_size);

    memset(dev->priv.xmit, 0, sizeof(uint8_t) * tx_size);

    /* waits 3ms on power-up before sending commands */
    ztimer_sleep(ZTIMER_MSEC, 3);

    /* reset L6470 devices */
    for (daisy = 0; daisy < dev->params.spi_daisy_chain_devs; daisy++) {
        res = l6470_mgmt_reset_dev(dev, daisy);

        if (res != L6470_OK) {
            return res;
        }
    }
    res = l6470_send_command(dev, NULL);

    /* waits 3ms after reset before sending commands */
    ztimer_sleep(ZTIMER_MSEC, 3);

    /* read config register */
    for (daisy = 0; daisy < dev->params.spi_daisy_chain_devs; daisy++) {
        res = l6470_mgmt_get_param(dev, L6470_REG_CONFIG, daisy);

        if (res != L6470_OK) {
            return res;
        }
    }

    res = l6470_send_command(dev, NULL);

    /* communication error if unable to read default value in config reg */
    for (daisy = 0; daisy < dev->params.spi_daisy_chain_devs; daisy++) {
        if (!(dev->priv.recv[0][daisy] == (L6470_CONFIG_DEFAULT >> 8) &&
              dev->priv.recv[1][daisy] == (L6470_CONFIG_DEFAULT & 0xff))) {
            return -L6470_ERROR_SPI;
        }
    }

    /* Set step mode */
    for (daisy = 0; daisy < dev->params.spi_daisy_chain_devs; daisy++) {
        res = l6470_mgmt_set_step_mode(dev, dev->params.step_mode, daisy);

        if (res != L6470_OK) {
            return res;
        }
    }

    res = l6470_send_command(dev, NULL);

    return res;
}

int l6470_mgmt_get_param(l6470_t *dev, uint8_t reg, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_GET_PARAM | reg;
    for (pkt_num = 1; pkt_num < (L6470_MAX_BYTES_TX - 1); pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_set_param(l6470_t *dev, uint8_t reg, uint32_t val, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_SET_PARAM | reg;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] =
            (val >> (8 * (L6470_MAX_BYTES_TX - pkt_num - 1))) & 0xff;
    }

    return L6470_OK;
}

int l6470_mgmt_reset_dev(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_RESET_DEV;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_reset_pos(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_RESET_POS;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_run(l6470_t *dev, l6470_speed_t fast, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_RUN | fast.direction;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] =
            (fast.speed >> (8 * (L6470_MAX_BYTES_TX - pkt_num - 1))) & 0xff;
        DEBUG_DEV("0x%02x", dev, data[pkt_num][dev_id]);
    }

    return L6470_OK;
}

int l6470_mgmt_move(l6470_t *dev, l6470_step_t rotate, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_MOVE | rotate.direction;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] =
            (rotate.step >> (8 * (L6470_MAX_BYTES_TX - pkt_num - 1))) & 0xff;
    }

    return L6470_OK;
}

int l6470_mgmt_go_to(l6470_t *dev, l6470_position_t position, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_GO_TO;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] =
            (position.target >>
             (8 * (L6470_MAX_BYTES_TX - pkt_num - 1))) & 0xff;
    }

    return L6470_OK;
}

int l6470_mgmt_go_to_dir(l6470_t *dev, l6470_position_t position, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    data[header][dev_id] = L6470_CMD_GO_TO_DIR | position.direction;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] =
            (position.target >>
             (8 * (L6470_MAX_BYTES_TX - pkt_num - 1))) & 0xff;
    }

    return L6470_OK;
}

int l6470_mgmt_go_until(l6470_t *dev, l6470_speed_t fast, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    data[header][dev_id] = L6470_CMD_GO_UNTIL | ((1 & fast.act) << 3)
                           | fast.direction;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] =
            (fast.speed >> (8 * (L6470_MAX_BYTES_TX - pkt_num - 1))) & 0xff;
    }

    return L6470_OK;
}

int l6470_mgmt_release_sw(l6470_t *dev, l6470_speed_t fast, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_RELEASE_SW | ((1 & fast.act) << 3) |
                           fast.direction;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_go_home(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_GO_HOME;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_go_mark(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_GO_MARK;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_soft_stop(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_SOFT_STOP;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_hard_stop(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_HARD_STOP;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_soft_hi_z(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_SOFT_HI_Z;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_hard_hi_z(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_HARD_HI_Z;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_get_status(l6470_t *dev, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    int header = 0;
    int pkt_num;

    uint8_t(*data)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    data[header][dev_id] = L6470_CMD_GET_STATUS;
    for (pkt_num = 1; pkt_num < L6470_MAX_BYTES_TX; pkt_num++) {
        data[pkt_num][dev_id] = 0;
    }

    return L6470_OK;
}

int l6470_mgmt_set_step_mode(l6470_t *dev, l6470_step_mode_t mode, int dev_id)
{
    ASSERT_PARAM(dev != NULL);

    if (dev_id < 0 || dev_id > L6470_MAX_DAISY_CHAIN_DEVS) {
        DEBUG_DEV("invalid ID, %d", dev, dev_id);
        return -L6470_ERROR_ID;
    }

    return l6470_mgmt_set_param(dev, L6470_REG_STEP_MODE, mode, dev_id);
}

#if IS_USED(MODULE_L6470_STBY)
int l6470_reset_dev(const l6470_t *dev) {
    ASSERT_PARAM(dev != NULL);

    gpio_clear(dev->params.stby);

    return L6470_OK;
}
#endif

int l6470_send_command(l6470_t *dev, uint8_t *rx)
{
    ASSERT_PARAM(dev != NULL);

    uint8_t daisy = dev->params.spi_daisy_chain_devs;

    uint8_t(*out)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;

    int pkt, dev_id;

    int header = 0;

    uint8_t(*in)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.recv;

    memset(in, 0, sizeof(in[0][0]));

    spi_acquire(_SPI_DEV, _SPI_CS, _SPI_MODE, _SPI_CLK);

    /* Send invoked commands to device */
    for (dev_id = 0; dev_id < (daisy - 1); dev_id++) {
        spi_transfer_bytes(_SPI_DEV, _SPI_CS, true, &out[header][dev_id],
                           NULL, 1);
    }

    /* L6470 device processes command only after CS pulled high */
    spi_transfer_bytes(_SPI_DEV, _SPI_CS, false, &out[header][dev_id], NULL, 1);

    /* Duplex communication */
    for (pkt = 1; pkt < L6470_MAX_BYTES_TX; pkt++) {
        spi_transfer_bytes(_SPI_DEV, _SPI_CS, false, &out[pkt][0],
                           &in[pkt - 1][0], daisy);
    }

    spi_release(_SPI_DEV);

    /* Receive device response */
    if (rx != NULL) {
        _chk_for_data(dev, rx);
    }

    memset(out, 0, sizeof(out[0][0]));

    return L6470_OK;
}

static void _chk_for_data(const l6470_t *dev, uint8_t *rx)
{
    int dev_id;
    int cmd = 0;     /* command byte */

    const uint8_t(*sent)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.xmit;
    const uint8_t(*recv)[L6470_MAX_DAISY_CHAIN_DEVS] = dev->priv.recv;
    uint8_t size = dev->params.spi_daisy_chain_devs;

    for (dev_id = 0; dev_id < size; dev_id++) {
        /* expect a response only for these command */
        if (sent[0][dev_id] != L6470_CMD_GET_PARAM &&
            sent[0][dev_id] != L6470_CMD_GET_STATUS) {
            continue;
        }

        cmd = sent[0][dev_id];

        switch (cmd) {
        /* GetStatus comand returns 2 bytes */
        case L6470_CMD_GET_STATUS:
            rx[0] = recv[0][dev_id];
            rx[1] = recv[1][dev_id];
            rx += 2;
            break;

        /* GetParam command returns 3 bytes */
        case L6470_CMD_GET_PARAM:
            rx[0] = recv[0][dev_id];
            rx[1] = recv[1][dev_id];
            rx[2] = recv[2][dev_id];
            rx += 3;
            break;

        default:
            break;
        }
    }
}
