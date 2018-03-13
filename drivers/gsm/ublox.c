
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "gsm/ublox.h"
#include "xtimer.h"
#include "fmt.h"
#include "log.h"

static int reset(gsm_t *dev)
{
    ublox_gsm_t *ublox = (ublox_gsm_t *)dev;

    if (ublox->params.pwr_on_pin != GPIO_UNDEF) {
        gpio_set(ublox->params.pwr_on_pin);
    }

    if (ublox->params.reset_pin != GPIO_UNDEF) {
        gpio_set(ublox->params.reset_pin);
        xtimer_usleep(51 * US_PER_MS);
        gpio_clear(ublox->params.reset_pin);
    }

    if (ublox->params.pwr_on_pin != GPIO_UNDEF) {
        xtimer_usleep(150 * US_PER_MS);
        gpio_clear(ublox->params.pwr_on_pin);
        xtimer_sleep(5);
    }

    return 0;
}

static int init_drv(gsm_t *dev, const gsm_params_t *params)
{
    ublox_gsm_t *ublox = (ublox_gsm_t *)dev;
    ublox_gsm_params_t *ublox_params = (ublox_gsm_params_t *)params;

    ublox->params = ublox_params->params;

    return 0;
}

static int init(gsm_t *dev)
{
    ublox_gsm_t *ublox = (ublox_gsm_t *)dev;
    at_dev_t *at_dev = &dev->at_dev;
    int res;
    char buf[128];
    char *pos;

    rmutex_lock(&dev->mutex);

    if (ublox->params.dtr_pin != GPIO_UNDEF) {
        gpio_clear(ublox->params.dtr_pin);
        xtimer_usleep(100);
        res = at_send_cmd_wait_ok(at_dev, "AT+UPSV=3", GSM_SERIAL_TIMEOUT);
    }
    else {
        res = at_send_cmd_wait_ok(at_dev, "AT+UPSV=1", GSM_SERIAL_TIMEOUT);
    }
    if (res) {
        rmutex_unlock(&dev->mutex);
        return res;
    }

    /* Error messages */
    at_send_cmd_wait_ok(at_dev, "AT+CMEE=1", GSM_SERIAL_TIMEOUT);
    /* Auto answer */
    at_send_cmd_wait_ok(at_dev, "ATS0=1", GSM_SERIAL_TIMEOUT);

    /* GPIO1 */
    if (ublox->params.gpio1_mode != UBLOX_GPIO_MODE_DEFAULT) {
        pos = buf;
        pos += fmt_str(pos, "AT+UGPIOC=");
        pos += fmt_u16_dec(pos, UBLOX_GPIO1);
        pos += fmt_str(pos, ",");
        pos += fmt_u16_dec(pos, ublox->params.gpio1_mode & UBLOX_GPIO_MODE_DISABLED);
        if (ublox->params.gpio1_mode == UBLOX_GPIO_MODE_OUTPUT) {
            pos += fmt_str(pos, ",");
            if (ublox->params.gpio1_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                pos += fmt_u16_dec(pos, 1);
            }
            else {
                pos += fmt_u16_dec(pos, 0);
            }
        }
        *pos = '\0';
        at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
    }
    /* GPIO2 */
    if (ublox->params.gpio2_mode != UBLOX_GPIO_MODE_DEFAULT) {
        pos = buf;
        pos += fmt_str(pos, "AT+UGPIOC=");
        pos += fmt_u16_dec(pos, UBLOX_GPIO2);
        pos += fmt_str(pos, ",");
        pos += fmt_u16_dec(pos, ublox->params.gpio1_mode & UBLOX_GPIO_MODE_DISABLED);
        if (ublox->params.gpio2_mode == UBLOX_GPIO_MODE_OUTPUT) {
            pos += fmt_str(pos, ",");
            if (ublox->params.gpio2_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                pos += fmt_u16_dec(pos, 1);
            }
            else {
                pos += fmt_u16_dec(pos, 0);
            }
        }
        *pos = '\0';
        at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
    }
    /* GPIO3 */
    if (ublox->params.gpio3_mode != UBLOX_GPIO_MODE_DEFAULT) {
        pos = buf;
        pos += fmt_str(pos, "AT+UGPIOC=");
        pos += fmt_u16_dec(pos, UBLOX_GPIO3);
        pos += fmt_str(pos, ",");
        pos += fmt_u16_dec(pos, ublox->params.gpio3_mode & UBLOX_GPIO_MODE_DISABLED);
        if (ublox->params.gpio3_mode == UBLOX_GPIO_MODE_OUTPUT) {
            pos += fmt_str(pos, ",");
            if (ublox->params.gpio3_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                pos += fmt_u16_dec(pos, 1);
            }
            else {
                pos += fmt_u16_dec(pos, 0);
            }
        }
        *pos = '\0';
        at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
    }
    /* GPIO4 */
    if (ublox->params.gpio4_mode != UBLOX_GPIO_MODE_DEFAULT) {
        pos = buf;
        pos += fmt_str(pos, "AT+UGPIOC=");
        pos += fmt_u16_dec(pos, UBLOX_GPIO4);
        pos += fmt_str(pos, ",");
        pos += fmt_u16_dec(pos, ublox->params.gpio4_mode & UBLOX_GPIO_MODE_DISABLED);
        if (ublox->params.gpio4_mode == UBLOX_GPIO_MODE_OUTPUT) {
            pos += fmt_str(pos, ",");
            if (ublox->params.gpio4_mode & UBLOX_GPIO_OUTPUT_HIGH) {
                pos += fmt_u16_dec(pos, 1);
            }
            else {
                pos += fmt_u16_dec(pos, 0);
            }
        }
        *pos = '\0';
        at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
    }

    if (ublox->params.gps_connected) {
        if (ublox->params.gps_pm_exti) {
            /* Turn on GPS */
            at_send_cmd_wait_ok(at_dev, "AT+UGPS=1,0", GSM_SERIAL_TIMEOUT);

            /* configure UBX-CFG-PM2 to use EXTI */
            at_send_cmd_get_lines(at_dev, "AT+UGUBX=\"B5 62 06 3B 30 00 02 06 00 00 60 14 43 01 E8 03 00 00 10 "
                    "27 00 00 00 00 00 00 00 00 00 00 2C 01 00 00 4F C1 03 00 86 02 00 00 "
                    "FE 00 00 00 64 40 01 00 00 00 00 00 BE 58\"", buf, sizeof(buf), false, GSM_SERIAL_TIMEOUT);

            /* Turn off GPS */
            at_send_cmd_wait_ok(at_dev, "AT+UGPS=0", GSM_SERIAL_TIMEOUT);
        }

        /* setup A-GPS */
        if (ublox->params.agps_token) {
            pos = buf;
            pos += fmt_str(pos, "AT+UGSRV=\"");
            pos += fmt_str(pos, ublox->params.agps_server);
            pos += fmt_str(pos, "\",\"");
            pos += fmt_str(pos, ublox->params.agps_server2);
            pos += fmt_str(pos, "\",\"");
            pos += fmt_str(pos, ublox->params.agps_token);
            pos += fmt_str(pos, "\"");
            *pos = '\0';
            at_send_cmd_wait_ok(at_dev, buf, GSM_SERIAL_TIMEOUT);
        }

        at_send_cmd_wait_ok(at_dev, "AT+UGRMC=1", GSM_SERIAL_TIMEOUT);
        at_send_cmd_wait_ok(at_dev, "AT+UGGSV=1", GSM_SERIAL_TIMEOUT);
        at_send_cmd_wait_ok(at_dev, "AT+UGGGA=1", GSM_SERIAL_TIMEOUT);
        at_send_cmd_wait_ok(at_dev, "AT+UGGSA=1", GSM_SERIAL_TIMEOUT);
    }

    rmutex_unlock(&dev->mutex);

    return 0;
}

