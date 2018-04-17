/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2018 OTA keys S.A.
 * Copyright (C) 2018 Max van Kessel <maxvankessel@betronic.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "log.h"
#include "gsm.h"
#include "fmt.h"
#include "xtimer.h"

/**
 * @ingroup     drivers_gsm
 * @{
 *
 * @file
 * @brief       Generic gsm implementation.
 *
 * @author
 *
 * @}
 */

#define LOG_HEADER  "gsm: "

static void * idle_thread(void *arg);
static void creg_cb(void *arg, const char *buf);

int gsm_init(gsm_t *dev, gsm_params_t *params)
{
    int err = -EINVAL;

    if(dev){

        /* initialize and lock device */
        rmutex_init(&dev->mutex);
        rmutex_lock(&dev->mutex);

        /* store provided parameters in device */
        dev->params = params;

        /* initialize AT device */
        err = at_dev_init(&dev->at_dev, params->uart, params->baudrate,
                dev->buffer, GSM_UART_BUFSIZE);

        if(err) {
            LOG_WARNING(LOG_HEADER"failed to initialize at parser with %d\n", err);
            goto out;
        }

        if((dev->driver) && (dev->driver->init_base)) {
            err = dev->driver->init_base(dev);

            if(err) {
                LOG_WARNING(LOG_HEADER"failed to initialize base with %d\n", err);
                goto out;
            }
        }

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CREG=1", GSM_SERIAL_TIMEOUT_US);
        if(err < 0) {
            LOG_INFO(LOG_HEADER"failed to enable unsolicited result for CREG\n");
        }

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CMEE=1", GSM_SERIAL_TIMEOUT_US);
        if(err < 0) {
            LOG_INFO(LOG_HEADER"failed to enable extended error notification CMEE\n");
        }

        dev->pid = thread_create(dev->stack, GSM_THREAD_STACKSIZE, GSM_THREAD_PRIO,
                THREAD_CREATE_STACKTEST, idle_thread, dev, "gsm");

out:
        rmutex_unlock(&dev->mutex);
    }

    return err;
}

int gsm_power_on(gsm_t *dev)
{
    int err = -EINVAL;

    if(dev) {
        rmutex_lock(&dev->mutex);

        if((dev->driver) && (dev->driver->power_on)) {
            err = dev->driver->power_on(dev);

            if(err <= 0) {
                LOG_WARNING(LOG_HEADER"failed to power on\n");
            }
            else {
                dev->state = GSM_ON;
            }
        }
        rmutex_unlock(&dev->mutex);
    }

    return err;
}

void gsm_power_off(gsm_t *dev)
{
    if(dev) {
        int err = -EINVAL;
        rmutex_lock(&dev->mutex);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CFUN=0",
                GSM_SERIAL_TIMEOUT_US * 30);

        if((dev->driver) && (dev->driver->power_off)) {
            err = dev->driver->power_off(dev);

            if(err <= 0) {
                LOG_WARNING(LOG_HEADER"failed to power off\n");
            }
            else {
                dev->state = GSM_OFF;
            }
        }
        rmutex_unlock(&dev->mutex);
    }
}

int gsm_enable_radio(gsm_t *dev)
{
    int err = -EINVAL;

    if(dev) {
        rmutex_lock(&dev->mutex);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CFUN=1",
                GSM_SERIAL_TIMEOUT_US * 30);

        rmutex_unlock(&dev->mutex);
    }

    return err;
}

int gsm_disable_radio(gsm_t *dev)
{
    int err = -EINVAL;

    if(dev) {
        rmutex_lock(&dev->mutex);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CFUN=0",
                GSM_SERIAL_TIMEOUT_US * 30);

        rmutex_unlock(&dev->mutex);
    }

    return err;
}

