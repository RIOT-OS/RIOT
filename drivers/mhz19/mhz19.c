/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mhz19
 * @{
 *
 * @file
 * @brief       Device driver implementation for MH-Z19 CO2 sensor.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Christian Manal <manal@uni-bremen.de>
 *
 * @}
 */

#include "mhz19.h"
#include "mhz19_params.h"
#include "xtimer.h"
#include "mutex.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

#ifdef MODULE_MHZ19_UART
#include "mhz19_internals.h"

/* Precalculated command sequence */
static const uint8_t value_read[] = {
    MHZ19_READ_START,
    MHZ19_READ_SENSOR_NUM,
    MHZ19_REG_GAS_CONCENTRATION,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    MHZ19_READ_GAS_CHECKSUM
};

static void _mhz19_timeout(void *arg)
{
    mhz19_t *dev = arg;

    mutex_unlock(&(dev->sync));
}

static void _mhz19_rx_cb(void *arg, uint8_t byte)
{
    mhz19_t *dev = arg;

    /* Skip start byte and skip out of array bounds writes */
    if ((dev->idx == 0 && byte == 0xff) || dev->idx >= MHZ19_BUF_SIZE) {
        return;
    }
    /* Store byte and increment idx */
    dev->rxmem[dev->idx++] = byte;
    if (dev->idx == MHZ19_BUF_SIZE) {
        mutex_unlock(&dev->sync);
    }
}

int mhz19_init(mhz19_t *dev, const mhz19_params_t *params)
{
    dev->params = params;

    DEBUG("mhz19: initializing device %p on UART %i\n",
          (void *)dev, dev->params->uart);
    mutex_init(&dev->mutex);
    mutex_init(&dev->sync);
    dev->idx = 0;
    /* Initialize UART interface */
    if (uart_init(params->uart, MHZ19_UART_BAUDRATE, _mhz19_rx_cb, dev)) {
        DEBUG("[Error] UART device not enabled\n");
        return MHZ19_ERR_UART;
    }
    DEBUG("mhz19: Initialization complete\n");
    return MHZ19_OK;
}

/*
 * Do a raw send/receive exchange to the sensor. As exchanges between the
 * MH-Z19 and the host always consists of 9 bytes in each direction, the size
 * of the input and output arrays is fixed at 9 bytes here. The returned bytes
 * from the MH-Z19 appear in mhz19_t::rxmem
 *
 * @param[in]  dev   The mhz19 device context
 * @param[in]  out   the 9 bytes to transmit to the device
 */
static void mhz19_xmit(mhz19_t *dev, const uint8_t *in)
{
    /* MH-Z19 timout timer, for the case the sensor doesn't respond */
    xtimer_t timer;
    timer.callback = _mhz19_timeout;
    timer.arg = dev;
    timer.target = 0;
    timer.long_target = 0;

    /* Reset the buffer index to zero */
    dev->idx = 0;

    /* Send read command to the sensor */
    uart_write(dev->params->uart, in, MHZ19_BUF_SIZE + 1);

    /* First time locking the synchronisation mutex */
    mutex_lock(&dev->sync);
    /* Schedule timeout wakeup callback */
    xtimer_set(&timer, MHZ19_READ_TIMEOUT * US_PER_MS);

    /* By locking the same mutex another time, this thread blocks until
     * something else calls the unlock. In this case it has to wait for
     * either the timeout xtimer callback or the UART ISR having received 9
     * bytes. This way we have both a timeout scheduled and a regular wait
     * for the UART to finish.
     */
    mutex_lock(&dev->sync);

    /* Unlock mutex again to reset it to unlocked */
    mutex_unlock(&dev->sync);
    /* Clean up timer in case it didn't fire yet */
    xtimer_remove(&timer);

}