static int _sleep(gsm_t *dev)
{
    ublox_gsm_t *ublox = (ublox_gsm_t *)dev;

    if (ublox->params.dtr_pin != GPIO_UNDEF) {
        gpio_set(ublox->params.dtr_pin);
        return 0;
    }

    return -1;
}

static int wake_up(gsm_t *dev)
{
    ublox_gsm_t *ublox = (ublox_gsm_t *)dev;

    if (ublox->params.dtr_pin != GPIO_UNDEF) {
        gpio_clear(ublox->params.dtr_pin);
        xtimer_usleep(100);
        return 0;
    }

    return -1;
}

static int change_baudrate(gsm_t *dev, uint32_t baudrate)
{
    char buf[64];
    char *pos = buf;

    pos += fmt_str(pos, "AT+IPR=");
    pos += fmt_u32_dec(pos, baudrate);
    *pos = '\0';

    return at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT);
}

#define MODEM_MAX_CREG_TRIES (60)

static int gprs_init(gsm_t *dev, const char *apn, const char *user, const char *passwd)
{
    (void)user;
    (void)passwd;
    char buf[128];
    char *pos = buf;
    pos += fmt_str(pos, "AT+UPSD=0,1,\"");
    if (fmt_strlen(apn) > sizeof(buf) - fmt_strlen(buf)) {
        return -1;
    }
    pos += fmt_str(pos, apn);
    pos += fmt_str(pos, "\"");
    *pos = '\0';

    rmutex_lock(&dev->mutex);

    int res = at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT);
    if (res) {
        goto out;
    }

    int cnt = 0;
    int creg = gsm_creg_get(dev);
    while (creg != 1 && creg < 5 && cnt++ < MODEM_MAX_CREG_TRIES) {
        creg = gsm_creg_get(dev);
        xtimer_usleep(1 * US_PER_SEC);
    }

    /* activate PDP */
    res = at_send_cmd_get_lines(&dev->at_dev, "AT+UPSDA=0,3", buf, sizeof(buf), false, 180 * US_PER_SEC);
    if (res > 0) {
        res = 0;
    }

