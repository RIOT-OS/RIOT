/*
 * msg_help.c
 *
 *  Created on: 24.11.2011
 *      Author: Oliver
 */

#include <thread.h>
#include <stdio.h>
#include "msg_help.h"

int net_msg_receive(msg_t *m, uint16_t function_id)
	{
	int ret_val;
	ret_val = msg_receive(m);
	while (m->type != function_id)
		{
		ret_val = msg_receive(m);
		}
	return ret_val;
	}

int net_msg_reply(msg_t *m, msg_t *reply, uint16_t function_id)
	{
	reply->type = function_id;
	return msg_reply(m, reply);
	}

int net_msg_send(msg_t *m, unsigned int pid, bool block, uint16_t function_id)
	{
	m->type = function_id;
	return msg_send(m, pid, block);
	}

int net_msg_send_recv(msg_t *m, msg_t *reply, unsigned int pid, uint16_t function_id_m, uint16_t function_id_reply)
	{
	int ret_val;
	m->type = function_id_m;
	ret_val = msg_send_receive(m, reply, pid);
	while(reply->type != function_id_reply)
		{
		ret_val = msg_receive(reply);
		}
	return ret_val;
	}
