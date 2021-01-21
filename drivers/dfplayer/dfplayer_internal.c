/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>

#include "dfplayer.h"
#include "dfplayer_constants.h"
#include "dfplayer_internal.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "thread.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Initial value of the frame check sequence
 */
static const uint16_t fcs_init = -(DFPLAYER_VERSION + DFPLAYER_LEN);

/**
 * @brief   Identify the source from an insert/eject event
 *
 * @param   dev     DFPlayer device descriptor
 *
 * @return  The source that was inserted / ejected
 * @retval  DFPLAYER_SOURCE_NUMOF   Unknown source
 */
static dfplayer_source_t _get_inserted_ejected_source(dfplayer_t *dev)
{
    switch (dev->buf[3]) {
        case DFPLAYER_DEVICE_USB:
            DEBUG("[dfplayer] Inserted/ejected USB storage device\n");
            return DFPLAYER_SOURCE_USB;
        case DFPLAYER_DEVICE_SDCARD:
            DEBUG("[dfplayer] Inserted/ejected SD card\n");
            return DFPLAYER_SOURCE_SDCARD;
    }

    DEBUG("[dfplayer] Insert/eject event with unknown source\n");
    return DFPLAYER_SOURCE_NUMOF;
}

/**
 * @brief   Handle a playback completed event
 *
 * @param   dev     DFPlayer device descriptor
 * @param   src     Medium the track was played from
 */
static void _handle_playback_completed(dfplayer_t *dev, dfplayer_source_t src)
{
    uint16_t track = (((uint16_t)dev->buf[2]) << 8) | dev->buf[3];
    DEBUG("[dfplayer] Playback of track %" PRIu16 " on medium %u completed\n",
          track, (unsigned)src);

    dev->flags |= DFPLAYER_FLAG_NO_ACK_BUG;

    /* Note: At least some revisions report playback completed more than once,
     * maybe to increase probability of the message reaching the MCU. This
     * de-duplicates the message by ignoring follow up messages for 100ms.
     * Filtering by track number and medium wouldn't work here, as the same
     * song might be played in repeat mode.
     */
    uint32_t now_us = xtimer_now_usec();
    if (dev->cb_done && (now_us - dev->last_event_us > DFPLAYER_TIMEOUT_MS * US_PER_MS)) {
        dev->cb_done(dev, src, track);
    }
    dev->last_event_us = now_us;
}

/**
 * @brief   Parse the bootup completed frame and init available sources
 *
 * @param   dev     DFPlayer device descriptor
 */
static void _handle_bootup_completed(dfplayer_t *dev)
{
    if (dev->buf[3] & DFPLAYER_MASK_USB) {
        dev->srcs |= 0x01 << DFPLAYER_SOURCE_USB;
    }

    if (dev->buf[3] & DFPLAYER_MASK_SDCARD) {
        dev->srcs |= 0x01 << DFPLAYER_SOURCE_SDCARD;
    }

    if (dev->buf[3] & DFPLAYER_MASK_FLASH) {
        dev->srcs |= 0x01 << DFPLAYER_SOURCE_FLASH;
    }

    /* Unblock caller of dfplayer_reset() */
    mutex_unlock(&dev->sync);
}

/**
 * @brief   Handle a notification message
 */
static void _handle_event_notification(dfplayer_t *dev)
{
    switch (dev->buf[0]) {
        case DFPLAYER_NOTIFY_INSERT:
            DEBUG("[dfplayer] Insert event\n");
            {
                dfplayer_source_t src = _get_inserted_ejected_source(dev);
                if (src < DFPLAYER_SOURCE_NUMOF) {
                    dev->srcs |= (dfplayer_source_set_t)(0x01 << src);
                }
            }
            if (dev->cb_src) {
                dev->cb_src(dev, dev->srcs);
            }
            return;
        case DFPLAYER_NOTIFY_EJECT:
            DEBUG("[dfplayer] Eject event\n");
            {
                dfplayer_source_t src = _get_inserted_ejected_source(dev);
                if (src < DFPLAYER_SOURCE_NUMOF) {
                    dev->srcs &= ~((dfplayer_source_set_t)(0x01 << src));
                }
            }
            if (dev->cb_src) {
                dev->cb_src(dev, dev->srcs);
            }
            return;
        case DFPLAYER_NOTIFY_DONE_USB:
            _handle_playback_completed(dev, DFPLAYER_SOURCE_USB);
            return;
        case DFPLAYER_NOTIFY_DONE_SDCARD:
            _handle_playback_completed(dev, DFPLAYER_SOURCE_SDCARD);
            return;
        case DFPLAYER_NOTIFY_DONE_FLASH:
            _handle_playback_completed(dev, DFPLAYER_SOURCE_FLASH);
            return;
        case DFPLAYER_NOTIFY_READY:
            _handle_bootup_completed(dev);
            return;
        default:
            DEBUG("[dfplayer] Unknown notification (%02x)\n", dev->buf[0]);
    }
}

/**
 * @brief   Parse the frame received from the DFPlayer Mini
 *
 * @param   dev     Device descriptor of the DFPlayer the frame received from
 *
 * The frame is stored in the buffer of the device descriptor
 */
