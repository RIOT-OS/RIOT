/*
 * @f ccnl-ext-debug.c
 * @b CCNL debugging support, dumping routines, memory tracking, stats
 *
 * Copyright (C) 2011-13, Christian Tschudin, University of Basel
 * Copyright (C) 2013, Christian Mehlis, Freie Universität Berlin
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * File history:
 * 2011-04-19 created
 * 2013-03-18 updated (ms): removed omnet related code
 * 2013-03-31 merged with ccnl-debug.h and ccnl-debug-mem.c
 */

#ifndef CCNL_EXT_DEBUG_H
#define CCNL_EXT_DEBUG_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "ccnl-core.h"

char *
ccnl_prefix_to_path(struct ccnl_prefix_s *pr)
{
    static char prefix_buf[256];
    int len, i, j;

    if (!pr) {
        return NULL;
    }

    for (len = 0, i = 0; i < pr->compcnt; i++) {
        if ((len + 1 + 3 * pr->complen[i]) >= (int) sizeof(prefix_buf)) {
            return (char *) "(...prefix...)";
        }

        prefix_buf[len++] = '/';

        for (j = 0; j < pr->complen[i]; j++) {
            unsigned char c = pr->comp[i][j];
            len += sprintf(prefix_buf + len,
                           !isprint(c) || isspace(c) || c == '/' ? "%%%02x" : "%c",
                           c);
        }
    }

    prefix_buf[len] = '\0';
    return prefix_buf;
}

#endif

// eof
