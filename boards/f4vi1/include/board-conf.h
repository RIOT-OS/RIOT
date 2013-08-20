/**
 * board configuration header
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @file
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 *
 */

#ifndef BOARDCONF_H_
#define BOARDCONF_H_

#define FEUERWARE_CONF_BOARD_NAME				"F4VI1 BOARD"

#define HSE_VALUE    ((uint32_t)16000000)
#define RTC_CLOCK_SOURCE_LSE


#ifdef MODULE_CC110X
#define FEUERWARE_CONF_NUM_RADIOS				0
#else
#define FEUERWARE_CONF_NUM_RADIOS				0
#endif

#define CC1101_ANAREN_27MHZ_SETTINGS			1

// if FAT is enabled this board supports files
#define FEUERWARE_CONF_CORE_SUPPORTS_FILES		defined(MODULE_FAT)

#ifdef MODULE_FAT
#define CFG_CONF_MEM_SIZE						0x7FFFFFFF
#define SYSLOG_CONF_NUM_INTERFACES				2
#else
#define SYSLOG_CONF_NUM_INTERFACES				1
#endif


/** @} */
#endif /* BOARDCONF_H_ */
