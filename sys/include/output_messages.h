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
 * @addtogroup      <module_name>
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

 #define PANIC(NS, ERR_ID)  puts(sl_err_descr[ERR_ID]); while(1) {}
 #define PANIC2(NS, ERR_ID, ERR_VAL) printf("%s (%d)\n", sl_err_descr[ERR_ID], ERR_VAL); while(1) {}
 #define SIGNAL(NS, ERR_ID)  puts(sl_err_descr[ERR_ID])

 #define PANIC_MESSAGE "critical fault: %s:%d\n", __FILE__, __LINE__

#else

 #define PANIC(NS, ERR_ID)           morse(NS, ERR_ID, 0)
 #define PANIC2(NS, ERR_ID, ERR_VAL) morse(NS, ERR_ID, ERR_VAL)
 #define SIGNAL(NS, ERR_ID)          morse(NS, ERR_ID, 0)

 #define PANIC_MESSAGE "SOS"

#endif

#define STOP_IF_ERR(NS, ERR_VAL, ERR_ID) if(ERR_VAL < 0) {PANIC2(NS, ERR_ID, ERR_VAL);}

void morse(const char* namespace, int err_code, int detail);


#endif /* INCLUDE_OUTPUT_MESSAGES_H_ */
/** @} */
