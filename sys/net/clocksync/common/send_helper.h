/**
 * Send helper functions for clock.-sync. protocols.
 *
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 * @file     send_helper.h
 * @author   Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author   Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */

#ifndef __CLOCKSYNC_COMMON_SEND_HELPER_H
#define __CLOCKSYNC_COMMON_SEND_HELPER_H

/**
 * @brief   Sends a beacon to all other nodes in transmission range
 * @param[in]   send_buf    buffer which will hold the beacon until it is sent (must be larger than beacon_len)
 * @param[in]   beacon      the beacon itself
 * @param[in]   beacon_len  size of the beacon
 */
void clocksync_common_send_broadcast(uint8_t *send_buf, const void *beacon,
                                     size_t beacon_len);

#endif /* __CLOCKSYNC_COMMON_SEND_HELPER_H */
