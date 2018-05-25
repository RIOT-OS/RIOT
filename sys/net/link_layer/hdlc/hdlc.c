/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup net_hdlc
 * @file
 * @brief   Layered implementation of the HDLC protocol
 *
 * @author  Max van Kessel
 *
 * @}
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

#include "net/netdev.h"
#include "net/netdev/layer.h"

#include "net/hdlc/fcs.h"
#include "net/hdlc.h"

#include "log.h"

#define MODULE  "hdlc: "

#define FCS16_INIT              (0xFFFF)
#define FCS16_GOOD              (0xF0B8)

static int _init(netdev_t *netdev)
{
    hdlc_t *dev = (hdlc_t *)netdev;

    if(netdev->lower != NULL) {
        netdev->lower->event_callback =  netdev_event_cb_pass;
    }

    /* initialize buffers */
   tsrb_init(&dev->inbuf, (char*)dev->rxmem, HDLC_BUFSIZE);

   return netdev_init_pass(netdev);
}

static int _tsrb_peek_one(const tsrb_t * rb)
{
    int res = -1;

    if(!tsrb_empty(rb)) {
        res = (unsigned char)rb->buf[rb->reads & (rb->size - 1)];
    }

    return res;
}

static void _drop_input(hdlc_t *dev)
{
    int c = tsrb_get_one(&dev->inbuf);

    while((c > 0) && (c != HDLC_FLAG_SEQUENCE)) {
        c = tsrb_get_one(&dev->inbuf);
    }
}

static int _rx_cb(void *arg, uint8_t byte)
{
    int res = 0;
    hdlc_t *dev = arg;

    /* special character */
    if ((byte == HDLC_CONTROL_ESCAPE) || (byte == HDLC_FLAG_SEQUENCE)) {
        if (byte == HDLC_CONTROL_ESCAPE) {
            dev->esc = 1;
        }
        else if (byte == HDLC_FLAG_SEQUENCE) {
            if (dev->state <= HDLC_ADDRESS) {
                /* ignore */
            }
            else if (dev->state < HDLC_DATA) {
                /* drop, incomplete*/
                _drop_input(dev);
            }
            else if (dev->fcs != FCS16_GOOD) {
                /* drop, bad fcs */
                _drop_input(dev);
                LOG_WARNING(MODULE"bad fcs, dropping frame\n");
            }
            else {
                /* complete package */
                res = (int)tsrb_avail(&dev->inbuf);
            }
            /* new packet preparation */
            dev->fcs = FCS16_INIT;
            dev->state = HDLC_ADDRESS;
            dev->esc = 0;

            /* add sequence flag */
            tsrb_add_one(&dev->inbuf, byte);
        }
    }
    else {
        if (dev->esc) {
            dev->esc = 0;
            byte ^= HDLC_SIX_CMPL;
        }

        switch (dev->state) {
            case HDLC_IDLE:
                /* fall through */
            case HDLC_START:
                dev->fcs = FCS16_INIT;
                /* fall through */
            case HDLC_ADDRESS:
                dev->state = HDLC_CONTROL;
                break;
            case HDLC_CONTROL:
                dev->state = HDLC_DATA;
                break;
            case HDLC_DATA:
                break;
        }

        tsrb_add_one(&dev->inbuf, byte);

        dev->fcs = fcs16_bit(dev->fcs, byte);
    }

    return res;
}

static int _recv(netdev_t *netdev, void *buf, size_t len, void *info)
{
    int res = 0;
    hdlc_t *dev = (hdlc_t *)netdev;
    size_t length = len;

    (void)info;
    uint8_t *ptr = buf;

    if(len > 0) {
        LOG_DEBUG(MODULE);
    }

    for(; length > 0; length--) {
        int byte;

        if ((byte = _tsrb_peek_one(&dev->inbuf)) < 0) {
            /* something went wrong, return error */
            return -EIO;
        }

        LOG_DEBUG("%02x ", byte);

        /* start or restart */
        if(byte == HDLC_FLAG_SEQUENCE) {
            if(res >= 2) {
                /* complete, remove checksum */
                res -= 2;

                LOG_DEBUG("[IN]\n");

                return res;
            }
        }
        else {
            /* drop if buf is null */
            if(buf) {
                *(ptr++) = (uint8_t)byte;
                res++;
            }
        }

        /* remove from buffer */
        (void)tsrb_get_one(&dev->inbuf);
    }

    if(len == 0) {
        /* the user was warned not to use a buffer size > `INT_MAX` ;-) */
       res = (int)tsrb_avail(&dev->inbuf);
    }

    return res;
}

