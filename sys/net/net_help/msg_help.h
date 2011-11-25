/*
 * msg_help.h
 *
 *  Created on: 24.11.2011
 *      Author: Oliver
 */

#ifndef MSG_HELP_H_
#define MSG_HELP_H_

// Function IDs
#define FID_SIXLOWIP_TCP		0
#define FID_SIXLOWIP_UDP		1
#define FID_TCP_PH				2
#define FID_UDP_PH				3
#define FID_H_PAYLOAD			4
#define FID_RECV				5
#define	FID_RECV_FROM			6



int net_msg_receive(msg_t *m, uint16_t function_id);
int net_msg_reply(msg_t *m, msg_t *reply, uint16_t function_id);
int net_msg_send(msg_t *m, unsigned int pid, bool block, uint16_t function_id);
int net_msg_send_recv(msg_t *m, msg_t *reply, unsigned int pid, uint16_t function_id_m, uint16_t function_id_reply);

#endif /* MSG_HELP_H_ */
