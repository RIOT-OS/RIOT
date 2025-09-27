/*
 * SPDX-FileCopyrightText: 2019 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_dfplayer
 * @{
 *
 * @file
 * @brief       Implementation DFPlayer Mini Device Driver
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include "dfplayer.h"
#include "dfplayer_constants.h"
#include "dfplayer_internal.h"
#include "irq.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "thread.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int dfplayer_init(dfplayer_t *dev, const dfplayer_params_t *params)
{
    if (!dev || !params) {
        return -EINVAL;
    }

    static const mutex_t locked = MUTEX_INIT_LOCKED;
    memset(dev, 0x00, sizeof(*dev));
    dev->uart = params->uart;
    dev->busy_pin = params->busy_pin;
    mutex_init(&dev->mutex);
    dev->sync = locked;

    if (gpio_is_valid(dev->busy_pin)) {
        if (gpio_init(dev->busy_pin, GPIO_IN)) {
            DEBUG("[dfplayer] Initializing busy pin failed\n");
            return -EIO;
        }
    }

    int retval = uart_init(dev->uart, DFPLAYER_BAUD, dfplayer_uart_rx_cb, dev);
    if (retval != UART_OK) {
        DEBUG("[dfplayer] uart_init() failed (return value %d)\n", retval);
        return -EIO;
    }

    DEBUG("[dfplayer] Resetting device now\n");
    retval = dfplayer_reset(dev);

    if (retval) {
        DEBUG("[dfplayer] Failed to reset device (return value %d)\n", retval);
        return retval;
    }

    DEBUG("[dfplayer] dfplayer_init almost completed, setting volume\n");

    return dfplayer_set_volume(dev, params->volume);
}

dfplayer_source_set_t dfplayer_get_sources(dfplayer_t *dev)
{
    unsigned state = irq_disable();
    dfplayer_source_set_t result = dev->srcs;
    irq_restore(state);
    return result;
}

int dfplayer_set_callbacks(dfplayer_t *dev, dfplayer_cb_done_t cb_done,
                           dfplayer_cb_src_t cb_src)
{
    if (!dev) {
        return -EINVAL;
    }

    unsigned state = irq_disable();
    dev->cb_done = cb_done;
    dev->cb_src = cb_src;
    irq_restore(state);
    return 0;
}

int dfplayer_get_state(dfplayer_t *dev, dfplayer_state_t *state)
{
    if (!dev || !state) {
        return -EINVAL;
    }

    if (gpio_is_valid(dev->busy_pin)) {
        if (!gpio_read(dev->busy_pin)) {
            *state = DFPLAYER_STATE_PLAYING;
            return 0;
        }
    }

    uint16_t status;
    int retval = dfplayer_query(dev, &status, DFPLAYER_CMD_GET_STATUS);
    if (retval) {
        /* pass error through */
        return retval;
    }

    if (status & DFPLAYER_STATUS_PLAYING) {
        *state = DFPLAYER_STATE_PLAYING;
        return 0;
    }

    if (status & DFPLAYER_STATUS_PAUSE) {
        *state = DFPLAYER_STATE_PAUSED;
        return 0;
    }

    *state = DFPLAYER_STATE_STOPPED;
    return 0;
}

int dfplayer_play_file(dfplayer_t *dev, uint8_t folder, uint8_t file)
{
    int retval;
    if (!folder || !file || (folder > DFPLAYER_MAX_FOLDER)) {
        return -EINVAL;
    }

    mutex_lock(&dev->mutex);
    retval = dfplayer_file_cmd(dev, DFPLAYER_CMD_FILE, folder, file);
    if (retval) {
        mutex_unlock(&dev->mutex);
        return retval;
    }

    dev->file.folder = folder;
    dev->file.file = file;
    dev->file.scheme = DFPLAYER_SCHEME_FOLDER_FILE;
    mutex_unlock(&dev->mutex);
    return 0;
}

int dfplayer_play_from_mp3(dfplayer_t *dev, uint16_t number)
{
    int retval;
    if (!dev || !number || (number > DFPLAYER_MAX_MP3_FILE)) {
        return -EINVAL;
    }

    mutex_lock(&dev->mutex);
    retval = dfplayer_file_cmd(dev, DFPLAYER_CMD_PLAY_FROM_MP3,
                               (uint8_t)(number >> 8), (uint8_t) number);
    if (retval) {
        mutex_unlock(&dev->mutex);
        return retval;
    }

    dev->file.number = number;
    dev->file.scheme = DFPLAYER_SCHEME_MP3_FILE;
    mutex_unlock(&dev->mutex);
    return 0;
}

int dfplayer_play_from_advert(dfplayer_t *dev, uint16_t number)
{
    int retval;
    if (!dev || !number || (number > DFPLAYER_MAX_ADVERT_FILE)) {
        return -EINVAL;
    }

    mutex_lock(&dev->mutex);
    retval = dfplayer_file_cmd(dev, DFPLAYER_CMD_PLAY_ADVERT,
                               (uint8_t)(number >> 8), (uint8_t) number);
    mutex_unlock(&dev->mutex);
    return retval;
}

int dfplayer_step(dfplayer_t *dev, int step)
{
    int retval;
    if (!dev) {
        return -EINVAL;
    }

    mutex_lock(&dev->mutex);
    uint8_t cmd, p1, p2;
    if (dev->file.scheme == DFPLAYER_SCHEME_FOLDER_FILE) {
        /* Currently using naming scheme <FOLDERNUM>/<FILENUM>.mp3 */
        if ((dev->file.file + step < 1) ||
            (dev->file.file + step > (int)UINT8_MAX))
        {
            mutex_unlock(&dev->mutex);
            return -ERANGE;
        }
        cmd = DFPLAYER_CMD_FILE;
        p1 = dev->file.folder;
        p2 = (uint8_t)(dev->file.file + step);
    }
    else {
        /* Currently using naming scheme MP3/<FILENUM>.mp3 */
        if ((dev->file.number + step < 1) ||
            (dev->file.number + step > DFPLAYER_MAX_MP3_FILE))
        {
            mutex_unlock(&dev->mutex);
            return -ERANGE;
        }
        cmd = DFPLAYER_CMD_PLAY_FROM_MP3;
        p1 = (uint8_t)((dev->file.number + step) >> 8);
        p2 = (uint8_t)(dev->file.number + step);
    }

    retval = dfplayer_file_cmd(dev, cmd, p1, p2);
    if (retval) {
        mutex_unlock(&dev->mutex);
        return retval;
    }

    if (dev->file.scheme == DFPLAYER_SCHEME_FOLDER_FILE) {
        /* Currently using naming scheme <FOLDERNUM>/<FILENUM>.mp3 */
        dev->file.file = (uint8_t)(dev->file.file + step);
    }
    else {
        /* Currently using naming scheme MP3/<FILENUM>.mp3 */
        dev->file.number = (uint16_t)(dev->file.number + step);
    }

    mutex_unlock(&dev->mutex);
    return 0;
}
