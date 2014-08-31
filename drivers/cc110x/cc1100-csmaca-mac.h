/*
 * Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup     dev_cc110x
 * @brief       ScatterWeb MSB-A2 mac-layer
 *
 * @author      Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @author      Heiko Will <hwill@inf.fu-berlin.de>
 * @version     $Revision: 1999 $
 *
 * @note        $Id: cc1100-csmaca-mac.h 1999 2010-03-16 15:48:18Z hillebra $
 */

#ifndef CC1100_CSMACA_MAC_
#define CC1100_CSMACA_MAC_

#include "cc1100-defaultSettings.h"

//#define CSMACA_MAC_AGGRESSIVE_MODE                // MAC aggressive mode on/off switch

#define CARRIER_SENSE_TIMEOUT            (200000)   // Carrier Sense timeout ~ 2 seconds
#define CARRIER_SENSE_TIMEOUT_MIN          (2000)   // Minimum Carrier Sense timeout ~ 20 milliseconds
#define CS_TX_SWITCH_TIME                    (80)   // Carrier Sense to TX switch time (measured ~ 350 us)

/** All values are in ticks (x10 us) */
#define PRIO_ALARM_DIFS                     (200)   // DIFS for ALARM packets, the default wait time
#define PRIO_ALARM_SLOTTIME   (CS_TX_SWITCH_TIME)   // Time of one additional wait slot
#define PRIO_ALARM_MIN_WINDOW_SIZE            (2)   // Minimum window size of backoff algorithm
#define PRIO_ALARM_MAX_WINDOW_SIZE            (8)   // Maximum window size of backoff algorithm

#define PRIO_WARN_DIFS                     (1000)   // DIFS for WARN packets, the default wait time
#define PRIO_WARN_SLOTTIME    (CS_TX_SWITCH_TIME)   // Time of one additional wait slot
#define PRIO_WARN_MIN_WINDOW_SIZE             (2)   // Minimum window size of backoff algorithm
#define PRIO_WARN_MAX_WINDOW_SIZE            (16)   // Maximum window size of backoff algorithm

#define PRIO_DATA_DIFS                     (2500)   // DIFS for normal data packets, the default wait time
#define PRIO_DATA_SLOTTIME    (CS_TX_SWITCH_TIME)   // Time of one additional wait slot
#define PRIO_DATA_MIN_WINDOW_SIZE             (4)   // Minimum window size of backoff algorithm
#define PRIO_DATA_MAX_WINDOW_SIZE            (32)   // Maximum window size of backoff algorithm

#endif /*CC1100_CSMACA_MAC_*/
