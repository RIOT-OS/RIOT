/*
 * destiny.c
 *
 *  Created on: 03.09.2011
 *      Author: Oliver
 */

#include <thread.h>
#include <stdio.h>
#include <string.h>
#include "udp.h"
#include "socket.h"

void init_transport_layer(void)
	{
	printf("Initiating Transport Layer packages.\n");
	// SOCKETS
	memset(sockets, 0, MAX_SOCKETS*sizeof(socket_t));

	// UDP
	int udp_thread_pid = thread_create(udp_stack_buffer, 2048, PRIORITY_MAIN, CREATE_STACKTEST, udp_packet_handler, "udp_packet_handler");
	set_udp_packet_handler_pid(udp_thread_pid, buffer);

	// TCP

	}