out:
    rmutex_unlock(&dev->mutex);
    return res;
}

static int upload_file(gsm_t *dev, const char *req_name,
                       const char *filename,
                       const uint8_t *data, size_t data_len)
{
    assert(filename || data);

    char cmd[128];
    char *pos;
    int ret;

    int fd = -1;
    if (filename) {
        fd = open(filename, O_RDONLY);
        if (fd < 0) {
            return -1;
        }
        off_t size = lseek(fd, 0, SEEK_END);
        if (size <= 0) {
            return -1;
        }
        lseek(fd, 0, SEEK_SET);
        data_len = size;
    }
    pos = cmd;
    pos += fmt_str(pos, "AT+UDWNFILE=\"");
    pos += fmt_str(pos, req_name);
    pos += fmt_str(pos, "\",");
    pos += fmt_u32_dec(pos, data_len);
    *pos = '\0';

    ret = at_send_cmd_wait_prompt(&dev->at_dev, cmd, GSM_SERIAL_TIMEOUT);
    if (ret < 0) {
        LOG_ERROR("modem_http_post: prompt not received (ret=%d)\n", ret);
        return ret;
    }
    if (fd >= 0) {
        size_t idx = 0;
        do {
            ssize_t len = (data_len - idx) > sizeof(cmd) ? sizeof(cmd) : (data_len - idx);
            len = read(fd, cmd, len);
            if (len < 0) {
                return len;
            }
            at_send_bytes(&dev->at_dev, cmd, len);
            idx += len;
        } while (idx < data_len);
        close(fd);
    }
    else {
        at_send_bytes(&dev->at_dev, (char *)data, data_len);
    }

    /* return at_send_cmd_wait_ok(&dev->at_dev, "", GSM_SERIAL_TIMEOUT);*/
    return at_expect_bytes(&dev->at_dev, "\r\nOK", GSM_SERIAL_TIMEOUT);
}

static int download_file(gsm_t *dev, const char *resp_name,
                         const char *filename,
                         uint8_t *resultbuf, size_t result_len)
{
    assert(resultbuf || filename);

    int ret = 0;
    char cmd[128];
    char resp[128];
    char *pos;
    size_t read = 0;
    unsigned long len = 0;

    int fd = -1;
    if (filename) {
        fd = open(filename, O_RDWR | O_CREAT);
        if (fd < 0) {
            return -1;
        }
    }
    /* Get file size */
    pos = cmd;
    pos += fmt_str(pos, "AT+ULSTFILE=2,\"");
    pos += fmt_str(pos, resp_name);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    ret = at_send_cmd_get_lines(&dev->at_dev, cmd, resp, sizeof(resp), false, GSM_SERIAL_TIMEOUT);
    if (ret > 0 && strncmp(resp, "+ULSTFILE:", 10) == 0) {
        len = strtoul(resp + 10, NULL, 10);
        if ((fd < 0) && (len > result_len)) {
            return -1;
        }
    }
    else {
        goto end;
    }
    /* Download file by block */
    pos = cmd;
    pos += fmt_str(pos, "AT+URDBLOCK=\"");
    pos += fmt_str(pos, resp_name);
    pos += fmt_str(pos, "\"");
    pos += fmt_str(pos, ",");
    char *pos_size = pos;
    do {
        pos = pos_size;
        pos += fmt_u32_dec(pos, read);
        pos += fmt_str(pos, ",");
        /* 16 = 4 digits + 2 * ',' + '\r\nOK\r' + 4 * '"' + ' ' */
        pos += fmt_u32_dec(pos, sizeof(resp) - (10 + 16 + strlen(resp_name)));
        *pos = '\0';
        ret = at_send_cmd_get_lines(&dev->at_dev, cmd, resp, sizeof(resp), true, GSM_SERIAL_TIMEOUT);
        if (ret > 11 && strncmp(resp, "+URDBLOCK: \"", 12) == 0
                && strncmp(resp + 12, resp_name, strlen(resp_name)) == 0) {
            pos = resp + 14 + strlen(resp_name);
            unsigned long block_size = strtoul(pos, &pos, 10);
            pos += 2; /* remove ',"' */
            if (fd >= 0) {
                ret = write(fd, pos, block_size);
                if (ret < 0) {
                    goto end;
                }
            }
            else {
                memcpy(resultbuf + read, pos, block_size);
            }
            read += block_size;
        }
        else {
            ret = -1;
            goto end;
        }
    } while (read < len);

    if (fd < 0) {
        resultbuf[read] = '\0';
    }

end:
    if (fd >= 0) {
        close(fd);
    }

    return ret;
}

