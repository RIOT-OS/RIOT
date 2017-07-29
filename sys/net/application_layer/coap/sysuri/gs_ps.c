/*
 * Copyright (C) 2017  Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     gcoap_sys_uri
 * @{
 *
 * @file
 * @brief       gcoap endpoint for ps stats
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <string.h>
#include "ps.h"
#include "net/gcoap.h"

#include "thread.h"
#include "sched.h"
#include "thread.h"
#include "kernel_types.h"

#ifdef MODULE_SCHEDSTATISTICS
#include "xtimer.h"
#endif

#ifdef MODULE_TLSF
#include "tlsf.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/*
 * example (formatted) json as output:
 * [
 *   { "pid": 1,
 *     "name": "idle",
 *     "state": "pending",
 *     "Q": 1,
 *     "pri": 15,
 *     "stack":
 *     { "size": 8192,
 *       "used": 420,
 *       "base": 0x8083380,
 *       "current": 0x80851f4
 *     }
 *   }
 * ]
 */

#define GS_PS_KEY_PID               "pid"
#define GS_PS_KEY_NAME              "name"
#define GS_PS_KEY_STATE             "state"
#define GS_PS_KEY_Q                 "Q"
#define GS_PS_KEY_PRIO              "pri"
#define GS_PS_KEY_STACK             "stack"
#define GS_PS_KEY_STACK_SIZE        "size"
#define GS_PS_KEY_STACK_USED        "used"
#define GS_PS_KEY_STACK_BASE        "base"
#define GS_PS_KEY_STACK_CUR         "current"

/* Length is length of the string with quotes and colon */
#define GS_PS_KEY_PID_LEN           strlen(GS_PS_KEY_PID)+3
#define GS_PS_KEY_NAME_LEN          strlen(GS_PS_KEY_NAME)+3
#define GS_PS_KEY_STATE_LEN         strlen(GS_PS_KEY_STATE)+3
#define GS_PS_KEY_Q_LEN             strlen(GS_PS_KEY_Q)+3
#define GS_PS_KEY_PRIO_LEN          strlen(GS_PS_KEY_PRIO)+3
#define GS_PS_KEY_STACK_LEN         strlen(GS_PS_KEY_STACK)+3
#define GS_PS_KEY_STACK_SIZE_LEN    strlen(GS_PS_KEY_STACK_SIZE)+3
#define GS_PS_KEY_STACK_USED_LEN    strlen(GS_PS_KEY_STACK_USED)+3
#define GS_PS_KEY_STACK_BASE_LEN    strlen(GS_PS_KEY_STACK_BASE)+3
#define GS_PS_KEY_STACK_CUR_LEN     strlen(GS_PS_KEY_STACK_CUR)+3

/* Worst case we require 3 more characters to
 * finish the json struct to valid json
 */
#define GS_PS_JSON_MARGIN           3

ssize_t _gcoap_ps_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len)
{
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);

    /* write payload */
    uint8_t *bufpos            = pdu->payload;
    uint8_t *bufpos_key        = pdu->payload;
    bool first = true;

    /* begin payload with a json array */
    *bufpos++ = '[';

    for (kernel_pid_t i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++) {
        thread_t *p = (thread_t *)sched_threads[i];
        if (p != NULL) {
            /* get process statistics */
            if ( bufpos + GS_PS_JSON_MARGIN + 2 > buf + len ) {
                break;
            }
            if (first != true) {
                *bufpos++ = ',';
            }
            first = false;
            *bufpos++ = '{';
            /* save position where the key starts, if the whole key:value doesn't fit, we ommit it */
            bufpos_key = bufpos;
            if ( bufpos + GS_PS_JSON_MARGIN + GS_PS_KEY_PID_LEN > buf + len ) {
                bufpos = bufpos_key;
                *bufpos++ = '}';
                break;
            }
            *bufpos++ = '"';
            memcpy(bufpos, GS_PS_KEY_PID, strlen(GS_PS_KEY_PID));
            bufpos += strlen(GS_PS_KEY_PID);
            *bufpos++ = '"';
            *bufpos++ = ':';
            unsigned pid_len = sprintf((char*)bufpos, "%u", p->pid);
            bufpos += pid_len;

            /* New key, new bufpos_key */
            bufpos_key = bufpos;
            *bufpos++ = ',';
            if ( bufpos + GS_PS_JSON_MARGIN + GS_PS_KEY_NAME_LEN > buf + len ) {
                bufpos = bufpos_key;
                *bufpos++ = '}';
                break;
            }
            *bufpos++ = '"';
            memcpy(bufpos, GS_PS_KEY_NAME, strlen(GS_PS_KEY_NAME));
            bufpos += strlen(GS_PS_KEY_NAME);
            *bufpos++ = '"';
            *bufpos++ = ':';
            *bufpos++ = '"';
            unsigned name_len = strlen(p->name);
            if (bufpos + name_len + 2 > buf + len) {
                bufpos = bufpos_key;
                *bufpos++ = '}';
                break;
            }
            memcpy(bufpos, p->name, name_len);
            bufpos += name_len;
            *bufpos++ = '"';
            *bufpos++ = '}';
        }
    }
    *bufpos++ = ']';

    return gcoap_finish(pdu, bufpos - pdu->payload, COAP_FORMAT_JSON);
}
