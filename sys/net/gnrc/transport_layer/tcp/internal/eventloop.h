/*
 * Copyright (C) 2015 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_tcp TCP
 * @ingroup     net_gnrc
 * @brief       RIOT's tcp implementation for the gnrc stack
 *
 * @{
 *
 * @file
 * @brief      Definition for gnrc tcp event processing loop
 *
 * @author     Simon Brummer <brummer.simon@googlemail.com>
 */

#ifndef GNRC_TCP_INTERNAL_EVENTLOOP_H_
#define GNRC_TCP_INTERNAL_EVENTLOOP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief PID of tcp event handling thread
 */
extern kernel_pid_t _gnrc_tcp_pid;

/**
 * @brief TCP's mein processing thread.
 *
 * @param[in] arg   arguments, unused
 *
 * @return   Never returns, its an endless loop
 */
void *_event_loop(__attribute__((unused)) void *arg);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_TCP_INTERNAL_EVENTLOOP_H_ */
/** @} */
