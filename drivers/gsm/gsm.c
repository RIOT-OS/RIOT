/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2018 OTA keys S.A.
 * Copyright (C) 2018 Max van Kessel
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
#include "fmt.h"
#include "xtimer.h"

#include "periph/gpio.h"

#include "gsm.h"

#include "include/gsm_internal.h"

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

#define MODULE  "gsm: "

#ifdef MODULE_AT_URC
#define GSM_HAS_THREAD
#endif

#ifdef MODULE_AT_URC
static void * idle_thread(void *arg);
#endif

#ifdef MODULE_AT_URC
static void creg_cb(void *arg, const char *buf);
#endif

#ifdef MODULE_PERIPH_GPIO_IRQ
static void ring_cb(void *arg);
#endif

int gsm_init(gsm_t *dev, const gsm_params_t *params, const gsm_driver_t *driver)
{
    int err = -EINVAL;

    if(dev){
        /* initialize and lock device */
        rmutex_init(&dev->mutex);
        rmutex_lock(&dev->mutex);

        /* store provided parameters in device */
        dev->params = params;
        dev->driver = driver;

        /* initialize AT device */
        err = at_dev_init(&dev->at_dev, params->uart, params->baudrate,
                            dev->buffer, GSM_UART_BUFSIZE);

        if(err) {
            LOG_WARNING(MODULE"failed to initialize at parser with %d\n", err);
            goto out;
        }

        if((dev->driver) && (dev->driver->init_base)) {
            err = dev->driver->init_base(dev);

            if(err) {
                LOG_WARNING(MODULE"failed to initialize base with %d\n", err);
                goto out;
            }
        }

#ifdef MODULE_PERIPH_GPIO_IRQ
        if(dev->params->ri_pin != GPIO_UNDEF) {
            gpio_init_int(dev->params->ri_pin, GPIO_IN, GPIO_FALLING, ring_cb, dev);
        }
#endif

#ifdef GSM_HAS_THREAD
        dev->pid = thread_create(dev->stack, GSM_THREAD_STACKSIZE, GSM_THREAD_PRIO,
                THREAD_CREATE_STACKTEST, idle_thread, dev, "gsm");
#endif

out:
        gsm_unlock(dev);
    }

    return err;
}

void gsm_lock(gsm_t *dev)
{
    rmutex_lock(&dev->mutex);
}

void gsm_unlock(gsm_t *dev)
{
    rmutex_unlock(&dev->mutex);
}

int gsm_poweron(gsm_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        gsm_lock(dev);

        dev->state = GSM_BOOT;

        if ((dev->driver) && (dev->driver->power_on)) {
            err = dev->driver->power_on(dev);

            if (err < 0) {
                LOG_WARNING(MODULE"failed to power on\n");

                dev->state = GSM_OFF;
            }
            else {
                dev->state = GSM_ON;
            }
        }

        if (dev->state == GSM_ON) {
            at_dev_poweron(&dev->at_dev);

            /* enable network registration unsolicited result code */
            err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CREG="GSM_URC_CREG, GSM_SERIAL_TIMEOUT_US);
            if (err < 0) {
                LOG_INFO(MODULE"failed to enable unsolicited result for CREG\n");
            }

            /* set error result codes to numeric values */
            err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CMEE=1", GSM_SERIAL_TIMEOUT_US);
            if (err < 0) {
                LOG_INFO(MODULE"failed to enable extended error notification CMEE\n");
            }
        }

        gsm_unlock(dev);
    }

    return err;
}

void gsm_poweroff(gsm_t *dev)
{
    if (dev) {
        int err = -EINVAL;
        gsm_lock(dev);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CFUN=0", GSM_SERIAL_TIMEOUT_US * 3);

        if ((dev->driver) && (dev->driver->power_off)) {
            err = dev->driver->power_off(dev);

            if (err < 0) {
                LOG_WARNING(MODULE"failed to power off\n");
            }
            else {
                at_dev_poweroff(&dev->at_dev);

                dev->state = GSM_OFF;
            }
        }
        gsm_unlock(dev);
    }
}

int gsm_wake_up(gsm_t *dev)
{
    int err = -EINVAL;
    if (dev) {

        gsm_lock(dev);

        if ((dev->driver) && (dev->driver->wake_up)) {
            err = dev->driver->wake_up(dev);

            if (err < 0) {
                LOG_WARNING(MODULE"failed to wake\n");
            }
            else {
                dev->state = GSM_UNDEFINED;
                err = -1;
            }
        }
        gsm_unlock(dev);
    }

    return err;
}

void gsm_sleep(gsm_t *dev)
{
    if (dev) {
        gsm_lock(dev);

        if ((dev->driver) && (dev->driver->sleep)) {
            dev->driver->sleep(dev);

            dev->state = GSM_SLEEP;
        }
        gsm_unlock(dev);
    }
}

