/*
 * Receive helper functions for clock synchronization protocols
 *
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 * @brief    Receive helper functions for clock synchronization protocols
 * @file     receive_helper.h
 * @author   Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author   Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */

#ifndef __CLOCKSYNC_COMMON_RECEIVE_HELPER_H
#define __CLOCKSYNC_COMMON_RECEIVE_HELPER_H

/**
 * Calls <protocol>_mac_read function whenever a beacon for <protocol> arrives.
 */
void clocksync_common_init_recv(void);

#endif /* __CLOCKSYNC_COMMON_RECEIVE_HELPER_H */
