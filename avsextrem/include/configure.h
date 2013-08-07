/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of FeuerWare.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

FeuerWare is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

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
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
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
