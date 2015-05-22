/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file    tcp_timer.h
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 */

#ifndef TCP_TIMER_H_
#define TCP_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_TIMER_RESOLUTION        500*1000

#define SECOND                      1000.0f*1000.0f
#define TCP_TIMER_STACKSIZE         THREAD_STACKSIZE_DEFAULT
#define TCP_SYN_INITIAL_TIMEOUT     6*SECOND
#define TCP_SYN_TIMEOUT             24*SECOND
#define TCP_MAX_SYN_RETRIES         3
#define TCP_INITIAL_ACK_TIMEOUT     3.0f*SECOND     // still static, should be calculated via RTT
#define TCP_ACK_MAX_TIMEOUT         30*SECOND   // TODO: Set back to 90 Seconds

#define TCP_ALPHA                   1.0f/8.0f
#define TCP_BETA                    1.0f/4.0f

#define TCP_NOT_DEFINED             0
#define TCP_RETRY                   1
#define TCP_TIMEOUT                 2
#define TCP_CONTINUE                3

void *tcp_general_timer(void *);

#ifdef __cplusplus
}
#endif

#endif /* TCP_TIMER_H_ */
/**
 * @}
 */
