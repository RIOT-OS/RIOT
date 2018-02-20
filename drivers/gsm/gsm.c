/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "thread.h"
#include "fmt.h"
#include "log.h"
#include "gsm.h"
#include "xtimer.h"
#include "net/ipv4/addr.h"

#define MODEM_INIT_MAXTRIES (3)

static void *_gsm_idle_thread(void *arg);

int gsm_init(gsm_t *gsmdev, const gsm_params_t *params)
{
    unsigned tries = MODEM_INIT_MAXTRIES;
    int res;

    assert(gsmdev && gsmdev->driver);
    assert(params);
    LOG_INFO("gsm: initializing...\n");

    rmutex_init(&gsmdev->mutex);
    rmutex_lock(&gsmdev->mutex);

    at_dev_t *at_dev = &gsmdev->at_dev;
    gsmdev->params = *params;
    /* initialize driver struct */
    if (gsmdev->driver->init_drv) {
        res = gsmdev->driver->init_drv(gsmdev, params);
        if (res) {
            goto out;
        }
    }

    /* initialize AT device */
    res = at_dev_init(at_dev, params->uart, params->baudrate, gsmdev->buf, sizeof(gsmdev->buf));
    if (res) {
        goto out;
    }

    /* reset device */
    res = gsmdev->driver->reset(gsmdev);
    if (res) {
        goto out;
    }

    /* wait for boot */
    do {
        xtimer_usleep(100000U);
        res = at_send_cmd_wait_ok(at_dev, "AT", GSM_SERIAL_TIMEOUT);
    } while (res != 0 && (tries--));
    if (res) {
        goto out;
    }

    /* Change baudrate if requested */
    if (gsmdev->params.baudrate_to_set && (gsmdev->driver->change_baudrate)) {
        res = gsmdev->driver->change_baudrate(gsmdev, gsmdev->params.baudrate_to_set);
        if (res) {
            goto out;
        }
        at_dev_poweroff(at_dev);
        res = at_dev_init(at_dev, params->uart, params->baudrate_to_set, gsmdev->buf,
                          sizeof(gsmdev->buf));
        if (res) {
            goto out;
        }
        xtimer_usleep(100000U);
        res = at_send_cmd_wait_ok(at_dev, "AT", GSM_SERIAL_TIMEOUT);
        if (res) {
            goto out;
        }
    }

    /* enable rf */
    res = at_send_cmd_wait_ok(at_dev, "AT+CFUN=1", GSM_SERIAL_TIMEOUT*30);
    if (res) {
        goto out;
    }

    /* device specific init */
    res = gsmdev->driver->init(gsmdev);
    if (res) {
        goto out;
    }

    gsmdev->state = GSM_ON;
    /* create OOB thread */
    gsmdev->pid = thread_create(gsmdev->stack, sizeof(gsmdev->stack), GSM_THREAD_PRIO,
                                THREAD_CREATE_STACKTEST, _gsm_idle_thread, gsmdev, "gsm");

out:
    rmutex_unlock(&gsmdev->mutex);
    return res;
}

int gsm_receive_sms(gsm_t *gsmdev, gsm_sms_cb_t cb, void *arg)
{
    assert(gsmdev);

    rmutex_lock(&gsmdev->mutex);

    gsmdev->sms_cb = cb;
    gsmdev->sms_arg = arg;

    /* Enable reception */
    int res = at_send_cmd_wait_ok(&gsmdev->at_dev, "AT+CNMI=1,1", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }
    /* set text mode */
    res = at_send_cmd_wait_ok(&gsmdev->at_dev, "AT+CMGF=1", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }
    /* set header info */
    res = at_send_cmd_wait_ok(&gsmdev->at_dev, "AT+CSDH=1", GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }
    /* set default storage */
    char buf[32];
    res = at_send_cmd_get_lines(&gsmdev->at_dev, "AT+CPMS=\"ME\",\"ME\",\"ME\"",
                                buf, sizeof(buf), false, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        goto out;
    }

    /* delete all messages */
    res = at_send_cmd_wait_ok(&gsmdev->at_dev, "AT+CMGD=0,4", GSM_SERIAL_TIMEOUT);

out:
    rmutex_unlock(&gsmdev->mutex);
    return res;
}

