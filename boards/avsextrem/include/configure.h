/*
* configure.h - Configuration file for the avsextrem board.
* Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
*
* This source code is licensed under the LGPLv2 license,
* See the file LICENSE for more details.
*
*/

/**
 * @file
 * @brief       definitions for the avsextrem board configuration
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      baar
 * @author      Zakaria Kasmi
 * @version     $Revision: 1127 $
 *
 * @note        $Id: board-conf.h 1127 2013-08-20 17:15:29 kasmi $
 */

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

/**
 *
 * The offline configuration file "configure.h" is automatically
 * included in every file by the build system and allows to configure
 * all neccessary defines on cpu, board and application level.
 *
 * The configuration is built of
 * \li a application specific file (project-conf.h)
 * \li a board specific file (board-conf.h) and
 * \li a cpu specific file (cpu-conf.h).
 * All of which are chosen by the build system depending on your arguments.
 *
 * @{
 */

//#include "project-conf.h"
//#include "board-conf.h"
#include "cpu-conf.h"

#ifdef MODULE_SYSMON
#ifndef TRACELOG_CONF_NUM_ENTRIES
#define TRACELOG_CONF_NUM_ENTRIES	10
#endif
#else
#undef TRACELOG_CONF_NUM_ENTRIES
#endif


/* ------------------------------------------------------------------------- */
/*
 * documentation of defines that do not belong to a specific module:
 */

/**
 * @def FEUERWARE_CONF_BOARD_NAME
 * @brief	Defines the name of the board as a string
 */

/**
 * @def FEUERWARE_CONF_CPU_NAME
 * @brief	Defines the name of the cpu as a string
 */

/**
 * @def FEUERWARE_CONF_SUPPORTS_FILES
 * @brief	Defines if standard library file functions are available
 */
#ifndef FEUERWARE_CONF_SUPPORTS_FILES
#define FEUERWARE_CONF_SUPPORTS_FILES 0
#endif

/** @} */
#endif /* CONFIGURE_H_ */