int gsm_set_puk(gsm_t *dev, const char *puk, const char *pin)
{
    int res = -EINVAL;

    if((strlen(pin) == 4)) {
        char buf[GSM_AT_LINEBUFFER_SIZE_SMALL];
        char *pos = buf;
        bool quotes = false;

        pos += fmt_str(pos, "AT+CPIN=");
        if(strlen(puk) == 8) {
            pos += fmt_str(pos, "\"");
            pos += fmt_str(pos, puk);
            pos += fmt_str(pos, "\",\"");

            quotes = true;
        }
        pos += fmt_str(pos, pin);
        if(quotes){
            pos += fmt_str(pos, "\"");
        }
        *pos = '\0';

        rmutex_lock(&dev->mutex);

        res = at_send_cmd_get_resp(&dev->at_dev, buf, buf,
        GSM_AT_LINEBUFFER_SIZE_SMALL, GSM_SERIAL_TIMEOUT_US);

        rmutex_unlock(&dev->mutex);

        if(res > 0) {
            /* if one matches (equals 0), return 0 */
            res = strncmp(buf, "OK", 2) && strncmp(buf, "+CPIN: READY", 12);

            if(res > 0) {
                LOG_INFO(LOG_HEADER"unexpected response: %s\n", buf);
            }
        }
    }

    return res;
}

int gsm_set_pin(gsm_t *dev, const char *pin)
{
    return gsm_set_puk(dev, NULL, pin);
}


int gsm_check_pin(gsm_t *dev)
{
    int res;
    char linebuf[GSM_AT_LINEBUFFER_SIZE_SMALL];

    rmutex_lock(&dev->mutex);

    res = at_send_cmd_get_resp(&dev->at_dev, "AT+CPIN?", linebuf,
            GSM_AT_LINEBUFFER_SIZE_SMALL, GSM_SERIAL_TIMEOUT_US);

    rmutex_unlock(&dev->mutex);

    if (res > 0) {
        if (!strncmp(linebuf, "OK", res)) {
            res = 0;
        }
        else if (!strcmp(linebuf, "+CPIN: READY")) {
            /* sim is ready */
            res = 0;
        }
        else if (!strcmp(linebuf, "+CPIN: SIM PIN")) {
            /* sim needs pin */
            res = 1;
        }
        else {
            LOG_INFO(LOG_HEADER"unexpected response: %s\n", linebuf);
            res = -1;
        }
    }

    return res;
}

int gsm_check_operator(gsm_t *dev)
{
    char buf[GSM_AT_LINEBUFFER_SIZE];

    rmutex_lock(&dev->mutex);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+COPS?", buf, sizeof(buf),
            GSM_SERIAL_TIMEOUT_US);
    if ((res > 0) && (strncmp(buf, "+COPS:", 6) == 0)) {
        res = 0;
    }
    else {
        res = -1;
    }

    rmutex_unlock(&dev->mutex);

    return res;
}

size_t gsm_get_operator(gsm_t *dev, char *outbuf, size_t len)
{
    char buf[GSM_AT_LINEBUFFER_SIZE];
    char *pos = buf;

    rmutex_lock(&dev->mutex);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+COPS?", buf, sizeof(buf),
            GSM_SERIAL_TIMEOUT_US);

    rmutex_unlock(&dev->mutex);

    if(res > 0) {
        /* +COPS: 1,0,"NL KPN KPN",8 */
        if(strncmp(pos, "+COPS:", 6) == 0) {
            size_t outlen = 0;
            if(pos[7] == '0') {
                strcpy(outbuf, "Unknown");
                res = 8;
            }
            else {
                /* skip '+COPS: [0-3],[0-3],"' */
                pos = strchr(buf, '"');
                while((*(++pos) != '"') && (len--)) {
                    *outbuf++ = *pos;
                    outlen++;
                }
                if(len) {
                    *outbuf = '\0';
                    res = outlen;
                }
                else {
                    res = -ENOSPC;
                }
            }
        }
        else {
            res = -1;
            LOG_INFO(LOG_HEADER"unexpected response: %s\n", buf);
        }
    }

    return res;
}

ssize_t gsm_get_imei(gsm_t *dev, char *buf, size_t len)
{
    rmutex_lock(&dev->mutex);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+GSN", buf, len,
            GSM_SERIAL_TIMEOUT_US);
    if (res > 0) {
        buf[res] = '\0';
    }
    else {
        res = -1;
    }

    rmutex_unlock(&dev->mutex);

    return res;
}

