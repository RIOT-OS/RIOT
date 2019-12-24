/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_tcp
 *
 * @{
 *
 * @file
 * @brief       TCP event loop declarations.
 *
* @author       Simon Brummer <simon.brummer@posteo.de>
 */

#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GNRC TCPs main processing thread.
 *
 * @param[in] arg   Thread arguments (unused).
 *
 * @returns   Never, its an endless loop
 */
void *_event_loop(__attribute__((unused)) void *arg);

#ifdef __cplusplus
}
#endif

#endif /* EVENTLOOP_H */
/** @} */
