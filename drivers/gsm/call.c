/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#include <string.h>
#include <assert.h>
#include <errno.h>

#include "periph/uart.h"
#include "fmt.h"
#include "log.h"

#include "byteorder.h"

#include "gsm/call.h"
#include "include/gsm_internal.h"

/**
 * @ingroup     drivers_gsm
 * @{
 *
 * @file
 * @brief       gsm call implementation.
 *
 * @author      Max van Kessel
 *
 * @}
 */
#define MODULE  "gsm_call: "

int gsm_call_dial_up(gsm_t *dev, const char * number, bool is_voice_call)
{
    int result = -EINVAL;

    if (dev && number) {
        char buf[GSM_AT_LINEBUFFER_SIZE];
        char *pos = buf;

        pos += fmt_str(pos, "ATD");
        pos += fmt_str(pos, number);

        if (is_voice_call) {
            pos += fmt_str(pos, ";");
        }
        *pos = '\0';

        gsm_lock(dev);

        result = at_send_cmd_get_resp(&dev->at_dev, buf, buf, sizeof(buf), GSM_SERIAL_TIMEOUT_US * 5);

        if (result > 0) {
            if (strncmp(buf, "CONNECT", 7) == 0) {
                result = 0;

                if(!is_voice_call){
                    at_drain(&dev->at_dev);

                    /* switch to data mode requires another buffer */
                    dev->state = GSM_CALL;
                }
            }
            else {
                LOG_INFO(MODULE"unexpected response: %s\n", buf);
                result = -1;
            }
        }

        gsm_unlock(dev);
    }

    return result;
}

void gsm_call_dial_down(gsm_t *dev)
{
    if (dev) {
        at_drain(&dev->at_dev);

        /* switch to data mode requires another buffer */
        dev->state = GSM_ON;
    }
}

int __attribute__((weak)) gsm_call_switch_to_command_mode(gsm_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        if (dev->state == GSM_CALL) {

            dev->state = GSM_ON;

            gsm_lock(dev);

            err = at_send_cmd_wait_ok(&dev->at_dev, "+++", GSM_SERIAL_TIMEOUT_US);

            gsm_unlock(dev);

            if (err != 0) {
                dev->state = GSM_CALL;
            }
        }
        else {
            err = 0;
        }
    }

    return err;
}

int __attribute__((weak)) gsm_call_switch_to_data_mode(gsm_t *dev)
{
    int err = -EINVAL;

    if (dev) {
        if (dev->state == GSM_ON) {
            char buf[GSM_AT_LINEBUFFER_SIZE];

            gsm_lock(dev);

            err = at_send_cmd_get_resp(&dev->at_dev, "ATO", buf,
                            GSM_AT_LINEBUFFER_SIZE, GSM_SERIAL_TIMEOUT_US);

            gsm_unlock(dev);

            if (err > 0) {
                if (strncmp(buf, "CONNECT", 7) == 0) {
                    err = 0;

                    dev->state = GSM_CALL;
                }
                else {
                    err = -1;
                }
            }
        }
        else {
            err = 0;
        }
    }

    return err;
}
