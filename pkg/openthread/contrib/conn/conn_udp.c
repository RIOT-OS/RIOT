#include "net/conn/udp.h"
#include "conn.h"
#include "stdio.h"
#include <openthread.h>
#include <net/ipv6/addr.h>
#include "ot.h"

static msg_t _msg;

void HandleUdpReceived(void *aContext, otMessage aMessage, const otMessageInfo *aMessageInfo)
{
    printf("Received UDP message. Send message to thread\n");
	_msg.type = OPENTHREAD_MSG_TYPE_RECV;
	conn_udp_t *conn = (conn_udp_t*) aContext;
	msg_send(&_msg, conn->receiver_pid);	
}

int conn_udp_create(conn_udp_t *conn, const void *addr, size_t addr_len, int family, uint16_t port)
{
    otSockAddr sockaddr = {};
    sockaddr.mPort = port;

    otOpenUdpSocket(&conn->mSocket, &HandleUdpReceived, conn);
    otBindUdpSocket(&conn->mSocket, &sockaddr);
    return 0;
}

int conn_udp_sendto(const void *data, size_t len, const void *src, size_t src_len,
                    const void *dst, size_t dst_len, int family, uint16_t sport,
                    uint16_t dport)
{
    otMessage message;
	otUdpSocket mSocket;

    ipv6_addr_t addr;
    const char *addr_str = dst;
    ipv6_addr_from_str(&addr, addr_str);

    otSockAddr peer = {};
    peer.mPort = dport;

    for(int i=0;i<16;i++)
    {
        peer.mAddress.mFields.m8[i] = addr.u8[i];
    }
    mSocket.mPeerName = peer;

    message = otNewUdpMessage();
    otSetMessageLength(message, len);
    otWriteMessage(message, 0, data, len);

	otMessageInfo mPeer;
    mPeer.mPeerAddr = peer.mAddress;
    mPeer.mPeerPort = peer.mPort;
    otSendUdp(&mSocket, message, &mPeer);
	return 0;
}

/*void conn_udp_close(conn_udp_t *conn)
int conn_udp_getlocaladdr(conn_udp_t *conn, void *addr, uint16_t *port)
    return conn->local_addr_len;*/

int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len, uint16_t *port)
{
	msg_t msg;
	conn->receiver_pid = thread_getpid();
	while(1)
	{
		msg_receive(&msg);
		if(msg.type == OPENTHREAD_MSG_TYPE_RECV)
		{
			break;
		}
	}
	printf("Received message! :)\n");
	return 0;
}
