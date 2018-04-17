/*
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <fmt.h>
#include "log.h"
#include "gsm/gsm_sms.h"

/**
 * @ingroup     drivers_gsm
 * @{
 *
 * @file
 * @brief       gsm sms implementation.
 *
 * @author      Max van Kessel <maxvankessel@betronic.nl>
 *
 * @}
 */

#define LOG_HEADER  "gsm_sms: "

#define GSM_SMS_FIELD_NUMOF     (11)
#define GSM_SMS_FIELD_SENDER_ID (1)
#define GSM_SMS_FIELD_DATE_ID   (3)
#define GSM_SMS_FIELD_LEN_ID    (GSM_SMS_FIELD_NUMOF - 1)

static void sms_callback(void *arg, const char *buf);

int gsm_sms_init(gsm_sms_t *sms, gsm_t *dev)
{
    int err = -EINVAL;

    assert(dev);
    assert(sms);

    sms->base = dev;

    return err;
}

int gsm_sms_set_format(gsm_sms_t * sms, gsm_sms_message_format_t format)
{
    int err = -EINVAL;

    if(sms) {
        gsm_t *dev = sms->base;
        char buf [GSM_AT_LINEBUFFER_SIZE_SMALL];
        char *pos = buf;

        assert(dev);

        pos += fmt_str(pos, "AT+CMGF=");
        pos += fmt_u32_dec(pos, format);
        *pos = '\0';

        rmutex_lock(&dev->mutex);

        err = at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);

        rmutex_unlock(&dev->mutex);
    }
    return err;
}

int gsm_sms_set_characters(gsm_sms_t * sms, const char * set)
{
    int err = -EINVAL;

    if(sms) {
        gsm_t *dev = sms->base;
        char buf[GSM_AT_LINEBUFFER_SIZE_SMALL];
        char *pos = buf;

        assert(dev);

        pos += fmt_str(pos, "AT+CSCS=");

        if(set) {
            pos += fmt_str(pos, set);
        }
        else {
            pos += fmt_str(pos, GSM_SMS_DEFAULT_CHARACTER_SET);
        }
        *pos = '\0';

        rmutex_lock(&dev->mutex);

        err = at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);

        rmutex_unlock(&dev->mutex);
    }

    return err;
}

int gsm_sms_enable_reception(gsm_sms_t * sms, gsm_sms_cb_t cb, void *arg)
{
    int err = -EINVAL;

    if(sms) {
        gsm_t *dev = sms->base;
        char buf[GSM_AT_LINEBUFFER_SIZE_SMALL];

        rmutex_lock(&dev->mutex);

        sms->callback = cb;
        sms->callback_args = arg;

        /* new message indications */
        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CNMI=1,1",
                GSM_SERIAL_TIMEOUT_US);

        if(err < 0) {
            goto out;
        }

        /* set header info */
        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CSDH=1",
                GSM_SERIAL_TIMEOUT_US);

        if(err < 0) {
            goto out;
        }

        /* set default storage, stores sms into modem */
        err = at_send_cmd_get_lines(&dev->at_dev,
                "AT+CPMS=\"ME\",\"ME\",\"ME\"", buf,
                GSM_AT_LINEBUFFER_SIZE_SMALL, false, GSM_SERIAL_TIMEOUT_US);

        if(err < 0) {
            goto out;
        }

        /* delete all messages */
        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CMGD=0,4",
                GSM_SERIAL_TIMEOUT_US);

        gsm_register_urc_callback(dev, "+CMTI", sms_callback, sms);

out:
        rmutex_unlock(&dev->mutex);
    }

    return err;
}


static void sms_callback(void *arg, const char *buf)
{
    if(strncmp(buf, "+CMTI:", 6) == 0) {
        gsm_sms_t * sms_dev = (gsm_sms_t *)arg;

        if((sms_dev) && (sms_dev->callback)) {
            gsm_t * dev = sms_dev->base;

            char resp_buf[64];
            char sms_buf[256];
            char req_buf[32];
            char *sms = NULL;
            char *sender = NULL;
            char *date = NULL;
            unsigned long len = 0;
            unsigned i = 0;

            LOG_DEBUG("SMS received\n");

            rmutex_lock(&dev->mutex);
            /* Decode */

            unsigned long index = strtoul(buf + 12, NULL, 10);
            char *pos = req_buf;

            pos += fmt_str(pos, "AT+CMGR=");
            pos += fmt_u32_dec(pos, index);
            *pos = '\0';

            int res = at_send_cmd_get_lines(&dev->at_dev, req_buf,
                    sms_buf, sizeof(sms_buf), false,
                    GSM_SERIAL_TIMEOUT_US);

            if(res < 0) {
                goto out;
            }

            char *cur = sms_buf;
            do {
                if(i == GSM_SMS_FIELD_SENDER_ID) {
                    sender = cur + 1; /* remove leading '"' */
                }
                if(i == GSM_SMS_FIELD_DATE_ID) {
                    date = cur + 1; /* remove leading '"' */
                    if(*cur == '\"') {
                        while(*cur != ',') {
                            cur++;
                        }
                        cur++;
                    }
                }
                if(i == GSM_SMS_FIELD_LEN_ID) {
                    len = strtoul(cur, NULL, 0);
                }
                while(*cur != ',' && *cur != '\n') {
                    cur++;
                }
                *cur = '\0';
                cur++;
                i++;
            } while(i < GSM_SMS_FIELD_NUMOF);

            if(sender) {
                sender[strlen(sender) - 1] = '\0'; /* remove trailing '"' */
            }
            if(date) {
                date[strlen(date) - 1] = '\0'; /* remove trailing '"' */
            }

            pos = req_buf;
            pos += fmt_str(pos, "AT+CMGD=");
            pos += fmt_u32_dec(pos, index);
            *pos = '\0';

            res = at_send_cmd_get_lines(&dev->at_dev, req_buf, resp_buf,
                    sizeof(resp_buf), true, GSM_SERIAL_TIMEOUT_US);

            if(res < 0) {
                goto out;
            }

            if(len) {
                while((*cur == '\r' || *cur == '\n') && (*cur != '\0')) {
                    cur++;
                }
                sms = cur;
                sms[len] = '\0';
            }
out:
            rmutex_unlock(&dev->mutex);
            sms_dev->callback(sms_dev->callback_args, sms, sender, date);

        }
    }
}
