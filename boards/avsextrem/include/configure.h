/*
* configure.h - Configuration file for the avsextrem board.
* Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
*
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*
*/

/**
 * @file
 * @brief       definitions for the avsextrem board configuration
 *
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
#define TRACELOG_CONF_NUM_ENTRIES   10
#endif
#else
#undef TRACELOG_CONF_NUM_ENTRIES
#endif

/** @} */
#endif /* CONFIGURE_H_ */
