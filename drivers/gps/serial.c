/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <errno.h>
#include <string.h>

#include "gps.h"
#include "gps/serial.h"
#include "periph/uart.h"
#include "isrpipe.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static ssize_t _readline(isrpipe_t *isrpipe, char *resp_buf, size_t len, uint32_t timeout)
{
    ssize_t res = -1;
    char *resp_pos = resp_buf;

    tsrb_empty(&isrpipe->tsrb);

    memset(resp_buf, 0, len);

    while (len) {
        int read_res;
        if ((read_res = isrpipe_read_timeout(isrpipe, resp_pos, 1, timeout)) == 1) {
            if (*resp_pos == '\r') {
                continue;
            }
            if (*resp_pos == '\n') {
                *resp_pos = '\0';
                res = resp_pos - resp_buf;
                goto out;
            }

            resp_pos += read_res;
            len -= read_res;
        }
        else if (read_res == -ETIMEDOUT) {
            res = -ETIMEDOUT;
            break;
        }
    }

out:
    return res;
}

static int _get_loc(gps_t *dev, char *lat, char *lon, uint32_t maxwait)
{
    gps_serial_t *gps_serial = (gps_serial_t*) dev;

    char linebuf[128];
    uint64_t before = xtimer_now_usec64();

    while ((maxwait == 0) || ((xtimer_now_usec64() - before) < maxwait)) {
        ssize_t res = _readline(&gps_serial->isrpipe, linebuf, sizeof(linebuf), maxwait);
        if (res >= 6 /* $GPGLL */) {
            DEBUG("gps: %s\n", linebuf);
            if (gps_parse_gll((uint8_t*) (linebuf), lat, lon) == 0) {
                return 0;
            }
        }
    }

    return -ETIMEDOUT;
}

static int _power(gps_t *dev, gps_powermode_t mode)
{
    (void)dev;
    (void)mode;

    return -ENOTSUP;
}

static const gps_driver_t _gps_serial_driver = {
    .get_loc=_get_loc,
    .power=_power,
};

int gps_serial_init(gps_serial_t *dev, const gps_serial_params_t *params)
{
    isrpipe_init(&dev->isrpipe, dev->buf, sizeof(dev->buf));
    uart_init(params->uart, params->baudrate, (uart_rx_cb_t) isrpipe_write_one,
              &dev->isrpipe);

    dev->super.driver = &_gps_serial_driver;

    return 0;
}
