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
