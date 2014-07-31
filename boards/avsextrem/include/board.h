/*
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 *
 */


/**
 * @defgroup    boards_avsextrem Avsextrem
 * @ingroup     boards
 * @brief       Support for the Avsextrem board
 * @{
 *
 * @file        board.h
 * @brief       Boards specific drivers and configuration for the Avsextrem board
 *
 * @author      Heiko Will
 * @author      Zakaria Kasmi
 */


#ifndef BOARDCONF_H_
#define BOARDCONF_H_
#include "bitarithm.h"
#include "msba2_common.h"

#define FEUERWARE_CONF_BOARD_NAME         "FU Berlin AVSEXTREM BOARD"

#define LED_RED_PIN (BIT25)
#define LED_GREEN_PIN (BIT26)

#define LED_GREEN_OFF (FIO3SET = LED_GREEN_PIN)
#define LED_GREEN_ON (FIO3CLR = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE (FIO3PIN ^= LED_GREEN_PIN)

#define LED_RED_OFF (FIO3SET = LED_RED_PIN)
#define LED_RED_ON (FIO3CLR = LED_RED_PIN)
#define LED_RED_TOGGLE (FIO3PIN ^= LED_RED_PIN)



#ifdef MODULE_CC110X
#define FEUERWARE_CONF_NUM_RADIOS         1
#else
#define FEUERWARE_CONF_NUM_RADIOS         0
#endif

// if FAT is enabled this board supports files
#define FEUERWARE_CONF_CORE_SUPPORTS_FILES    defined(MODULE_FAT)

#ifdef MODULE_FAT
#define CFG_CONF_MEM_SIZE             0x7FFFFFFF
#define SYSLOG_CONF_NUM_INTERFACES        2
#else
#define SYSLOG_CONF_NUM_INTERFACES        1
#endif

void init_clks1(void);

typedef uint8_t radio_packet_length_t;

/** @} */
#endif /* BOARDCONF_H_ */
