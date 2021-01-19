/*
 * Copyright (C) 2021 Continental
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_syslog
 * @{
 * @file
 * @brief   Syslog implementation - backend
 * @author  Vincent Dupont <vincent.dupont@continental-its.com>
 * @}
 */

#include <string.h>

#include "syslog.h"
#include "syslog_backend.h"
#include "stdio_base.h"
#include "fmt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if CONFIG_SYSLOG_BACKEND_EXTRA
#include "syslog_backend_extra.h"
#else
/**
 * @brief   External user-defined backend(s) driver(s)
 *
 * This must be defined in @p syslog_backend_extra.h if @p CONFIG_SYSLOG_BACKEND_EXTRA
 * is set.
 */
#define SYSLOG_BACKEND_EXTRA
#endif

#ifdef MODULE_SYSLOG_BACKEND_STDIO
static bool _print = true;

static void _stdio_send(struct syslog_msg *msg)
{
    if (!_print) {
        return;
    }
    syslog_msg_get(msg);
    stdio_write(msg->msg, msg->len);
    if (msg->len && msg->msg[msg->len - 1] != '\n') {
        stdio_write("\n", 1);
    }
    syslog_msg_put(msg);
}

void syslog_backend_stdio_print(bool print)
{
    _print = print;
}

static const syslog_backend_t stdio_be = {
    .send = _stdio_send,
};
#endif /* MODULE_SYSLOG_BACKEND_STDIO */

#ifdef MODULE_SYSLOG_BACKEND_FILE
extern const syslog_backend_t file_be;
#endif

const syslog_backend_t *syslog_backends[] = {
#ifdef MODULE_SYSLOG_BACKEND_STDIO
    &stdio_be,
#endif
#ifdef MODULE_SYSLOG_BACKEND_FILE
    &file_be,
#endif
    SYSLOG_BACKEND_EXTRA
};
const size_t syslog_backends_numof = ARRAY_SIZE(syslog_backends);