static void _parse_frame(dfplayer_t *dev)
{
    assert(dev->len == DFPLAYER_LEN);
    switch (dev->buf[0] & DFPLAYER_CLASS_MASK) {
        case DFPLAYER_CLASS_NOTIFY:
            _handle_event_notification(dev);
            return;
        case DFPLAYER_CLASS_RESPONSE:
            /* Unblock thread waiting for response */
            mutex_unlock(&dev->sync);
            return;
    }

    DEBUG("[dfplayer] Got frame of unknown class\n");
}

/**
 * @brief   Function called when a byte was received over UART (ISR-context)
 *
 * @param   _dev    The corresponding device descriptor
 * @param   data    The received byte of data
 */
void dfplayer_uart_rx_cb(void *_dev, uint8_t data)
{
    dfplayer_t *dev = _dev;
    switch (dev->state) {
        case DFPLAYER_RX_STATE_START:
            if (data == DFPLAYER_START) {
                dev->state = DFPLAYER_RX_STATE_VERSION;
                return;
            }
            break;
        case DFPLAYER_RX_STATE_VERSION:
            if (data == DFPLAYER_VERSION) {
                dev->state = DFPLAYER_RX_STATE_LENGTH;
                return;
            }
            break;
        case DFPLAYER_RX_STATE_LENGTH:
            if (data == DFPLAYER_LEN) {
                dev->len = 0;
                dev->state = DFPLAYER_RX_STATE_DATA;
                return;
            }
            else {
                DEBUG("[dfplayer] Got frame with length %" PRIu8 ", but all "
                      "frames should have length 6\n", data);
            }
            break;
        case DFPLAYER_RX_STATE_DATA:
            /* We are a bit more liberal here and allow the end symbol to
             * appear in the payload of the frame, as the data sheet does not
             * mention any sort of escaping to prevent it from appearing in the
             * frame's payload. If bytes get lost and an and of frame symbol
             * is mistaken for a payload byte, this will be almost certainly
             * detected, as additionally a second end of frame symbol would
             * need to appear at the right position *and* the frame check
             * sequence need to match
             */
            if ((data == DFPLAYER_END) && (dev->len == DFPLAYER_LEN)) {
                uint16_t fcs_exp = fcs_init;
                fcs_exp -= dev->buf[0] + dev->buf[1] + dev->buf[2] + dev->buf[3];
                uint16_t fcs = (((uint16_t)dev->buf[4]) << 8) | dev->buf[5];
                if (fcs == fcs_exp) {
                    DEBUG("[dfplayer] Got 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
                          dev->buf[0], dev->buf[1], dev->buf[2],
                          dev->buf[3]);
                    _parse_frame(dev);
                }
                else {
                    DEBUG("[dfplayer] Checksum mismatch");
                }
            }
            else if (dev->len < sizeof(dev->buf)) {
                dev->buf[dev->len++] = data;
                return;
            }
            else {
                DEBUG("[dfplayer] Frame overflown\n");
            }
            break;
        default:
            break;
    }

    dev->state = DFPLAYER_RX_STATE_START;
}

static int _send(dfplayer_t *dev, uint8_t cmd, uint8_t p1, uint8_t p2,
                  uint32_t timeout_us)
{
    int retval;
    if (dev->flags & DFPLAYER_FLAG_NO_ACK_BUG) {
        /* Hardware bug: The next command will not be ack'ed, unless it is
         * a query command. We can clear the flag, as we issue now a fake query,
         * if needed.
         */
        dev->flags &= ~(DFPLAYER_FLAG_NO_ACK_BUG);
        if (cmd < DFPLAYER_LOWEST_QUERY) {
            /* Command is a control command, we query the volume and ignore the
             * result as work around */
            retval = _send(dev, DFPLAYER_CMD_GET_VOLUME, 0, 0,
                           DFPLAYER_TIMEOUT_MS * US_PER_MS);
            if (retval) {
                /* pass through error */
                return retval;
            }
        }
    }

    uint16_t fcs = fcs_init - (cmd + DFPLAYER_ACK + p1 + p2);
    uint8_t frame[] = {
        DFPLAYER_START, DFPLAYER_VERSION, DFPLAYER_LEN, cmd, DFPLAYER_ACK,
        p1, p2, (uint8_t)(fcs>>8), (uint8_t)fcs, DFPLAYER_END
    };

    for (unsigned i = 0; i < DFPLAYER_RETRIES; i++) {
        retval = 0;
        DEBUG("[dfplayer] About to exchange frame\n");
        /* Enforce that mutex is locked, so that xtimer_mutex_lock_timeout()
         * will not return immediately. */
        mutex_trylock(&dev->sync);
        uart_write(dev->uart, frame, sizeof(frame));

        if (xtimer_mutex_lock_timeout(&dev->sync, timeout_us)) {
            DEBUG("[dfplayer] Response timed out\n");
            retval = -ETIMEDOUT;
        }
        else {
            uint8_t code = dev->buf[0];
            if (code == DFPLAYER_RESPONSE_ERROR) {
                switch (dev->buf[3]) {
                    case DFPLAYER_ERROR_BUSY:
                        DEBUG("[dfplayer] Error: Module is busy\n");
                        retval = -EAGAIN;
                        break;
                    case DFPLAYER_ERROR_FRAME:
                        DEBUG("[dfplayer] Error: DFPlayer received incomplete "
                              "frame\n");
                        retval = -EIO;
                        break;
                    case DFPLAYER_ERROR_FCS:
                        DEBUG("[dfplayer] Error: DFPlayer received corrupt frame "
                              "(FCS mismatch)\n");
                        retval = -EIO;
                        break;
                    default:
                        DEBUG("[dfplayer] Unknown error!\n");
                        /* This should never be reached according the datasheet */
                        retval = -EIO;
                        break;
                }
            }
        }

        /* wait to work around HW bug */
        xtimer_msleep(DFPLAYER_SEND_DELAY_MS);

        if (!retval) {
            break;
        }
    }

    return retval;
}

