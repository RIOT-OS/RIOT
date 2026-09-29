/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the "radio" bsp module definitions
 *
 * The radio adaptation runs in an own thread with the highest priority
 * (`THREAD_PRIORITY_MAIN - 4`) and maps to RIOT's `802.15.4 Radio HAL` API.
 *
 * Hardware MAC layer features such as CSMA/CA, ACK handling and retransmissions
 * are handled by OpenWSN, so the radio driver must support disabling AUTOACK
 * and CSMA handling by the hardware. Frame filtering must as well be disabled.
 *
 * OpenWSN expects to recover crc information on every received frame even if it
 * will simply drop frames with invalid crc. The stack can function correctly if
 * radio drivers automatically drop frames with an invalid crc (i.e. the stack
 * doesn't get notified about these frames), but it might print the following
 * error if using `openwsn_serial`:
 *
 * - `[IEEE802154E] wdDataDuration overflows while at state 19 in slotOffset 0`
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, April 2019
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ieee802154/radio.h"
#include "radio.h"

/**
 * @brief   Initialize OpenWSN radio
 *
 * @param[in]   radio_dev     pointer to a dev interface
 *
 * @return  PID of OpenWSN thread
 * @return  -1 on initialization error
 */
int openwsn_radio_init(void *radio_dev);

/**
 * @brief OpenWSN radio variables structure
 */
typedef struct {
    ieee802154_dev_t  *dev;           /**< radio hal */
    radio_capture_cbt startFrame_cb;  /**< start of frame capture callback */
    radio_capture_cbt endFrame_cb;    /**< end of frame capture callback */
} openwsn_radio_t;

#ifdef __cplusplus
}
#endif

/** @} */
