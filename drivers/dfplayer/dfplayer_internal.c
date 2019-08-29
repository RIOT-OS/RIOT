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

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Initial value of the FCS
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

    if (dev->cb_done) {
        dev->cb_done(src, track, dev->userdata);
    }
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
    thread_wakeup(dev->sync);
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
                dev->cb_src(dev->srcs, dev->userdata);
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
                dev->cb_src(dev->srcs, dev->userdata);
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
            thread_wakeup(dev->sync);
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
             * need to appear at the right position *and* the FCS need to match
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
static void _timeout_cb(void *_dev)
{
    dfplayer_t *dev = _dev;
    dev->flags |= DFPLAYER_FLAG_TIMEDOUT;
    thread_wakeup(dev->sync);
}

static int _send(dfplayer_t *dev, uint8_t cmd, uint8_t p1, uint8_t p2,
                  uint32_t timeout_us)
{
    if (dev->flags & DFPLAYER_FLAG_NO_ACK_BUG) {
        /* Hardware bug: The next command will not be ack'ed, unless it is
         * a query command. We can clear the flag, as  */
        dev->flags &= ~(DFPLAYER_FLAG_NO_ACK_BUG);
        if (cmd < DFPLAYER_LOWSET_QUERY) {
            /* Command is a control command, we query the volume and ignore the
             * result as work around */
            int retval = _send(dev, DFPLAYER_CMD_GET_VOLUME, 0, 0,
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

    /* clear timeout flag */
    dev->flags &= ~(DFPLAYER_FLAG_TIMEDOUT);

    /* set timeout timer */
    xtimer_t timer = {
        .callback = _timeout_cb,
        .arg = dev
    };
    DEBUG("[dfplayer] About to exchange frame\n");
    xtimer_set(&timer, timeout_us);

    uart_write(dev->uart, frame, sizeof(frame));

    /* wait for reply from ISR */
    dev->sync = thread_getpid();
    thread_sleep();
    dev->sync = KERNEL_PID_UNDEF;

    /* In case no timeout occurred, stop timer */
    xtimer_remove(&timer);

    if (dev->flags & DFPLAYER_FLAG_TIMEDOUT) {
        DEBUG("[dfplayer] Response timed out\n");
        return -ETIMEDOUT;
    }

    uint8_t code = dev->buf[0];
    if (code == DFPLAYER_RESPONSE_ERROR) {
        switch (dev->buf[3]) {
            case DFPLAYER_ERROR_BUSY:
                DEBUG("[dfplayer] Error: Module is busy\n");
                return -EAGAIN;
            case DFPLAYER_ERROR_FRAME:
                DEBUG("[dfplayer] Error: DFPlayer received incomplete frame\n");
                return -EIO;
            case DFPLAYER_ERROR_FCS:
                DEBUG("[dfplayer] Error: DFPlayer received corrupt frame "
                      "(FCS mismatch)\n");
                return -EIO;
            default:
                DEBUG("[dfplayer] Unknown error!\n");
                /* This should never be reached if the data sheet is correct */
                return -EIO;
        }
    }

    return 0;
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

    /* wait to work around HW bug */
    xtimer_usleep(DFPLAYER_SEND_DELAY_MS * US_PER_MS);

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

    /* Await notification the DFPlayer Mini is available */
    /* set timeout timer */
    xtimer_t timer = {
        .callback = _timeout_cb,
        .arg = dev
    };
    xtimer_set(&timer, DFPLAYER_BOOTUP_TIME_MS * US_PER_MS);
    dev->sync = thread_getpid();
    thread_sleep();
    dev->sync = KERNEL_PID_UNDEF;

    /* In case no timeout occurred, stop timer */
    xtimer_remove(&timer);

    if (dev->flags & DFPLAYER_FLAG_TIMEDOUT) {
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

    /* The DFPlayer did acknowledge reception of the command, but did not yet
     * check if the file exists. We wait for a second reply */
    xtimer_t timer = {
        .callback = _timeout_cb,
        .arg = dev
    };
    xtimer_set(&timer, DFPLAYER_TIMEOUT_MS * US_PER_MS);
    dev->sync = thread_getpid();
    thread_sleep();
    dev->sync = KERNEL_PID_UNDEF;

    /* In case no timeout occurred, stop timer */
    xtimer_remove(&timer);

    uint8_t code = dev->buf[0];
    uint8_t error = dev->buf[3];

    if (dev->flags & DFPLAYER_FLAG_TIMEDOUT) {
        /* For command DFPLAYER_CMD_FILE (0x0f) a second replay is generated
         * also on success. A timeout is an error in that case */
        if (cmd == DFPLAYER_CMD_FILE) {
            return -ETIMEDOUT;
        }

        /* For commands DFPLAYER_CMD_PLAY_FROM_MP3 (0x12) and
         * DFPLAYER_CMD_PLAY_ADVERT (0x13) a second reply is only generated on
         * failure. A timeout could be either:
         *   a) Success. DFPlayer is playing the selected file
         * or
         *   b) Failure, but the reply got lost (or was rejected due to FCS
         *      mismatch)
         *
         * We want to give our best to still detect any potential error. If
         * the busy pin is connected, we can use that to confirm that the device
         * is actually playing.
         */
        if (dev->busy_pin != GPIO_UNDEF) {
            if (gpio_read(dev->busy_pin)) {
                /* Device not playing, file does not exist */
                return -ENOENT;
            }

            return 0;
        }

        /* We could theoretically query the status to check if the device is
         * playing. But using the UART communication during playback results
         * in audible glitches, and we do not want to sabotage sound quality.
         * Thus, we just return success and hope for the best. (Most of the time
         * the UART should be reliable enough to actually get the error reply,
         * if something went wrong.)
         */
        DEBUG("[dfplayer] Unable to confirm that last cmd succeeded.\n");
        return 0;
    }

    /* wait to work around HW bug */
    xtimer_usleep(DFPLAYER_SEND_DELAY_MS * US_PER_MS);

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