static int delete_file(gsm_t *dev, const char *filename)
{
    char cmd[128];
    char *pos;

    pos = cmd;
    pos += fmt_str(pos, "AT+UDELFILE=\"");
    pos += fmt_str(pos, filename);
    pos += fmt_str(pos, "\"");
    *pos = '\0';
    return at_send_cmd_wait_ok(&dev->at_dev, cmd, GSM_SERIAL_TIMEOUT);
}

static const char *get_ct(const gsm_http_params_t *params)
{
    if (!params) {
        return "1";
    }
    switch (params->content_type) {
    case GSM_CT_APPLICATION_X_WWW_URL_ENCODED:
        return "0";
    case GSM_CT_TEXT_PLAIN:
        return "1";
    case GSM_CT_APPLICATION_OCTET_STREAM:
        return "2";
    case GSM_CT_MULTIPART_FORM_DATA:
        return "3";
    case GSM_CT_APPLICATION_JSON:
        return "4";
    case GSM_CT_APPLICATION_XML:
        return "5";
    default:
        return "1";
    }
}

static ssize_t http_req(gsm_t *dev,
                        const char *url,
                        const char *filename,
                        const uint8_t *data, size_t data_len,
                        const char *result_filename,
                        uint8_t *resultbuf, size_t result_len,
                        bool post, const gsm_http_params_t *params)
{
    rmutex_lock(&dev->mutex);

    int ret = 0;
    char cmd[128];
    const char *req_name = "req_f";
    const char *resp_name = "resp_f";
    const char *server = url;
    const char *resource;
    long result = 0;

    if (post) {
        ret = upload_file(dev, req_name, filename, data, data_len);
        if (ret) {
            goto end;
        }
    }

    int creg = gsm_creg_get(dev);
    int cnt = 0;
    while (creg != 1 && creg < 5 && cnt++ < MODEM_MAX_CREG_TRIES) {
        xtimer_usleep(1 * US_PER_SEC);
        creg = gsm_creg_get(dev);
    }

    /* get PDP context state */
    if (at_send_cmd_get_lines(&dev->at_dev, "AT+UPSND=0,8", cmd, sizeof(cmd), false, GSM_SERIAL_TIMEOUT) < 0) {
        ret = -1;
        goto end;
    }
    if (strncmp(cmd, "+UPSND: 0,8,0", 13) == 0) {
        /* activate PDP */
        if (at_send_cmd_get_lines(&dev->at_dev, "AT+UPSDA=0,3", cmd, sizeof(cmd), false, 180 * US_PER_SEC) < 0) {
            ret = -1;
            goto end;
        }
    }

    if (strncmp(url, "https://", 8) == 0) {
        /* set https */
        if (at_send_cmd_wait_ok(&dev->at_dev, "AT+UHTTP=0,6,1", GSM_SERIAL_TIMEOUT) != 0) {
            LOG_ERROR("modem_http_req: error when setting https\n");
            ret = -1;
            goto end;
        }
        server += 8;
    }
    else if (strncmp(url, "http://", 7) == 0) {
        server += 7;
    }
    else {
        ret = -1;
        goto end;
    }

    resource = server;
    while (*resource != '/' && *resource != '\0') {
        resource++;
    }
    char *pos = cmd;
    pos += fmt_str(pos, "AT+UHTTP=0,1,\"");
    if ((size_t)(resource - server) > sizeof(cmd) - 17) {
        LOG_ERROR("modem_http_req: cannot write server name\n");
        goto end;
    }
    memcpy(pos, server, resource - server);
    pos += resource - server;
    pos += fmt_str(pos, "\"");
    *pos = '\0';

    if (at_send_cmd_wait_ok(&dev->at_dev, cmd, GSM_SERIAL_TIMEOUT) != 0) {
        LOG_ERROR("modem_http_req: error when sending server name\n");
        ret = -1;
        goto end;
    }

    if (params && params->user_agent) {
        pos = cmd;
        pos += fmt_str(pos, "AT+UHTTP=0,9,\"0:User-Agent:");
        pos += fmt_str(pos, params->user_agent);
        pos += fmt_str(pos, "\"");
        *pos = '\0';
        if (at_send_cmd_wait_ok(&dev->at_dev, cmd, GSM_SERIAL_TIMEOUT) != 0) {
            ret = -1;
            goto end;
        }
    }

    if (params && params->port) {
        /* set port */
        pos = cmd;
        pos += fmt_str(pos, "AT+UHTTP=0,5,");
        pos += fmt_s16_dec(pos, params->port);
        *pos = '\0';
        if (at_send_cmd_wait_ok(&dev->at_dev, cmd, GSM_SERIAL_TIMEOUT) != 0) {
            LOG_ERROR("Error");
            ret = -1;
            goto end;
        }
    }

    /* url, response in "resp", content-type: json */
    pos = cmd;
    pos += fmt_str(pos, "AT+UHTTPC=0,");
    if (post) {
        /* POST */
        pos += fmt_str(pos, "4");
    }
    else {
        /* GET */
        pos += fmt_str(pos, "1");
    }
    pos += fmt_str(pos, ",\"");
    if ((sizeof(cmd) - (pos - cmd)) < strlen(resource) + 3) {
        ret = -1;
        goto end;
    }
    /* URL */
    if (*resource == '\0') {
        pos += fmt_str(pos, "/");
    }
    else {
        pos += fmt_str(pos, resource);
    }
    pos += fmt_str(pos, "\",\"");
    if ((sizeof(cmd) - (pos - cmd)) < strlen(resp_name) + 2) {
        ret = -1;
        goto end;
    }
    /* Response file name */
    pos += fmt_str(pos, resp_name);
    pos += fmt_str(pos, "\"");
    if (post) {
        /* Request file name */
        if ((sizeof(cmd) - (pos - cmd)) < strlen(req_name) + 5) {
            ret = -1;
            goto end;
        }
        pos += fmt_str(pos, ",\"");
        pos += fmt_str(pos, req_name);
        pos += fmt_str(pos, "\",");
        /* Content-type (default: text/plain) */
        pos += fmt_str(pos, get_ct(params));
    }
    *pos = '\0';
    if (at_send_cmd_wait_ok(&dev->at_dev, cmd, GSM_SERIAL_TIMEOUT) != 0) {
        LOG_ERROR("modem_http_req: error when sending POST cmd\n");
        ret = -1;
        goto end;
    }

    for (int retry = 0; retry < 3; retry++) {
        at_readline(&dev->at_dev, cmd, sizeof(cmd), false, 20 * US_PER_SEC);
        if (strncmp(cmd, "+UUHTTPCR:", 10) != 0) {
            ret = -1;
            continue;
        }
        long profile, command;
        pos = cmd + 10; /* remove '+UUHTTPCR:' */
        profile = strtol(pos, &pos, 10);
        if (pos && *pos == ',') {
            pos++;
        }
        else {
            ret = -1;
        }
        command = strtol(pos, &pos, 10);
        if (pos && *pos == ',') {
            pos++;
        }
        else {
            ret = -1;
        }
        result = strtol(pos, &pos, 10);
        if (result == 0) {
            ret = -1;
        }
        else {
            (void)profile;
            (void)command;
            /* TODO check profile and command as well */
            ret = 0;
        }
        break;
    }
    if (ret != 0) {
        goto end;
    }

    ret = download_file(dev, resp_name, result_filename, resultbuf, result_len);

end:
    if (post) {
        delete_file(dev, req_name);
    }
    delete_file(dev, resp_name);

    LOG_INFO("modem_http_post: finished (%ld)\n", result);

    rmutex_unlock(&dev->mutex);

    return ret;
}

