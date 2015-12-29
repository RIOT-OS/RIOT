/*
 * t_debug.h
 *
 *  Created on: 14 gru 2015
 *      Author: mateusz
 */

#ifndef TESTS_GNRC_NETWORKING_CC112X_T_DEBUG_H_
#define TESTS_GNRC_NETWORKING_CC112X_T_DEBUG_H_

extern struct timeval time_ref;
extern char time_stamp[20];

void debug_timeref_init(void);
void debug_timestamp(void);

#endif /* TESTS_GNRC_NETWORKING_CC112X_T_DEBUG_H_ */