static void _isr(netdev_t *netdev)
{
    hdlc_t * dev = (hdlc_t *)netdev;

    int bytes_expected = netdev_recv_pass(netdev, NULL, 0, NULL);

    for(; bytes_expected; --bytes_expected) {
        uint8_t byte;

        if(netdev_recv_pass(netdev, &byte, 1, NULL) > 0) {
            if(_rx_cb(dev, byte) > 0) {

                LOG_INFO(MODULE"frame received\n");

                if (netdev->event_callback != NULL) {
                    netdev->event_callback((netdev_t *)dev, NETDEV_EVENT_RX_COMPLETE);
                }
            }
        }
    }
}

static uint8_t * _add(uint8_t *arr, uint8_t byte, bool flag, uint16_t *fcs)
{
    uint8_t c = byte;

    if (fcs) {
        *fcs = fcs16_bit(*fcs, byte);
    }

    if (!flag) {
        if ((c == HDLC_FLAG_SEQUENCE) || (c == HDLC_CONTROL_ESCAPE)) {
            *arr++ = HDLC_CONTROL_ESCAPE;
            c ^= HDLC_SIX_CMPL;
        }
    }

    *arr++ = c;

    return arr;
}

static int _send(netdev_t *netdev, const iolist_t *iolist)
{
    hdlc_t *dev = (hdlc_t *)netdev;
    uint16_t fcs = FCS16_INIT;
    uint8_t * ptr = dev->txmem;

    LOG_DEBUG(MODULE"%02x ", HDLC_FLAG_SEQUENCE);

    ptr = _add(ptr, HDLC_FLAG_SEQUENCE, true, NULL);

    for(const iolist_t *iol = iolist; iol; iol = iol->iol_next) {
        uint8_t * data = iol->iol_base;

        for(unsigned j = 0; j < iol->iol_len; j++, data++) {
            ptr = _add(ptr, *data, false, &fcs);

            LOG_DEBUG("%02x ", *data);
        }
    }

    fcs ^= 0xffff;
    ptr = _add(ptr, (uint8_t) fcs & 0x00ff, false, NULL);
    LOG_DEBUG("%02x ", (uint8_t)fcs & 0x00ff);

    ptr = _add(ptr, (uint8_t) (fcs >> 8) & 0x00ff, false, NULL);
    LOG_DEBUG("%02x ", (uint8_t)(fcs >> 8));

    ptr = _add(ptr, HDLC_FLAG_SEQUENCE, true, NULL);

    LOG_DEBUG("%02x [OUT]\n", HDLC_FLAG_SEQUENCE);

    iolist_t new_iolist = {
            .iol_next = NULL,
            .iol_base = dev->txmem,
            .iol_len = ptr - dev->txmem,
    };

    return netdev_send_pass(netdev, &new_iolist);
}

static const netdev_driver_t hdlc_driver = {
    .send = _send,
    .recv = _recv,
    .init = _init,
    .isr = _isr,
    .get = netdev_get_pass,
    .set = netdev_set_pass,
};


void hdlc_setup(hdlc_t *dev)
{
    netdev_t *netdev = (netdev_t *)&dev->netdev;

    netdev->driver = &hdlc_driver;
}

void hdlc_hdr_print(hdlc_hdr_t *hdr)
{
    printf("   address: %" PRIu8 "\n", hdr->address);

    if(hdr->control.frame & (1 << 7)) {
        /* s or u frame */
        if(hdr->control.s.id == (1 << 0)) {
            printf("   control frame: supervisory\n");
            printf("   control recv seq: %" PRIu8 "\n", hdr->control.s.sequence_no);
            printf("   control type: %" PRIu8 "\n", hdr->control.s.type);
        }
        else {
            printf("   control frame: unnumbered\n");
            printf("   control type: %" PRIu8 "\n", hdr->control.u.type);
            printf("   control type: %" PRIu8 "\n", hdr->control.u.type_x);
        }
    }
    else {
        /* information frame */
        printf("   control frame: information\n");
        printf("   control recv seq: %" PRIu8 "\n", hdr->control.i.sequence_no);
        printf("   control send seq: %" PRIu8 "\n", hdr->control.i.send_sequence_no);
    }

    printf("   control : %" PRIu8 "\n", hdr->control.frame);
}