static ssize_t http_get(gsm_t *dev, const char *url, uint8_t *resultbuf, size_t len,
                        const gsm_http_params_t *params)
{
    return http_req(dev, url, NULL, NULL, 0, NULL, resultbuf, len, false, params);
}

static ssize_t http_get_file(gsm_t *dev, const char *url, const char *filename,
                             const gsm_http_params_t *params)
{
    return http_req(dev, url, NULL, NULL, 0, filename, NULL, 0, false, params);
}

static ssize_t http_post(gsm_t *dev,
                         const char *url,
                         const uint8_t *data, size_t data_len,
                         uint8_t *resultbuf, size_t result_len,
                         const gsm_http_params_t *params)
{
    return http_req(dev, url, NULL, data, data_len, NULL, resultbuf, result_len, true, params);
}

static ssize_t http_post_file(gsm_t *dev, const char *url,
                              const char *filename,
                              uint8_t *resultbuf, size_t result_len,
                              const gsm_http_params_t *params)
{
    return http_req(dev, url, filename, NULL, 0, NULL, resultbuf, result_len, true, params);
}

static ssize_t http_post_file_2(gsm_t *dev, const char *url,
                                const char *filename,
                                const char *result_file,
                                const gsm_http_params_t *params)
{
    return http_req(dev, url, filename, NULL, 0, result_file, NULL, 0, true, params);
}