int gsm_enable_radio(gsm_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        gsm_lock(dev);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CFUN=1",
        GSM_SERIAL_TIMEOUT_US * 30);

        gsm_unlock(dev);
    }

    return err;
}

int gsm_disable_radio(gsm_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        gsm_lock(dev);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CFUN=0",
                                    GSM_SERIAL_TIMEOUT_US * 30);

        gsm_unlock(dev);
    }

    return err;
}

bool __attribute__((weak)) gsm_is_alive(gsm_t *dev, uint8_t retries)
{
    bool alive = false;
    if (dev) {
        int err;
        /* send AT to see if it's alive */
        do {
            err = at_send_cmd_wait_ok(&dev->at_dev, "AT",
                                        GSM_SERIAL_TIMEOUT_US);

            xtimer_usleep(GSM_SERIAL_TIMEOUT_US);
        } while (err != 0 && (retries--));

        if (err == 0) {
            alive = true;
        }
    }
    return alive;
}

int gsm_set_puk(gsm_t *dev, const char *puk, const char *pin)
{
    int res = -EINVAL;

    if ((strlen(pin) == 4)) {
        char buf[GSM_AT_LINEBUFFER_SIZE_SMALL];
        char *pos = buf;
        bool quotes = false;

        pos += fmt_str(pos, "AT+CPIN=");
        if (strlen(puk) == 8) {
            pos += fmt_str(pos, "\"");
            pos += fmt_str(pos, puk);
            pos += fmt_str(pos, "\",\"");

            quotes = true;
        }
        pos += fmt_str(pos, pin);
        if (quotes) {
            pos += fmt_str(pos, "\"");
        }
        *pos = '\0';

        gsm_lock(dev);

        res = at_send_cmd_get_resp(&dev->at_dev, buf, buf,
                        GSM_AT_LINEBUFFER_SIZE_SMALL, GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);

        if (res > 0) {
            /* if one matches (equals 0), return 0 */
            res = strncmp(buf, "OK", 2) && strncmp(buf, "+CPIN: READY", 12);

            if (res > 0) {
                LOG_INFO(MODULE"unexpected response: %s\n", buf);
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

    gsm_lock(dev);

    res = at_send_cmd_get_resp(&dev->at_dev, "AT+CPIN?", linebuf,
            GSM_AT_LINEBUFFER_SIZE_SMALL, GSM_SERIAL_TIMEOUT_US);

    gsm_unlock(dev);

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
            LOG_INFO(MODULE"unexpected response: %s\n", linebuf);
            res = -1;
        }
    }

    return res;
}

int gsm_check_operator(gsm_t *dev)
{
    char buf[GSM_AT_LINEBUFFER_SIZE];

    gsm_lock(dev);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+COPS?", buf, sizeof(buf),
                                    GSM_SERIAL_TIMEOUT_US);
    if ((res > 0) && (strncmp(buf, "+COPS:", 6) == 0)) {
        res = 0;
    }
    else {
        res = -1;
    }

    gsm_unlock(dev);

    return res;
}

size_t gsm_get_operator(gsm_t *dev, char *outbuf, size_t len, uint8_t * tech)
{
    char buf[GSM_AT_LINEBUFFER_SIZE];
    char *pos = buf;

    gsm_lock(dev);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+COPS?", buf, sizeof(buf),
                                    GSM_SERIAL_TIMEOUT_US);

    gsm_unlock(dev);

    if (res > 0) {
        /* +COPS: 1,0,"NL KPN KPN",8 */
        if (strncmp(pos, "+COPS:", 6) == 0) {
            size_t outlen = 0;
            if (res < 10) {
                /* not registered yet */
                res = -1;
            }
            else {
                /* skip '+COPS: [0-3],[0-3],"' */
                pos = strchr(buf, '"');
                while ((*(++pos) != '"') && (len--)) {
                    *outbuf++ = *pos;
                    outlen++;
                }
                if (len) {
                    *outbuf = '\0';
                    res = outlen;
                }
                else {
                    res = -ENOSPC;
                }

                if (tech) {
                    if (*(++pos) == ',') {
                        *tech = (uint8_t)scn_u32_dec(++pos, 1);
                    }
                }
            }
        }
        else {
            res = -1;
            LOG_INFO(MODULE"unexpected response: %s\n", buf);
        }
    }

    return res;
}

ssize_t gsm_get_imei(gsm_t *dev, char *buf, size_t len)
{
    gsm_lock(dev);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+GSN", buf, len,
                                    GSM_SERIAL_TIMEOUT_US);
    if (res > 0) {
        buf[res] = '\0';
    }
    else {
        res = -1;
    }

    gsm_unlock(dev);

    return res;
}

