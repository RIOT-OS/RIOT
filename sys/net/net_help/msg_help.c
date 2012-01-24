/*
 * msg_help.c
 *
 *  Created on: 24.11.2011
 *      Author: Oliver
 */

#include <thread.h>
#include <stdio.h>
#include "msg_help.h"

int net_msg_receive(msg_t *m)
	{
	return msg_receive(m);
	}

int net_msg_reply(msg_t *m, msg_t *reply, uint16_t message)
	{
	reply->type = message;
	return msg_reply(m, reply);
	}

int net_msg_send(msg_t *m, unsigned int pid, bool block, uint16_t message)
	{
	m->type = message;
	return msg_send(m, pid, block);
	}

int net_msg_send_recv(msg_t *m, msg_t *reply, unsigned int pid, uint16_t message)
	{
	m->type = message;
	return msg_send_receive(m, reply, pid);;
	}
