/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef GNRC_TCP_INTERNAL_EVENTLOOP_H_
#define GNRC_TCP_INTERNAL_EVENTLOOP_H_

/**
 * TODO
 */
void *_event_loop(__attribute__((unused))void *arg);

extern kernel_pid_t _tcp_pid;

#endif