static void _wkup_cb(void *arg)
{
    gsm_t *gsmdev = arg;

    at_dev_poweron(&gsmdev->at_dev);

    gsmdev->driver->wake_up(gsmdev);

    gsmdev->state = GSM_ON;
    thread_wakeup(gsmdev->pid);

    if (gsmdev->wkup_cb) {
        gsmdev->wkup_cb(gsmdev->wkup_arg);
    }
}

void gsm_sleep(gsm_t *gsmdev, gsm_wkup_cb_t cb, void *arg)
{
    rmutex_lock(&gsmdev->mutex);

    gsmdev->wkup_cb = cb;
    gsmdev->wkup_arg = arg;

    if (gsmdev->driver->sleep(gsmdev)) {
        rmutex_unlock(&gsmdev->mutex);
        return;
    }

    at_dev_poweroff(&gsmdev->at_dev);
    gsmdev->state = GSM_OFF;

    if (gsmdev->params.ri_pin != GPIO_UNDEF) {
        gpio_init_int(gsmdev->params.ri_pin, GPIO_IN, GPIO_FALLING, _wkup_cb, gsmdev);
    }

    rmutex_unlock(&gsmdev->mutex);
}


#define GSM_SMS_FIELD_NUMOF     (11)
#define GSM_SMS_FIELD_SENDER_ID (1)
#define GSM_SMS_FIELD_DATE_ID   (3)
#define GSM_SMS_FIELD_LEN_ID    (GSM_SMS_FIELD_NUMOF - 1)

static void _sms_cb(void *arg, const char *buf)
{
    gsm_t *gsmdev = arg;
    char resp_buf[64];
    char sms_buf[256];
    char req_buf[32];
    char *sms = NULL;
    char *sender = NULL;
    char *date = NULL;
    unsigned long len = 0;
    unsigned i = 0;

    LOG_DEBUG("SMS received\n");

    rmutex_lock(&gsmdev->mutex);

    if (strncmp(buf, "+CMTI: \"ME\",", 12) != 0) {
        rmutex_unlock(&gsmdev->mutex);
        return;
    }
    unsigned long index = strtoul(buf + 12, NULL, 10);
    char *pos = req_buf;
    pos += fmt_str(pos, "AT+CMGR=");
    pos += fmt_u32_dec(pos, index);
    *pos = '\0';
    int res = at_send_cmd_get_lines(&gsmdev->at_dev, req_buf, sms_buf, sizeof(sms_buf),
                                    false, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        rmutex_unlock(&gsmdev->mutex);
        return;
    }

    char *cur = sms_buf;
    do {
        if (i == GSM_SMS_FIELD_SENDER_ID) {
            sender = cur + 1; /* remove leading '"' */
        }
        if (i == GSM_SMS_FIELD_DATE_ID) {
            date = cur + 1; /* remove leading '"' */
            if (*cur == '\"') {
                while (*cur != ',') {
                    cur++;
                }
                cur++;
            }
        }
        if (i == GSM_SMS_FIELD_LEN_ID) {
            len = strtoul(cur, NULL, 0);
        }
        while (*cur != ',' && *cur != '\n') {
            cur++;
        }
        *cur = '\0';
        cur++;
        i++;
    } while (i < GSM_SMS_FIELD_NUMOF);

    if (sender) {
        sender[strlen(sender) - 1] = '\0'; /* remove trailing '"' */
    }
    if (date) {
        date[strlen(date) - 1] = '\0'; /* remove trailing '"' */
    }

    pos = req_buf;
    pos += fmt_str(pos, "AT+CMGD=");
    pos += fmt_u32_dec(pos, index);
    *pos = '\0';
    res = at_send_cmd_get_lines(&gsmdev->at_dev, req_buf, resp_buf, sizeof(resp_buf),
                                true, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        rmutex_unlock(&gsmdev->mutex);
        return;
    }

    if (len) {
        while ((*cur == '\r' || *cur == '\n') && (*cur != '\0')) {
            cur++;
        }
        sms = cur;
        sms[len] = '\0';
    }

    rmutex_unlock(&gsmdev->mutex);

    if (gsmdev->sms_cb) {
        gsmdev->sms_cb(gsmdev->sms_arg, sms, sender, date);
    }
}

