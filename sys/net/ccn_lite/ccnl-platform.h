/*
 * @f ccnl-platform.h
 *
 * Copyright (C) 2013, Christian Mehlis, Freie University Berlin
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
 */

#include <time.h>

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

// (ms) I moved the following struct def here because it is used by all
// containers apart from the kernel (this way I don't need to redefine it
// for omnet.
//
struct ccnl_timer_s {
    struct ccnl_timer_s *next;
    struct timeval timeout;
    void (*fct)(char, int);
    void (*fct2)(void *, void *);
    char node;
    int intarg;
    void *aux1;
    void *aux2;
    int handler;
};

void ccnl_get_timeval(struct timeval *tv);

long timevaldelta(struct timeval *a, struct timeval *b);

void *ccnl_set_timer(int usec, void (*fct)(void *aux1, void *aux2),
               void *aux1, void *aux2);

void
ccnl_rem_timer(void *h);

extern struct ccnl_timer_s *eventqueue;

#ifdef __cplusplus
}
#endif
