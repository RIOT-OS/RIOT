#ifndef OT_CONN_H
#define OT_CONN_H

#ifdef __cplusplus
extern "C" {
#endif


struct conn_udp {
	static otUdpSocket mSocket;
	static otMessageInfo mPeer;
};

#ifdef __cplusplus
}
#endif
#endif