static void _creg_cb(void *arg, const char *buf)
{
    (void)arg;
    LOG_DEBUG("CREG received: %s\n", buf);
}

static void *_gsm_idle_thread(void *arg)
{
    gsm_t *gsmdev = arg;

    at_oob_t oob[] = {
        {
            .urc = "+CMTI",
            .cb = _sms_cb,
            .arg = gsmdev,
        },
        {
            .urc = "+CREG",
            .cb = _creg_cb,
            .arg = gsmdev,
        },
    };

    for (unsigned i = 0; i < sizeof(oob) / sizeof(oob[0]); i++) {
        at_add_oob(&gsmdev->at_dev, &oob[i]);
    }

    while (1) {
        if (gsmdev->state == GSM_ON) {
            rmutex_lock(&gsmdev->mutex);
            at_process_oob(&gsmdev->at_dev);
            rmutex_unlock(&gsmdev->mutex);
            xtimer_sleep(1);
        }
        else {
            thread_sleep();
        }
    }

    return NULL;
}

int gsm_off(gsm_t *gsmdev)
{
    LOG_INFO("gsm: turning off.\n");
    at_dev_t *at_dev = &gsmdev->at_dev;
    return at_send_cmd_wait_ok(at_dev, "AT+CFUN=0", GSM_SERIAL_TIMEOUT*30);
}

int gsm_set_pin(gsm_t *gsmdev, const char *pin)
{
    if (strlen(pin) != 4) {
        return -EINVAL;
    }

    rmutex_lock(&gsmdev->mutex);

    xtimer_usleep(100000U);

    char buf[32];
    char *pos = buf;
    pos += fmt_str(pos, "AT+CPIN=");
    pos += fmt_str(pos, pin);
    *pos = '\0';

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, buf, buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    rmutex_unlock(&gsmdev->mutex);

    if (res > 0) {
        /* if one matches (equals 0), return 0 */
        return strncmp(buf, "OK", 2) && strncmp(buf, "+CPIN: READY", 12);
    }
    else {
        return -1;
    }
}

int gsm_check_pin(gsm_t *gsmdev)
{
    char linebuf[32];

    rmutex_lock(&gsmdev->mutex);
    xtimer_usleep(100000U);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CPIN?", linebuf, sizeof(linebuf), GSM_SERIAL_TIMEOUT);

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
            res = -1;
        }
    }

    rmutex_unlock(&gsmdev->mutex);
    return res;
}

//int gsm_gprs_init(gsm_t *gsmdev, const char *apn)
//{
//    char buf[64];
//    int res;
//    at_dev_t *at_dev = &gsmdev->at_dev;
//
//    mutex_lock(&gsmdev->mutex);
//
//    /* close possible open GPRS connection */
//    at_send_cmd_wait_ok(at_dev, "AT+SAPBR=0,1", GSM_SERIAL_TIMEOUT);
//
//    /* set GPRS */
//    res = at_send_cmd_wait_ok(at_dev, "AT+SAPBR=3,1,\"Contype\",\"GPRS\"", GSM_SERIAL_TIMEOUT);
//    if (res) {
//        goto out;
//    }
//
//    /* set APN */
//    char *pos = buf;
//    pos += fmt_str(pos, "AT+SAPBR=3,1,\"APN\",\"");
//    pos += fmt_str(pos, apn);
//    pos += fmt_str(pos, "\"");
//    *pos = '\0';
//    res = at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
//    if (res) {
//        goto out;
//    }
//
//    xtimer_usleep(100000U);
//
//    /* AT+SAPBR=1,1 -> open GPRS */
//    res = at_send_cmd_wait_ok(at_dev, "AT+SAPBR=1,1", GSM_SERIAL_TIMEOUT * 5);
//
//out:
//    mutex_unlock(&gsmdev->mutex);
//
//    return res;
//}

