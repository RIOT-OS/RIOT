/******************************************************************************
Copyright 2008-2009 , Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
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

#ifndef RADIO_H_
#define RADIO_H_

/**
 * @defgroup	net_datalink	Data link layer
 * @ingroup		net
 *
 * @brief	Defines interface of data link layers for use with micro mesh stack.
 *
 * @{
 */

/**
 * @file
 * @brief
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics, FeuerWhere project
 * @author		baar
 * @author		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 1961 $
 *
 * @note		$Id$
 */

#include <stdint.h>
#include <stdbool.h>
#include "radio/types.h"

#ifndef FEUERWARE_CONF_NUM_RADIOS
#define FEUERWARE_CONF_NUM_RADIOS 1
#endif

#define L1_PROTOCOL_CATCH_ALL 			  (0xff)	///< Catch all protocol ID

enum layer_1_protocols
{
	LAYER_1_PROTOCOL_LL_ACK 	= 1,	///< Link-Level Acknowledgement (LL-ACK)
	LAYER_1_PROTOCOL_MM     	= 2,	///< Micro Mesh network packet (MM)
};

/**
 * Radio/MAC API.
 */
typedef struct radio {
	const char*	name;
	const radio_address_t broadcast_address;
	const uint8_t output_power_max;
	/**
	 * @return	the average transmission duration of one packet
	 * 			in milliseconds, e.g. till ACK received.
	 */
	int (*get_avg_transmission_duration)(void);
	radio_address_t (*get_address)(void);
	bool (*set_address)(radio_address_t address);
	bool (*set_output_power)(uint8_t pa_idx);
	bool (*set_packet_monitor)(packet_monitor_t monitor);
	/**
	 * @return	-1 if an error occurs (e.g. handler table full) else >= 0.
	 */
	int (*set_packet_handler)(protocol_t protocol, packet_handler_t handler);
	/**
	 * @return	A negative value if operation failed; else the number of transmitted bytes.
	 */
	int (*send)(radio_address_t address, protocol_t protocol, int priority, char *payload, int payload_len);
	void (*print_stats)(void);
	void (*print_config)(void);
} radio_t;

extern const struct radio* feuerware_radios[FEUERWARE_CONF_NUM_RADIOS];

/** @} */

#endif /* RADIO_H_ */
