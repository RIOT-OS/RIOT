/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "fmt.h"
#include "log.h"
#include "net/ipv4/addr.h"

#include "gsm/gprs.h"
#include "include/gsm_internal.h"

/**
 * @ingroup     drivers_gsm
 * @{
 *
 * @file
 * @brief       gsm gprs implementation.
 *
 * @author      Max van Kessel
 *
 * @}
 */

#define MODULE  "gsm_gprs: "

static const char * gsm_context_types[GSM_CONTEXT_COUNT] = {"IP", "PPP", "IPV6",
        "IP4V6"};

int gsm_grps_attach(gsm_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        gsm_lock(dev);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CGATT=1",
                                    GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);
    }

    return err;
}

int gsm_grps_detach(gsm_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        gsm_lock(dev);

        err = at_send_cmd_wait_ok(&dev->at_dev, "AT+CGATT=0",
                                    GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);
    }

    return err;
}

int gsm_gprs_setup_pdp_context(gsm_t *dev, uint8_t ctx, gsm_context_t type,
                        const char * apn, const char * user, const char * pass)
{
    int err = -EINVAL;

    if ((dev) && (type < GSM_CONTEXT_COUNT)) {
        char buf[128] = { 0 };
        char *pos = buf;

        pos += fmt_str(pos, "AT+CGDCONT=");
        pos += fmt_u32_dec(pos, ctx);
        pos += fmt_str(pos, ",\"");
        pos += fmt_str(pos, gsm_context_types[type]);
        pos += fmt_str(pos, "\",\"");
        pos += fmt_str(pos, apn);

        if (user) {
            pos += fmt_str(pos, "\",\"");
            pos += fmt_str(pos, user);

            if(pass) {
                pos += fmt_str(pos, "\",\"");
                pos += fmt_str(pos, pass);
            }
        }
        pos += fmt_str(pos, "\"\0");

        gsm_lock(dev);

         /* AT+CGDCONT=<ctx>,"<type>","<apn>",["<user>",["<pass>"]] */
        err = at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);
    }

    return err;
}

int gsm_grps_activate_context(gsm_t *dev, uint8_t ctx)
{
    int err = -EINVAL;

    if (dev) {
       char buf [GSM_AT_LINEBUFFER_SIZE_SMALL];
       char *pos = buf;

       assert(dev);

       pos += fmt_str(pos, "AT+CGACT=1,");
       pos += fmt_u32_dec(pos, ctx);
       *pos = '\0';

        gsm_lock(dev);

        err = at_send_cmd_wait_ok(&dev->at_dev, buf, GSM_SERIAL_TIMEOUT_US);

        gsm_unlock(dev);
    }

    return err;
}

int gsm_grps_deactivate_context(gsm_t *dev, uint8_t ctx)
{
    int err = -EINVAL;

    if (dev) {
        char buf[GSM_AT_LINEBUFFER_SIZE_SMALL];
        char *pos = buf;

        pos += fmt_str(pos, "AT+CGACT=0,");
        pos += fmt_u32_dec(pos, ctx);
        *pos = '\0';

        gsm_lock(dev);

        err = at_send_cmd_get_resp(&dev->at_dev, buf, buf,
                           GSM_AT_LINEBUFFER_SIZE_SMALL, GSM_SERIAL_TIMEOUT_US);

        if (err > 0) {
            if (strcmp(buf, "OK") == 0) {
                err = 0;
            }
            else if (strncmp(buf, "NO CARRIER", 10) == 0) {
                err = 0;
            }
            else {
                err = -1;
            }
        }

        gsm_unlock(dev);
    }
    return err;
}

uint32_t gsm_gprs_get_address(gsm_t *dev, uint8_t ctx)
{
    uint32_t ip = 0;

    if (dev) {
        int res;
        char buf[GSM_AT_LINEBUFFER_SIZE];
        char *pos = buf;

        pos += fmt_str(pos, "AT+CGPADDR=");
        pos += fmt_u32_dec(pos, ctx);
        *pos = '\0';

        gsm_lock(dev);

        res = at_send_cmd_get_resp(&dev->at_dev, buf, buf,
                sizeof(buf), GSM_SERIAL_TIMEOUT_US);

        if ((res > 13) && strncmp(buf, "+CGPADDR:", 9) == 0) {
            buf[res] = '\0';
            pos = strchr(buf, '"');
            if (pos != NULL) {
                char * end = strrchr(buf, '"');
                *end = '\0';
                ipv4_addr_from_str((ipv4_addr_t *)&ip, ++pos);
            }
        }

        gsm_unlock(dev);
    }
    return ip;
}


int gsm_gprs_get_registration(gsm_t *dev)
{
    int res;
    char buf[GSM_AT_LINEBUFFER_SIZE_SMALL];

    gsm_lock(dev);

    res = at_send_cmd_get_resp(&dev->at_dev, "AT+CGREG?", buf,
            GSM_AT_LINEBUFFER_SIZE_SMALL, GSM_SERIAL_TIMEOUT_US);

    gsm_unlock(dev);

    if ((res > 0) && (strncmp(buf, "+CGREG:", 6) == 0)) {
        res = atoi(buf + 9);
    }
    else {
        res = -1;
    }

    return res;
}