int gsm_creg_get(gsm_t *gsmdev)
{
    char buf[64];

    rmutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CREG?", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 0) && (strncmp(buf, "+CREG: 0,", 9) == 0)) {
        res = atoi(buf + 9);
    }
    else {
        res = -1;
    }

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_reg_check(gsm_t *gsmdev)
{
    char buf[64];

    rmutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+COPS?", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 0) && (strncmp(buf, "+COPS: 0,", 9) == 0)) {
        res = 0;
    }
    else {
        res = -1;
    }

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

size_t gsm_reg_get(gsm_t *gsmdev, char *outbuf, size_t len)
{
    char buf[64];
    char *pos = buf;

    rmutex_lock(&gsmdev->mutex);

    size_t outlen = 0;
    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+COPS?", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 12) && (strncmp(pos, "+COPS: 0,", 9) == 0)) {
        /* skip '+COPS: 0,[01],"' */
        pos += 12;

        while (*pos != '"' && len--) {
            *outbuf++ = *pos++;
            outlen++;
        }
        if (len) {
            *outbuf = '\0';
            res = outlen;
        }
        else {
            return -ENOSPC;
        }
    }
    else {
        res = -1;
    }

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

ssize_t gsm_imei_get(gsm_t *gsmdev, char *buf, size_t len)
{
    rmutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+GSN", buf, len, GSM_SERIAL_TIMEOUT);
    if (res > 0) {
        buf[res] = '\0';
    }
    else {
        res = -1;
    }

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_signal_get(gsm_t *gsmdev, unsigned *rssi, unsigned *ber)
{
    char buf[32];
    char *pos = buf;

    rmutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CSQ", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
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

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

//uint32_t gsm_gprs_getip(gsm_t *gsmdev)
//{
//    char buf[40];
//    char *pos = buf;
//    uint32_t ip = 0;
//
//    mutex_lock(&gsmdev->mutex);
//
//    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+SAPBR=2,1", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
//    if ((res > 13) && strncmp(buf, "+SAPBR: 1,1,\"", 13) == 0) {
//        res -= 1;   /* cut off " */
//        buf[res] = '\0';
//        pos += 13; /* skip +SAPBR: 1,1," */
//
//        ipv4_addr_from_str((ipv4_addr_t *)&ip, pos);
//    }
//
//    mutex_unlock(&gsmdev->mutex);
//
//    return ip;
//}

uint32_t gsm_gprs_getip(gsm_t *gsmdev)
{
    char buf[40];
    char *pos = buf;
    uint32_t ip = 0;

    rmutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CGPADDR=1", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);
    if ((res > 13) && strncmp(buf, "+CGPADDR: 1,\"", 13) == 0) {
        res -= 1;   /* cut off " */
        buf[res] = '\0';
        pos += 13; /* skip '+CGPADDR: 1,"' */

        ipv4_addr_from_str((ipv4_addr_t *)&ip, pos);
    }

    rmutex_unlock(&gsmdev->mutex);

    return ip;
}

void gsm_print_status(gsm_t *gsmdev)
{
    char buf[64];

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "ATI", buf, sizeof(buf), GSM_SERIAL_TIMEOUT);

    if (res >= 2) {
        buf[res] = '\0';
        printf("gsm: device type %s\n", buf);
    }
    else {
        printf("gsm: error reading device type!\n");
    }

    res = gsm_imei_get(gsmdev, buf, sizeof(buf));
    if (res >= 0) {
        printf("gsm: IMEI: \"%s\"\n", buf);
    }
    else {
        printf("gsm: error getting IMEI\n");
    }

    res = gsm_iccid_get(gsmdev, buf, sizeof(buf));
    if (res >= 0) {
        printf("gsm: ICCID: \"%s\"\n", buf);
    }
    else {
        printf("gsm: error getting ICCID\n");
    }


    res = gsm_reg_get(gsmdev, buf, sizeof(buf));
    if (res >= 0) {
        printf("gsm: registered to \"%s\"\n", buf);
    }
    else {
        printf("gsm: not registered\n");
    }

    unsigned rssi;
    unsigned ber;
    res = gsm_signal_get(gsmdev, &rssi, &ber);
    if (res == 0) {
        printf("gsm: RSSI=%u ber=%u%%\n", rssi, ber);
    }
    else {
        printf("gsm: error getting signal strength\n");
    }

    uint32_t ip = gsm_gprs_getip(gsmdev);
    if (ip) {
        printf("gsm: GPRS connected. IP=");
        for (unsigned i = 0; i < 4; i++) {
            uint8_t *_tmp = (uint8_t*) &ip;
            printf("%u%s", (unsigned)_tmp[i], (i < 3) ? "." : "\n");
        }
    }
    else {
        printf("gsm: error getting GPRS state\n");
    }
}

int gsm_iccid_get(gsm_t *gsmdev, char *buf, size_t len)
{
    rmutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CCID", buf, len, GSM_SERIAL_TIMEOUT);
    if (res > 0) {
        if (!strncmp(buf, "+CCID: ", 7)) {
            memcpy(buf, buf + 7, res - 7);
            buf[res - 7] = '\0';
        }
    }
    else {
        res = -1;
    }

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_gps_get_loc(gsm_t *gsmdev, uint8_t *buf, size_t len)
{
    rmutex_lock(&gsmdev->mutex);

    int res = at_send_cmd_get_resp(&gsmdev->at_dev, "AT+CGPSINF=0", (char *)buf, len, GSM_SERIAL_TIMEOUT);

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

ssize_t gsm_cmd(gsm_t *gsmdev, const char *cmd, uint8_t *buf, size_t len, unsigned timeout)
{
    rmutex_lock(&gsmdev->mutex);

    ssize_t res = at_send_cmd_get_lines(&gsmdev->at_dev, cmd, (char *)buf, len, true, GSM_SERIAL_TIMEOUT * timeout);

    rmutex_unlock(&gsmdev->mutex);

    return res;
}

int gsm_get_loc(gsm_t *gsmdev, char *lon, char *lat)
{
    return gsmdev->driver->get_loc(gsmdev, lon, lat);
}

int gsm_cnet_scan(gsm_t *gsmdev, char *buf, size_t len)
{
    return gsmdev->driver->cnet_scan(gsmdev, buf, len);
}

ssize_t gsm_http_get(gsm_t *gsmdev, const char *url, uint8_t *resultbuf, size_t len,
                     const gsm_http_params_t *params)
{
    return gsmdev->driver->http_get(gsmdev, url, resultbuf, len, params);
}

ssize_t gsm_http_get_file(gsm_t *gsmdev, const char *url, const char *filename,
                          const gsm_http_params_t *params)
{
    return gsmdev->driver->http_get_file(gsmdev, url, filename, params);
}

ssize_t gsm_http_post(gsm_t *gsmdev,
                      const char *url,
                      const uint8_t *data, size_t data_len,
                      uint8_t *resultbuf, size_t result_len,
                      const gsm_http_params_t *params)
{
    return gsmdev->driver->http_post(gsmdev, url, data, data_len, resultbuf, result_len, params);
}

ssize_t gsm_http_post_file(gsm_t *gsmdev, const char *url,
                           const char *filename,
                           uint8_t *resultbuf, size_t result_len,
                           const gsm_http_params_t *params)
{
    return gsmdev->driver->http_post_file(gsmdev, url, filename, resultbuf, result_len, params);
}

ssize_t gsm_http_post_file_2(gsm_t *gsmdev, const char *url,
                             const char *filename,
                             const char *result_filename,
                             const gsm_http_params_t *params)
{
    return gsmdev->driver->http_post_file_2(gsmdev, url, filename, result_filename, params);
}

int gsm_time_sync(gsm_t *gsmdev)
{
    return gsmdev->driver->time_sync(gsmdev);
}

int gsm_gps_start(gsm_t *gsmdev)
{
    if (gsmdev->driver->gps_start) {
        return gsmdev->driver->gps_start(gsmdev);
    }

    return -ENOTSUP;
}

int gsm_gps_stop(gsm_t *gsmdev)
{
    if (gsmdev->driver->gps_stop) {
        return gsmdev->driver->gps_stop(gsmdev);
    }

    return -ENOTSUP;
}

ssize_t gsm_get_nmea(gsm_t *gsmdev, char *nmea, size_t len)
{
    if (gsmdev->driver->get_nmea) {
        return gsmdev->driver->get_nmea(gsmdev, nmea, len);
    }

    return -ENOTSUP;
}