static size_t copy_nmea(char *dest, const char *src, size_t rem_bytes, size_t len_src)
{
    if (!len_src) {
        return 0;
    }
    const char *p = src;
    while (*p != '$' && len_src) {
        p++;
        len_src--;
    }
    size_t to_copy = len_src < rem_bytes ? len_src : rem_bytes;
    if (to_copy > 2 && p[to_copy - 3] == 'O' && p[to_copy - 2] == 'K') {
        to_copy -= 2;
    }
    memcpy(dest, p, to_copy);
    dest[to_copy - 1] = '\0';
    rem_bytes -= to_copy;

    return rem_bytes;
}

static ssize_t get_nmea(gsm_t *dev, char *nmea, size_t len)
{
    char buf[512];
    ssize_t res;
    size_t rem_bytes = len;

    rmutex_lock(&dev->mutex);

    res = at_send_cmd_get_lines(&dev->at_dev, "AT+UGGSA?", buf, sizeof(buf), false, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        goto out;
    }
    rem_bytes = copy_nmea(nmea + (len - rem_bytes), buf, rem_bytes, res);
    if (rem_bytes) {
        rem_bytes++;
    }

    res = at_send_cmd_get_lines(&dev->at_dev, "AT+UGGGA?", buf, sizeof(buf), false, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        goto out;
    }
    rem_bytes = copy_nmea(nmea + (len - rem_bytes), buf, rem_bytes, res);
    if (rem_bytes) {
        rem_bytes++;
    }

    res = at_send_cmd_get_lines(&dev->at_dev, "AT+UGGSV?", buf, sizeof(buf), false, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        goto out;
    }
    rem_bytes = copy_nmea(nmea + (len - rem_bytes), buf, rem_bytes, res);
    if (rem_bytes) {
        rem_bytes++;
    }

    res = at_send_cmd_get_lines(&dev->at_dev, "AT+UGRMC?", buf, sizeof(buf), false, GSM_SERIAL_TIMEOUT);
    if (res < 0) {
        goto out;
    }
    rem_bytes = copy_nmea(nmea + (len - rem_bytes), buf, rem_bytes, res);
    res = len - rem_bytes;

out:
    rmutex_unlock(&dev->mutex);

    return res;
}

static int gps_start(gsm_t *gsmdev)
{
    return at_send_cmd_wait_ok(&gsmdev->at_dev, "AT+UGPS=1,4,127", 20 * US_PER_SEC);
}

static int gps_stop(gsm_t *gsmdev)
{
    return at_send_cmd_wait_ok(&gsmdev->at_dev, "AT+UGPS=0", 20 * US_PER_SEC);
}

const gsm_driver_t ublox_gsm_driver = {
    .reset = reset,
    .init = init,
    .init_drv = init_drv,
    .wake_up = wake_up,
    .change_baudrate = change_baudrate,
    .sleep = _sleep,
    .gprs_init = gprs_init,
    .http_post = http_post,
    .http_get = http_get,
    .http_get_file = http_get_file,
    .http_post_file = http_post_file,
    .http_post_file_2 = http_post_file_2,
    .get_nmea = get_nmea,
    .gps_start = gps_start,
    .gps_stop = gps_stop,
};
