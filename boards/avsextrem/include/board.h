/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */


/**
 * @defgroup    boards_avsextrem Avsextrem
 * @ingroup     boards
 * @brief       Support for the Avsextrem board
 * @{
 *
 * @file
 * @brief       Boards specific drivers and configuration for the Avsextrem board
 *
 * @author      Heiko Will
 * @author      Zakaria Kasmi
 */


#ifndef BOARDCONF_H_
#define BOARDCONF_H_
#include "bitarithm.h"
#include "msba2_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED_RED_PIN (BIT25)
#define LED_GREEN_PIN (BIT26)

#define LED_GREEN_OFF (FIO3SET = LED_GREEN_PIN)
#define LED_GREEN_ON (FIO3CLR = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE (FIO3PIN ^= LED_GREEN_PIN)

#define LED_RED_OFF (FIO3SET = LED_RED_PIN)
#define LED_RED_ON (FIO3CLR = LED_RED_PIN)
#define LED_RED_TOGGLE (FIO3PIN ^= LED_RED_PIN)

#ifdef MODULE_FAT
#define CFG_CONF_MEM_SIZE             0x7FFFFFFF
#define SYSLOG_CONF_NUM_INTERFACES        2
#else
#define SYSLOG_CONF_NUM_INTERFACES        1
#endif

void init_clks1(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARDCONF_H_ */