ssize_t gsm_get_imsi(gsm_t *dev, char *buf, size_t len)
{
    rmutex_lock(&dev->mutex);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+CIMI", buf, len,
            GSM_SERIAL_TIMEOUT_US);

    if (res > 0) {
        if(strncmp(buf, "+CME ERROR:", 10) == 0) {
            res = -1;
            LOG_INFO(LOG_HEADER"unexpected response: %s\n", buf);
        }
    }
    else {
        res = -1;
    }

    rmutex_unlock(&dev->mutex);

    return res;
}

ssize_t gsm_get_simcard_identification(gsm_t *dev, char *outbuf, size_t len)
{
    int err = -EINVAL;

    if(dev) {
        char buf[GSM_AT_LINEBUFFER_SIZE];
        char *pos = buf;


        rmutex_lock(&dev->mutex);

        err = at_send_cmd_get_resp(&dev->at_dev, "AT+CCID", buf, len,
        GSM_SERIAL_TIMEOUT_US);

        rmutex_unlock(&dev->mutex);

        if(err <= 0) {
            goto out;
        }

        if(strncmp(buf, "+CCID:", 6) == 0) {
            pos = strchr(buf, '"');
            if(pos) {
                size_t outlen = 0;
                while((*(++pos) != '"') && (len--)) {
                    *outbuf++ = *pos;
                    outlen++;
                }
                if(len) {
                    *outbuf = '\0';
                    err = outlen;
                }
                else {
                    err = -ENOSPC;
                }
            }
        }
    }
out:
    return err;
}

ssize_t gsm_get_identification(gsm_t *dev, char *buf, size_t len)
{
    ssize_t res = -1;
    if(dev && buf){

        rmutex_lock(&dev->mutex);

        res = at_send_cmd_get_lines(&dev->at_dev, "ATI", buf, len, false,
                GSM_SERIAL_TIMEOUT_US);

        rmutex_unlock(&dev->mutex);

        if(res >= 0) {
            char * pos = &buf[res -1]; /* will provide the pointer to index of the terminator */

            /* OK is still in the buffer, so find final character */
            if(strncmp(pos - 2, "OK", 2) == 0) {
                pos -= 2;

                while(--pos != buf) {
                    if(isalnum(*pos)) {
                        break;
                    }
                }

                *(++pos) = '\0';
                res = pos - buf;
            }
        }
    }

    return res;
}

int gsm_get_signal(gsm_t *dev, unsigned *rssi, unsigned *ber)
{
    char buf[32];
    char *pos = buf;

    rmutex_lock(&dev->mutex);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+CSQ", buf, sizeof(buf),
            GSM_SERIAL_TIMEOUT_US);
    if ((res > 2) && strncmp(buf, "+CSQ: ", 6) == 0) {
        pos += 6; /* skip "+CSQ: " */
        *rssi = scn_u32_dec(pos, 2);
        pos += 2 + (*rssi > 9); /* skip rssi value ( n, or nn,) */
        *ber = scn_u32_dec(pos, 2);
        res = 0;
    }
    else {
        res = -1;
    }

    rmutex_unlock(&dev->mutex);

    return res;
}

int gsm_get_registration(gsm_t *dev)
{
    int res = -EINVAL;

    if(dev) {
        char buf[GSM_AT_LINEBUFFER_SIZE];

        rmutex_lock(&dev->mutex);

        res = at_send_cmd_get_resp(&dev->at_dev, "AT+CREG?", buf, sizeof(buf),
                GSM_SERIAL_TIMEOUT_US);

        rmutex_unlock(&dev->mutex);

        if((res > 0) && (strncmp(buf, "+CREG:", 6) == 0)) {
            char * pos = strchr(buf, ',');
            if(pos){
                res = atoi(++pos);
            }
            else {
                res = -1;
            }
        } else {
            res = -1;
        }
    }

    return res;
}

ssize_t __attribute__((weak)) gsm_get_local_time(gsm_t *dev, char * outbuf, size_t len)
{
    ssize_t res = -1;
    if(dev && outbuf) {
        char buf[GSM_AT_LINEBUFFER_SIZE];

        rmutex_lock(&dev->mutex);

        res = at_send_cmd_get_lines(&dev->at_dev, "AT+CCLK?", buf,
                GSM_AT_LINEBUFFER_SIZE, false, GSM_SERIAL_TIMEOUT_US);

        rmutex_unlock(&dev->mutex);

        if(res > 0) {
            if(strncmp(buf, "+CCLK:", 6) == 0) {
                char * pos = strchr(buf, '"');
                if(pos) {
                    size_t outlen = 0;
                    while((*(++pos) != '"') && (len--)) {
                        *outbuf++ = *pos;
                        outlen++;
                    }
                    if(len) {
                        *outbuf = '\0';
                        res = outlen;
                    }
                    else {
                        res = -ENOSPC;
                    }
                }
                else {
                    res = -1;
                }
            }
        }
    }

    return res;
}


