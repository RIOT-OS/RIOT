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
 * @ingroup      cpu_cc3200
 * @{
 *
 * @file
 * @brief           
 *
 * @author          Attilio Dona'
 */

#ifndef RIOT_CPU_CC3200_INCLUDE_SBAPP_PRIV_H_
#define RIOT_CPU_CC3200_INCLUDE_SBAPP_PRIV_H_

#include "simplelink.h"

/**
 * @brief describe a communication endpoint
 */
typedef struct cd_t {
	kernel_pid_t recv_pid;
	int16_t fd;
	kernel_pid_t pid;
	char *recv_stack;
	uint8_t conn_type; //< SOCK_STREAM (1) or SOCK_DGRAM (2)
	sockaddr_in addr;
	sockaddr_in local_addr;
} cd_t;


#endif /* RIOT_CPU_CC3200_INCLUDE_SBAPP_PRIV_H_ */
/** @} */