int mhz19_get_ppm(mhz19_t *dev, int16_t *ppm)
{
    int res = MHZ19_OK;

    /* First lock, guarantees no concurrent access to the UART device */
    mutex_lock(&dev->mutex);
    DEBUG("mhz19: Starting measurement\n");
    mhz19_xmit(dev, value_read);

    DEBUG("mhz19: Checking buffer: %d\n", dev->idx);
    /* MHZ19_BUF_SIZE indicates completely filled buffer */
    if (dev->idx == MHZ19_BUF_SIZE) {
        uint8_t checksum = 0;
        /* MHZ19_BUF_SIZE - 1 to exclude the received checksum */
        for (unsigned i = 0; i < MHZ19_BUF_SIZE - 1; i++) {
            checksum -= dev->rxmem[i];
        }
        if (checksum == dev->rxmem[MHZ19_RX_POS_CHECKSUM]) {
            *ppm = dev->rxmem[MHZ19_RX_POS_PPM_HIGH] << 8;
            *ppm += dev->rxmem[MHZ19_RX_POS_PPM_LOW];
            res = MHZ19_OK;
        }
        else {
            /* Checksum mismatch */
            DEBUG("mhz19: Checksum failed, calculated 0x%x != 0x%x\n", checksum, dev->rxmem[MHZ19_RX_POS_CHECKSUM]);
            res = MHZ19_ERR_CHECKSUM;
        }
    }
    else {
        DEBUG("mhz19: Timeout trying to retrieve measurement\n");
        res = MHZ19_ERR_TIMEOUT;
    }
    /* Unlock concurrency guard mutex */
    mutex_unlock(&dev->mutex);

    return res;
}

#endif /* MODULE_MHZ19_UART */

#ifdef MODULE_MHZ19_PWM

int mhz19_init(mhz19_t *dev, const mhz19_params_t *params)
{
    int16_t res;
    dev->pin = params->pin;
    gpio_init(dev->pin, GPIO_IN_PD);
    /* Take one measurement to make sure there's an mhz19 on the pin */
    if (mhz19_get_ppm(dev, &res) == MHZ19_ERR_TIMEOUT) {
        return MHZ19_ERR_TIMEOUT;
    }
    return MHZ19_OK;
}

int mhz19_get_ppm(mhz19_t *dev, int16_t *ppm)
{
    uint32_t start, middle, end, th, tl;
    /*
     * Per the docs, one sample should take 1004ms +-5%. Worst case is
     * that  we come in right after the rising edge of the current cycle,
     * so we wanna wait two cycles plus some wiggle room at most for
     * a measurement.
     */
    int16_t timeout = 2200;

    DEBUG("%s: Waiting for high level to end\n", __func__);
    while (gpio_read(dev->pin) && timeout) {
        timeout--;
        xtimer_usleep(US_PER_MS);
    }

    DEBUG("%s: Waiting for initial rising edge\n", __func__);
    while (!gpio_read(dev->pin) && timeout) {
        timeout--;
        xtimer_usleep(US_PER_MS);
    }

    start = xtimer_now_usec() / US_PER_MS;
    DEBUG("%s: Waiting for falling edge\n", __func__);
    while (gpio_read(dev->pin) && timeout) {
        timeout--;
        xtimer_usleep(US_PER_MS);
    }
    middle = xtimer_now_usec() / US_PER_MS;
    DEBUG("%s: Waiting for rising edge\n", __func__);
    while (!gpio_read(dev->pin) && timeout) {
        timeout--;
        xtimer_usleep(US_PER_MS);
    }

    /* If we waited too long for flanks, something went wrong */
    if (!timeout) {
        DEBUG("%s: Measurement timed out\n", __func__);
        return MHZ19_ERR_TIMEOUT;
    }
    end = xtimer_now_usec() / US_PER_MS;

    th = (middle - start);
    tl = (end - middle);

    *ppm = (int16_t)(2000 * (th - 2) / (th + tl - 4));

    return MHZ19_OK;
}

#endif /* MODULE_MHZ19_PWM */
