/*
 * Copyright (C) 2017 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */


/**
 *
 * @file
 *
 *
 * @defgroup     sbapp sbapp
 * @{
 *
 * @brief       Provide an interface to the CC3200 network processor
 *
 * @author      Attilio Dona' <attilio.dona>
 *
 */

#ifndef SYS_INCLUDE_TCP_CC3200_H_
#define SYS_INCLUDE_TCP_CC3200_H_

#include "sbapp.h"

/**
 * @brief connect to a tcp server
 * @param hostname the server hostname or ip address
 * @param server_port
 * @return the connection handler
 */
int16_t tcp_connect(const char *hostname, uint16_t server_port);

#endif /* SYS_INCLUDE_TCP_CC3200_H_ */

/** @} */

