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
#include "tcp.h"
#include "socket.h"

void init_transport_layer(void)
	{
	printf("Initiating Transport Layer packages.\n");
	// SOCKETS
	memset(sockets, 0, MAX_SOCKETS*sizeof(socket_internal_t));

	// UDP
	int udp_thread_pid = thread_create(udp_stack_buffer, UDP_STACK_SIZE, PRIORITY_MAIN, CREATE_STACKTEST, udp_packet_handler, "udp_packet_handler");
	set_udp_packet_handler_pid(udp_thread_pid, buffer_udp);

	// TCP
	int tcp_thread_pid = thread_create(tcp_stack_buffer, TCP_STACK_SIZE, PRIORITY_MAIN, CREATE_STACKTEST, tcp_packet_handler, "tcp_packet_handler");
	set_tcp_packet_handler_pid(tcp_thread_pid, buffer_tcp);
	}
