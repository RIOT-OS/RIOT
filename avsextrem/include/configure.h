/*
* avsextrem-cc1100.c - CC100 Transceiver Driver for the AVSEXTREM-BOARD.
* Copyright (C) 2013 Heiko Will <hwill@inf.fu-berlin.de>
*
* This source code is licensed under the LGPLv2 license,
* See the file LICENSE for more details.
*
*/

/**
 * @file
 * @brief       definitions for the avsextrem board configuration
 *
 * @author      Freie Universit‰t Berlin, Computer Systems & Telematics
 * @author      baar
 * @author      Zakaria Kasmi
 * @version     $Revision: 1127 $
 *
 * @note        $Id: board-conf.h 1127 2013-06-13 08:48:29Z baar $
 */

#ifndef CONFIGURE_H_
#define CONFIGURE_H_

/**
 * @defgroup	conf	Offline Configuration
 * @ingroup		feuerware
 *
 * The offline configuration file "configure.h" is automatically
 * included in every file by the build system and allows to configure
 * all neccessary defines on cpu, board and project level.
 *
 * The configuration is built of
 * \li a project specific file (project-conf.h)
 * \li a board specific file (board-conf.h) and
 * \li a cpu specific file (cpu-conf.h).
 * All of which are chosen by the build system depending on your arguments.
 *
 * @{
 */

/**
 * @file
 * @brief	Central offline configuration
 *
 * @author      Freie Universit√§t Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author	baar
 * @version     $Revision: 3854 $
 *
 * @note	$Id: configure.h 3854 2011-12-06 15:27:01Z hwill $
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
