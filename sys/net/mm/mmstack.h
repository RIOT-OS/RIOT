/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

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

#ifndef MMSTACK_H_
#define MMSTACK_H_

/**
 * @defgroup	net_mmstack		Micro Mesh Stack
 * @ingroup		net
 *
 * @{
 */

/**
 * @file
 * @brief
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author 		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: mmstack.h 3854 2011-12-06 15:27:01Z hwill $
 */

#include "radio.h"
#include "net-types.h"

/**
 * @brief	Initialize micro mesh stack.
 */
void mms_init(void);

/**
 * @brief	Initialize micro mesh stack.
 *
 * @param 	rp Routing protocol type identifier.
 * @param	ri Pointer to route interface.
 */
void mms_initp(protocol_t rp, route_interface_t *ri);

/**
 * @brief	Add network interface.
 *
 * @param 	name Network layer name on this interface (maximum 5 characters).
 * @param	addr Network layer address on this interface.
 * @param	radio Radio layer API access.
 *
 * @return	Interface identifier or -1 if an error occurs.
 */
int mms_add_interface(const char *name, uint16_t addr, const radio_t *radio);

/**
 * @brief	Get the address of the network interface with given id.
 *
 * @param	interface_id The network interface id.
 *
 * @return	The address of the network interface with given id
 *          or 0 if no interface with given id is found.
 */
uint16_t mms_get_interface_address(int interface_id);

/**
 * @brief	Set the address of the network interface with the given id.
 *
 * @param	interface_id The network interface id.
 * @param	addr The new address to set.
 *
 * @return	true if address was successfully set; false otherwise (e.g.
 *          wrong interface, invalid address etc.)
 */
bool mms_set_interface_address(int interface_id, uint16_t addr);

/**
 * @brief	Set the output power of the radio device on the network
 *          interface with the given id.
 *
 * @param	interface_id The network interface id.
 * @param	pa_idx Output power value from 0 (lowest output power) to
 *                 X (highest output power).
 *
 * @return	true if output power was successfully set; false otherwise
 *          (e.g. wrong interface, invalid output power value etc.)
 */
bool mms_set_output_power(int interface_id, uint8_t pa_idx);

/**
 * @brief	Set a protocol handler for a given protocol.
 *
 * @param	protocol The protocol identifier (must be unique).
 * @param	handler The packet handler called if a packet of given protocol is received.
 *
 * @return	-1 if an error occurs (e.g. handler table full) else >= 0.
 */
int mms_set_protocol_handler(protocol_t protocol, packet_handler_t handler);

/**
 * @see	::trans_socket(int)
 */
int mms_socket(int type);

/**
 * @see	::trans_connect(int,uint16_t)
 */
int mms_connect(int socket, uint16_t dest_addr);

/**
 * @see	::trans_send(int,void*,int,protocol_t,uint8_t)
 */
int mms_sock_send(int socket, void *buffer, int length, protocol_t protocol,
                  uint8_t priority);

/**
 * @see	::trans_sendto(int,void*,int,protocol_t,uint8_t,uint16_t)
 */
int mms_sock_sendto(int socket, void *buffer, int length, protocol_t protocol,
                    uint8_t priority, uint16_t dest_addr);

/**
 * @brief	Convenience function to send a message via UDPL socket.
 *
 * @param 	msg packet payload
 * @param	msg_len the size of the packet payload in bytes (max. 46 bytes).
 * @param 	dst packet destination address
 * @param 	protocol packet protocol identifier
 * @param 	priority packet priority
 *
 * @return	Upon successful completion, mms_send() returns a value greater zero.
 * 			Otherwise, a negative value is returned to indicate the error.
 */
int mms_send(void *msg, int msg_len, uint16_t dst, protocol_t protocol,
             uint8_t priority);

/**
 * @see	::trans_poll(int)
 */
int mms_poll(int socket);

/**
 * @see	::trans_close(int,int)
 */
int mms_close(int socket, int how);

/**
 * @brief	Receive function for incoming packets.
 *
 * This function is the protocol handler function for micro mesh messages received
 * on the lower layers.
 *
 * @param	msg Incoming packet.
 * @param	msg_size Size of incoming packet.
 * @param	packet_info Additional packet information.
 */
void mms_receive(void *msg, int msg_size, packet_info_t *packet_info);

/**
 * @brief	Flush kernel route table.
 *
 * @param 	flush_static Whether to flush static routes also
 */
void mms_flush_routes(bool flush_static);

/**
 * @brief	Writes to the standard output (stdout) a sequence of data formatted as
 * 			the format argument specifies.
 *
 * If a SSH connection is active, the output is redirected to the node who
 * initialized the SSH connection.
 *
 * @param format String that contains the text to be written to stdout.
 */
void mms_net_printf(const char *format);

/**
 * @brief	Print kernel route table.
 */
void mms_print_routes(void);

/**
 * @brief	Print interface configuration.
 *
 * @param	iface Optional interface parameter.
 */
void mms_print_ifconfig(int iface);

/**
 * @brief	Print Micro Mesh stack statistics.
 */
void mms_print_stats(void);

/** @} */
#endif /* MMSTACK_H_ */
