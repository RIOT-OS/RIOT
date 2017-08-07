/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 *
 * @addtogroup      cpu_cc3200
 * @{
 *
 * @file
 * @brief
 *
 * @author          Attilio Dona'
 */

#ifndef INCLUDE_OUTPUT_MESSAGES_H_
#define INCLUDE_OUTPUT_MESSAGES_H_

#ifdef DEVELHELP
#include <stdio.h>

// #define PANIC(NS, ERR_ID)  puts(sl_err_descr[ERR_ID]); while(1) {}
// #define PANIC2(NS, ERR_ID, ERR_VAL) printf("%s (%d)\n", sl_err_descr[ERR_ID], ERR_VAL); while(1) {}
// #define SIGNAL(NS, ERR_ID)  puts(sl_err_descr[ERR_ID])

#define PANIC(NS, ERR_ID)  printf("%s:%d fatal error: [%s:%d]\n", __FILE__, __LINE__, NS, ERR_ID); while(1) {}
#define PANIC2(NS, ERR_ID, ERR_VAL) printf("%s:%d fatal error: [%s:%d:%d] \n", __FILE__, __LINE__, NS, ERR_ID, ERR_VAL); while(1) {}
#define SIGNAL(NS, ERR_ID) printf("%s:%d error: [%s:%d]\n", __FILE__, __LINE__, NS, ERR_ID)

 #define PANIC_MESSAGE "critical fault: %s:%d\n", __FILE__, __LINE__

#else

/**
 * @brief A panic condition blocking the operations
 *
 * The critical error is signaled through morse signaling if the board is in a
 * status that permits GPIO functionality
 *
 * @param NS        Namespace. Short string that identify the context raising
 *                  The critical condition
 * @param ERR_ID    error code
 */
 #define PANIC(NS, ERR_ID)           morse(NS, ERR_ID, 0, 1)

/**
 * A panic condition blocking the operations
 *
 * The critical error is signaled through morse signaling if the board is in a
 * status that permits GPIO functionality
 *
 * @param NS        Namespace. Short string that identify the context raising
 *                  The critical condition
 * @param ERR_ID    The error code, integer
 * @param ERR_VAL   A value associated with ERR_ID, integer
 */
 #define PANIC2(NS, ERR_ID, ERR_VAL) morse(NS, ERR_ID, ERR_VAL, 1)

/**
 * @brief signal an event with morse signaling
 *
 * The critical error is signaled through morse signaling if the board is in
 * a status that permits GPIO functionality
 *
 * @param NS        Namespace. Short string that identify the context raising
 *                  The critical condition
 * @param ERR_ID    The error code, integer
 */
#define SIGNAL(NS, ERR_ID)          morse(NS, ERR_ID, 0, 0)

/**
 * @brief a generic and well know message
 *
 */
#define PANIC_MESSAGE "SOS"

#endif

/**
 * @brief stop the operations if an error is detected
 * @param NS        Namespace. Short string that identify the context raising
 *                  The critical condition
 * @param ERR_ID    The error code, integer
 * @param ERR_VAL   A value associated with ERR_ID, integer
 */
#define STOP_IF_ERR(NS, ERR_VAL, ERR_ID) if(ERR_VAL < 0) {PANIC2(NS, ERR_ID, ERR_VAL);}

/**
 * @brief morse signaling
 * @param namespace Short string that identify the context raising
 *                  The critical condition
 * @param err_code  The error code, integer
 * @param detail    A value associated with ERR_ID, integer
 * @param loop      a boolean. 1 loop on the morse message forever,
 *                  0 emit the message only once and then returns
 */
void morse(const char* namespace, int err_code, int detail, char loop);


#endif /* INCLUDE_OUTPUT_MESSAGES_H_ */
/** @} */
