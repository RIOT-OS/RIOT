/*
 * tcp_timer.c
 *
 *  Created on: 21.01.2012
 *      Author: Oliver
 */

#include <stdio.h>
#include "tcp_timer.h"
#include "vtimer.h"
#include "thread.h"
#include "destiny.h"
#include "socket.h"
#include "net_help/msg_help.h"

void handle_syn_sent(socket_internal_t *current_socket)
	{
	msg_t send;
	if ((current_socket->socket_values.tcp_control.no_of_retry == 0) &&
			(timex_sub(vtimer_now(), current_socket->socket_values.tcp_control.last_packet_time).microseconds >
			TCP_SYN_INITIAL_TIMEOUT))
		{
		current_socket->socket_values.tcp_control.no_of_retry++;
		net_msg_send(&send, current_socket->pid, 0, TCP_RETRY);
		printf("FIRST RETRY!\n");
		}
	else if ((current_socket->socket_values.tcp_control.no_of_retry > 0) &&
			(timex_sub(vtimer_now(), current_socket->socket_values.tcp_control.last_packet_time).microseconds >
			(current_socket->socket_values.tcp_control.no_of_retry * TCP_SYN_TIMEOUT + TCP_SYN_INITIAL_TIMEOUT)))
		{
		current_socket->socket_values.tcp_control.no_of_retry++;
		if (current_socket->socket_values.tcp_control.no_of_retry > TCP_MAX_SYN_RETRIES)
			{
			net_msg_send(&send, current_socket->pid, 0, TCP_TIMEOUT);
			printf("TCP SYN TIMEOUT!!\n");
			}
		else
			{
			net_msg_send(&send, current_socket->pid, 0, TCP_RETRY);
			printf("NEXT RETRY!\n");
			}
		}
	}

void check_sockets(void)
	{
	socket_internal_t *current_socket;
	uint8_t i = 1;
	while (i < MAX_SOCKETS+1)
		{
		current_socket = getSocket(i);

		if(isTCPSocket(i))
			{
			switch (current_socket->socket_values.tcp_control.state)
				{
				case SYN_SENT:
					{
					handle_syn_sent(current_socket);
					break;
					}
				case SYN_RCVD:
					{
					handle_syn_sent(current_socket);
					break;
					}
				default:
					{
					break;
					}
				}
			}
		i++;
		}
	}

void tcp_general_timer(void)
	{
	vtimer_t tcp_vtimer;
	timex_t interval = timex_set(0, 500*1000);

	while (1)
		{
		check_sockets();
//		cur_time = vtimer_now();
//		printf("Time Seconds: %li uSeconds: %li\n", cur_time.seconds, cur_time.microseconds);
		vtimer_set_wakeup(&tcp_vtimer, interval, tcp_timer_pid);
		thread_sleep();
		}
	}
