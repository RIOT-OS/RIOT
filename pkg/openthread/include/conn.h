/*
 * Copyright (C) 2016 José Ignacio Alamos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_openthread_conn   Connection type definitions for OpenThread
 * @ingroup     pkg_openthread
 * @{
 *
 * @file
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */
#ifndef OT_CONN_H
#define OT_CONN_H

#include <openthread.h>
#include "thread.h"
#ifdef __cplusplus
extern "C" {
#endif


struct conn_udp {
	otUdpSocket mSocket;
	otMessageInfo mPeer;
	kernel_pid_t receiver_pid;
};

typedef struct conn_udp_msg_t {
	otMessage message;
	otMessageInfo *message_info;
} conn_udp_msg_t;

#ifdef __cplusplus
}
#endif
#endif
/** @} */
