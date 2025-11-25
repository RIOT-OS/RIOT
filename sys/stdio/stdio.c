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
    return tsrb_avail(&stdin_isrpipe.tsrb);
}
#endif