ssize_t gsm_cmd(gsm_t *dev, const char *cmd, uint8_t *buf, size_t len, unsigned timeout)
{
    rmutex_lock(&dev->mutex);

    ssize_t res = at_send_cmd_get_lines(&dev->at_dev, cmd, (char *)buf, len,
            true, GSM_SERIAL_TIMEOUT_US * timeout);

    rmutex_unlock(&dev->mutex);

    return res;
}


void gsm_print_status(gsm_t *dev)
{
    char buf[GSM_AT_LINEBUFFER_SIZE];

    int res = gsm_get_identification(dev, buf, GSM_AT_LINEBUFFER_SIZE);

    if(res >= 2) {
        printf(LOG_HEADER"device type %s\n", buf);
    } else {
        printf("gsm: error reading device type!\n");
    }

    res = gsm_get_imei(dev, buf, GSM_AT_LINEBUFFER_SIZE);
    if(res >= 0) {
        printf(LOG_HEADER"IMEI: \"%s\"\n", buf);
    } else {
        printf(LOG_HEADER"error getting IMEI\n");
    }

    res = gsm_get_simcard_identification(dev, buf, GSM_AT_LINEBUFFER_SIZE);
    if(res >= 0) {
        printf(LOG_HEADER"ICCID: \"%s\"\n", buf);
    } else {
        printf(LOG_HEADER"error getting ICCID\n");
    }

    res = gsm_get_imsi(dev, buf, GSM_AT_LINEBUFFER_SIZE);
    if(res >= 0) {
        printf(LOG_HEADER"IMSI: \"%s\"\n", buf);
    } else {
        printf(LOG_HEADER"error getting IMSI\n");
    }

    res = gsm_get_registration(dev);
    if(res >= 0) {
        printf(LOG_HEADER"registration code: %d\n", res);

        res = gsm_get_operator(dev, buf, GSM_AT_LINEBUFFER_SIZE);
        if(res >= 0) {
            printf(LOG_HEADER"registered to \"%s\"\n", buf);
        } else {
            printf(LOG_HEADER"no operator\n");
        }
    } else {
        printf(LOG_HEADER"not registered\n");
    }

    unsigned rssi;
    unsigned ber;
    res = gsm_get_signal(dev, &rssi, &ber);
    if(res == 0) {
        printf(LOG_HEADER"RSSI=%u ber=%u%%\n", rssi, ber);
    } else {
        printf(LOG_HEADER"error getting signal strength\n");
    }
}

void gsm_register_urc_callback(gsm_t *dev, const char * urc,
                                    at_oob_cb_t cb, void * args)
{
    if(dev) {
        at_oob_t oob = {
            .urc = urc,
            .cb = cb,
            .arg = args
        };

        at_add_oob(&dev->at_dev, &oob);
    }
}

static void * idle_thread(void *arg)
{
    unsigned i;
    gsm_t *dev = arg;
    at_oob_t oob[] = {
        {
            .urc = "+CREG",
            .cb = creg_cb,
            .arg = dev,
        },
    };

    assert(dev);

    /* register all unsolicited result codes */
    for(i = 0; i < sizeof(oob) / sizeof(oob[0]); i++) {
        at_add_oob(&dev->at_dev, &oob[i]);
    }

    while(1) {
        if(dev->state >= GSM_ON) {
            rmutex_lock(&dev->mutex);
            at_process_oob(&dev->at_dev);
            rmutex_unlock(&dev->mutex);
            xtimer_sleep(1);
        }
        else {
            thread_sleep();
        }
    }

    return NULL;
}

static void creg_cb(void *arg, const char *buf)
{
    (void)arg;
    LOG_DEBUG(LOG_HEADER"CREG received: %s\n", buf);
}