ssize_t gsm_get_imsi(gsm_t *dev, char *buf, size_t len)
{
    gsm_lock(dev);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+CIMI", buf, len,
            GSM_SERIAL_TIMEOUT_US);

    if (res > 0) {
        if ((strncmp(buf, "+CME ERROR:", 10) == 0)
                        || (strncmp(buf, "ERROR:", 6) == 0)) {
            res = -1;
            LOG_INFO(MODULE"unexpected response: %s\n", buf);
        }
    }
    else {
        res = -1;
    }

    gsm_unlock(dev);

    return res;
}

ssize_t gsm_get_simcard_identification(gsm_t *dev, char *outbuf, size_t len)
{
    int err = -EINVAL;

    if(dev) {
        char buf[GSM_AT_LINEBUFFER_SIZE];
        char *pos = buf;


        gsm_lock(dev);

        err = at_send_cmd_get_resp(&dev->at_dev, "AT+CCID", buf, len,
                                    GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);

        if (err <= 0) {
            if (err == 0) {
                err = -1;
            }
            goto out;
        }

        if (strncmp(buf, "+CCID:", 6) == 0) {
            pos = strchr(buf, ':');
            if (pos) {
                size_t outlen = 0;
                while ((!isalnum((uint8_t )(*(++pos))))
                                && ((buf + GSM_AT_LINEBUFFER_SIZE) > pos)) {
                }

                while ((isalnum((uint8_t )*pos)) && (len--)) {
                    *outbuf++ = *pos++;
                    outlen++;
                }
                if (len) {
                    *outbuf = '\0';
                    err = outlen;
                }
                else {
                    err = -ENOSPC;
                }
            }
        }
        else {
            err = -1;
        }
    }
out:
    return err;
}

