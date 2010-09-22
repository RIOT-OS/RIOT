/******************************************************************************
Copyright 2008, Freie Universitaet Berlin (FUB). All rights reserved.

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

#ifndef BOARDCONF_H_
#define BOARDCONF_H_

/**
 * @ingroup		conf
 * @ingroup		msba2
 *
 * @{
 */

/**
 * @file
 * @brief		MSB-A2 board configuration
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		baar
 * @version     $Revision$
 *
 * @note		$Id$
 */

#define FEUERWARE_CONF_BOARD_NAME				"FU Berlin MSB-A2"

#ifdef MODULE_CC110X
#define FEUERWARE_CONF_NUM_RADIOS				1
#else
#define FEUERWARE_CONF_NUM_RADIOS				0
#endif

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
