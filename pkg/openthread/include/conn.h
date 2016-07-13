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

#ifdef __cplusplus
}
#endif
#endif
