/*
 * tcp_timer.h
 *
 *  Created on: 21.01.2012
 *      Author: Oliver
 */

#ifndef TCP_TIMER_H_
#define TCP_TIMER_H_

#define SECONDS						1000*1000
#define TCP_TIMER_STACKSIZE			2048
#define TCP_SYN_INITIAL_TIMEOUT		6*SECONDS
#define TCP_SYN_TIMEOUT				24*SECONDS
#define TCP_MAX_SYN_RETRIES			3
#define TCP_ACK_TIMEOUT				3*SECONDS 	// still static, should be calculated via RTT
#define TCP_ACK_MAX_TIMEOUT			90*SECONDS

#define TCP_NOT_DEFINED				0
#define TCP_RETRY					1
#define TCP_TIMEOUT					2
#define TCP_CONTINUE				3

void tcp_general_timer(void);

char tcp_timer_stack[TCP_TIMER_STACKSIZE];

#endif /* TCP_TIMER_H_ */