int dfplayer_transceive(dfplayer_t *dev, uint16_t *resp,
                        uint8_t cmd, uint8_t p1, uint8_t p2)
{
    if (!dev) {
        return -EINVAL;
    }

    mutex_lock(&dev->mutex);

    int retval = _send(dev, cmd, p1, p2,  DFPLAYER_TIMEOUT_MS * US_PER_MS);
    if (retval) {
        mutex_unlock(&dev->mutex);
        return retval;
    }

    if (resp) {
        *resp = (((uint16_t)dev->buf[2]) << 8) | (uint16_t)dev->buf[3];
    }

    mutex_unlock(&dev->mutex);
    return 0;
}

int dfplayer_reset(dfplayer_t *dev)
{
    if (!dev) {
        return -EINVAL;
    }

    mutex_lock(&dev->mutex);

    int retval = _send(dev, DFPLAYER_CMD_RESET, 0, 0,
                       DFPLAYER_TIMEOUT_MS * US_PER_MS);

    if (retval) {
        mutex_unlock(&dev->mutex);
        return retval;
    }

    /* Enforce that mutex is locked, so that xtimer_mutex_lock_timeout()
     * will not return immediately. */
    mutex_trylock(&dev->sync);

    const uint32_t bootup_timeout = DFPLAYER_BOOTUP_TIME_MS * US_PER_MS;
    if (xtimer_mutex_lock_timeout(&dev->sync, bootup_timeout)) {
        mutex_unlock(&dev->mutex);
        DEBUG("[dfplayer] Waiting for device to boot timed out\n");
        return -ETIMEDOUT;
    }

    uint8_t code = dev->buf[0];
    mutex_unlock(&dev->mutex);

    if (code != DFPLAYER_NOTIFY_READY) {
        DEBUG("[dfplayer] Got unexpected response after reset\n");
        return -EIO;
    }

    return 0;
}

int dfplayer_file_cmd(dfplayer_t *dev, uint8_t cmd, uint8_t p1, uint8_t p2)
{
    int retval = _send(dev, cmd, p1, p2, DFPLAYER_TIMEOUT_MS * US_PER_MS);
    if (retval) {
        return retval;
    }

    /* Enforce that mutex is locked, so that xtimer_mutex_lock_timeout()
     * will not return immediately. */
    mutex_trylock(&dev->sync);

    const uint32_t timeout_us = DFPLAYER_TIMEOUT_MS * US_PER_MS;
    if (xtimer_mutex_lock_timeout(&dev->sync, timeout_us)) {
        /* For commands DFPLAYER_CMD_PLAY_FROM_MP3 (0x12) and
         * DFPLAYER_CMD_PLAY_ADVERT (0x13) a second reply is only generated on
         * failure. A timeout could be either:
         *   a) Success. DFPlayer is playing the selected file
         * or
         *   b) Failure, but the reply got lost (or was rejected due to mismatch
         *      of the frame check sequence)
         *
         * We just check if the DFPlayer is actually playing
         */
        if (gpio_is_valid(dev->busy_pin)) {
            retval = 0;
            /* Using BUSY pin to check if device is playing */
            if (gpio_read(dev->busy_pin)) {
                /* Device not playing, file does not exist */
                retval = -ENOENT;
            }
        }
        else {
            /* BUSY pin not connected, query status instead */
            retval = _send(dev, DFPLAYER_CMD_GET_STATUS, 0, 0, timeout_us);

            if (!retval) {
                uint8_t status = dev->buf[3];
                retval = (status & DFPLAYER_STATUS_PLAYING) ? 0 : -ENOENT;
            }
        }
    }
    uint8_t code = dev->buf[0];
    uint8_t error = dev->buf[3];

    if (retval) {
        return retval;
    }

    if (code == DFPLAYER_RESPONSE_ERROR) {
        /* The DFPlayer already acknowledged successful reception of the
         * command, so we expect that the only cause for an error is that the
         * file was not found. But better check anyway, the device is strange */
        if (error == DFPLAYER_ERROR_NO_SUCH_FILE) {
            return -ENOENT;
        }

        DEBUG("[dfplayer] Got unexpected error message\n");
        return -EIO;
    }

    return 0;
}
