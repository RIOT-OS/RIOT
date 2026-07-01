/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_stdio
 * @{
 *
 * @file
 * @brief       STDIO common layer
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "errno.h"
#include "isrpipe.h"
#include "stdio_base.h"
#include "macros/utils.h"
#include "xfa.h"

static uint8_t _rx_buf_mem[STDIO_RX_BUFSIZE];
isrpipe_t stdin_isrpipe = ISRPIPE_INIT(_rx_buf_mem);

#if IS_USED(MODULE_STDIO_NOTIFY)
static stdio_notify_cb_t _notify_cb;
static void *_notify_arg;

void stdio_set_notify(stdio_notify_cb_t cb, void *arg)
{
    unsigned irqstate = irq_disable();

    _notify_arg = arg;
    _notify_cb = cb;

    irq_restore(irqstate);
}

static inline void _notify(void)
{
    if (_notify_cb) {
        _notify_cb(_notify_arg);
    }
}

int stdio_rx_write_one(uint8_t c)
{
    int res = isrpipe_write_one(&stdin_isrpipe, c);

    /* notify even if the pipe is full, isrpipe_read() would unblock anyway */
    _notify();

    return res;
}

int stdio_rx_write(const uint8_t *buf, size_t len)
{
    int res = isrpipe_write(&stdin_isrpipe, buf, len);

    /* notify even if the pipe is full, isrpipe_read() would unblock anyway */
    _notify();

    return res;
}
#endif

#ifdef MODULE_STDIO_DISPATCH
XFA_INIT_CONST(stdio_provider_t, stdio_provider_xfa);

void stdio_init(void)
{
    for (unsigned i = 0; i < XFA_LEN(stdio_provider_t, stdio_provider_xfa); ++i) {
        if (stdio_provider_xfa[i].open) {
            stdio_provider_xfa[i].open();
        }
    }
}

ssize_t stdio_write(const void* buffer, size_t len)
{
    for (unsigned i = 0; i < XFA_LEN(stdio_provider_t, stdio_provider_xfa); ++i) {
        stdio_provider_xfa[i].write(buffer, len);
    }

    return len;
}

void stdio_close(void) {
    for (unsigned i = 0; i < XFA_LEN(stdio_provider_t, stdio_provider_xfa); ++i) {
        if (stdio_provider_xfa[i].close) {
            stdio_provider_xfa[i].close();
        }
    }
}

#define MAYBE_WEAK
#else
#define MAYBE_WEAK __attribute__((weak))
#endif

MAYBE_WEAK
ssize_t stdio_read(void* buffer, size_t len)
{
    if (!IS_USED(MODULE_STDIN)) {
        return -ENOTSUP;
    }

    return isrpipe_read(&stdin_isrpipe, buffer, len);
}

#if IS_USED(MODULE_STDIO_AVAILABLE)
MAYBE_WEAK
int stdio_available(void)
{
    if (!IS_USED(MODULE_STDIN)) {
        return 0;
    }
    return isrpipe_available(&stdin_isrpipe);
}
#endif

void stdio_clear_stdin(void)
{
    if (IS_USED(MODULE_STDIN)) {
        isrpipe_clear(&stdin_isrpipe);
    }
}