ssize_t gsm_get_identification(gsm_t *dev, char *buf, size_t len)
{
    ssize_t res = -1;
    if(dev && buf){

        gsm_lock(dev);

        res = at_send_cmd_get_lines(&dev->at_dev, "ATI", buf, len, false,
                GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);

        if (res >= 0) {
            char * pos = &buf[res -1]; /* will provide the pointer to index of the terminator */

            /* OK is still in the buffer, so find final character */
            if (strncmp(pos - 2, "OK", 2) == 0) {
                pos -= 2;

                while (--pos != buf) {
                    if (isalnum((uint8_t)*pos)) {
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

int __attribute__((weak)) gsm_signal_to_rssi(unsigned rssi)
{
    return (int)rssi;
}

int gsm_get_signal(gsm_t *dev, int *rssi, unsigned *ber)
{
    char buf[32];
    char *pos = buf;

    gsm_lock(dev);

    int res = at_send_cmd_get_resp(&dev->at_dev, "AT+CSQ", buf, sizeof(buf),
            GSM_SERIAL_TIMEOUT_US);
    if ((res > 2) && strncmp(buf, "+CSQ: ", 6) == 0) {
        pos += 6; /* skip "+CSQ: " */
        *rssi = gsm_signal_to_rssi(scn_u32_dec(pos, 2));
        pos += 2 + (*rssi > 9); /* skip rssi value ( n, or nn,) */
        *ber = scn_u32_dec(pos, 2);
        res = 0;
    }
    else {
        res = -1;
    }

    gsm_unlock(dev);

    return res;
}

int gsm_get_registration(gsm_t *dev, uint16_t * location_id, uint32_t * cell_id)
{
    int res = -EINVAL;

    if(dev) {
        char buf[GSM_AT_LINEBUFFER_SIZE];

        gsm_lock(dev);

        res = at_send_cmd_get_resp(&dev->at_dev, "AT+CREG?", buf, sizeof(buf),
                                    GSM_SERIAL_TIMEOUT_US);
        gsm_unlock(dev);

        if ((res > 0) && (strncmp(buf, "+CREG:", 6) == 0)) {
            char * pos = strtok(buf, ",");
            if (pos) {
                int registration;
                /* skip first token */
                pos = strtok(NULL, ",");

                registration = atoi(pos);

                pos = strtok(NULL, ",");

                /* creg extension: +CREG: 2,1,"008E","CE87" */
                if ((pos != NULL) && (location_id)) {
                    char loc[5] = { 0 };
                    char * loc_ptr = &loc[0];
                    unsigned int len = res - (++pos - buf);
                    while ((*(pos) != '"') && (len--)) {
                        *loc_ptr++ = *pos++;
                    }

                    *location_id = strtol(loc, NULL, 16);
                }

                pos = strtok(NULL, ",");

                if ((pos != NULL) && (cell_id)) {
                    char cell[9] = { 0 };
                    char * cell_ptr = &cell[0];
                    unsigned int len = res - (++pos - buf);
                    while ((*(pos) != '"') && (len--)) {
                        *cell_ptr++ = *pos++;
                    }

                    *cell_id = strtol(cell, NULL, 16);
                }

                res = registration;
            }
            else {
                res = -1;
            }
        }
        else {
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

        gsm_lock(dev);

        res = at_send_cmd_get_lines(&dev->at_dev, "AT+CCLK?", buf,
                GSM_AT_LINEBUFFER_SIZE, false, GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);

        if (res > 0) {
            if (strncmp(buf, "+CCLK:", 6) == 0) {
                char * pos = strchr(buf, '"');
                if (pos) {
                    size_t outlen = 0;
                    while ((*(++pos) != '"') && (len--)) {
                        *outbuf++ = *pos;
                        outlen++;
                    }
                    if (len) {
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
    gsm_lock(dev);

    ssize_t res = at_send_cmd_get_lines(&dev->at_dev, cmd, (char *)buf, len,
                                        true, (uint32_t)US_PER_SEC * timeout);

    gsm_unlock(dev);

    return res;
}


void gsm_print_status(gsm_t *dev)
{
    char buf[GSM_AT_LINEBUFFER_SIZE];
    uint16_t loc;
    uint32_t cell;

    int res = gsm_get_identification(dev, buf, GSM_AT_LINEBUFFER_SIZE);

    if (res >= 2) {
        printf("device type %s\n", buf);
    }
    else {
        printf("error reading device type!\n");
    }

    res = gsm_get_imei(dev, buf, GSM_AT_LINEBUFFER_SIZE);
    if (res >= 0) {
        printf("IMEI: \"%s\"\n", buf);
    }
    else {
        printf("error getting IMEI\n");
    }

    res = gsm_get_simcard_identification(dev, buf, GSM_AT_LINEBUFFER_SIZE);
    if (res >= 0) {
        printf("ICCID: \"%s\"\n", buf);
    }
    else {
        printf("error getting ICCID\n");
    }

    res = gsm_get_imsi(dev, buf, GSM_AT_LINEBUFFER_SIZE);
    if (res >= 0) {
        printf("IMSI: \"%s\"\n", buf);
    }
    else {
        printf("error getting IMSI\n");
    }

    res = gsm_get_registration(dev, &loc, &cell);
    if (res > 0) {
        uint8_t tech = -1;
        printf("registration code: %d\n", res);
        printf("location id: %X\n", loc);
        printf("cell id: %X\n", cell);

        res = gsm_get_operator(dev, buf, GSM_AT_LINEBUFFER_SIZE, &tech);
        if (res >= 0) {
            if ((char)tech != -1) {
                printf("registered to \"%s\" (%u)\n", buf, tech);
            }
            else {
                printf("registered to \"%s\"\n", buf);
            }
        }
        else {
            printf("no operator\n");
        }
    }
    else {
        printf("not registered\n");
    }

    int rssi;
    unsigned ber;
    res = gsm_get_signal(dev, &rssi, &ber);
    if (res == 0) {
        printf(MODULE"RSSI= %ddBm ber=%u%%\n", rssi, ber);
    }
    else {
        printf(MODULE"error getting signal strength\n");
    }
}

#ifdef MODULE_AT_URC
void gsm_register_urc_callback(gsm_t *dev, at_urc_t *urc)
{
    if(dev) {
        gsm_lock(dev);

        at_add_urc(&dev->at_dev, urc);

        gsm_unlock(dev);
    }
}

#endif

#ifdef GSM_HAS_THREAD
static void * idle_thread(void *arg)
{
    gsm_t *dev = arg;
    assert(dev);

#ifdef MODULE_AT_URC
    unsigned i;
    at_urc_t urc[] = {
        {
            .code = "+CREG",
            .cb = creg_cb,
            .arg = dev,
        },
    };

    gsm_lock(dev);

    /* register all unsolicited result codes */
    for (i = 0; i < sizeof(urc) / sizeof(urc[0]); i++) {
        at_add_urc(&dev->at_dev, &urc[i]);
    }

    gsm_unlock(dev);
#endif

    while(1) {
        if (dev->state >= GSM_ON) {
#ifdef MODULE_AT_URC
            gsm_lock(dev);
            at_process_urc(&dev->at_dev, 1000 * US_PER_MS);
            gsm_unlock(dev);
#endif
        }
        thread_sleep();
    }

    return NULL;
}
#endif

#ifdef MODULE_AT_URC
static void creg_cb(void *arg, const char *buf)
{
    (void)arg;
    LOG_DEBUG(MODULE"CREG received: %s\n", buf);
}
#endif

#ifdef MODULE_PERIPH_GPIO_IRQ
static void ring_cb(void *arg)
{
#ifdef GSM_HAS_THREAD
    if (arg) {
        thread_wakeup(((gsm_t *)arg)->pid);
    }
#else
    (void)arg;
#endif
    LOG_DEBUG(MODULE"ring!");
}
#endif
