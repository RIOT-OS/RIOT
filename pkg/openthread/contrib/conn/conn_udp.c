#include "net/conn/udp.h"
#include "conn.h"
#include "stdio.h"
#include <openthread.h>
#include "string.h"
#include <net/ipv6/addr.h>
#include "ot.h"

static msg_t _msg;

static conn_udp_msg_t msg_info;
static otSockAddr sockaddr = {};

char tmp[100];

void HandleUdpReceived(void *aContext, otMessage aMessage, const otMessageInfo *aMessageInfo)
{
	int msg_length, msg_offset;
	msg_offset = otGetMessageOffset(aMessage);
	msg_length = otGetMessageLength(aMessage)-msg_offset;
	otReadMessage(aMessage, msg_offset, tmp, msg_length);

	printf("Received UDP packet. Content: <");
	printf("%.*s", msg_length, tmp);
	printf(">\n");
	msg_info.message = aMessage;
	msg_info.message_info  = (otMessageInfo*) aMessageInfo;
	_msg.type = OPENTHREAD_MSG_TYPE_RECV;
	_msg.content.ptr = (void*) &msg_info;
	conn_udp_t *conn = (conn_udp_t*) aContext;
	msg_send(&_msg, conn->receiver_pid);	
}

int conn_udp_create(conn_udp_t *conn, const void *addr, size_t addr_len, int family, uint16_t port)
{
	memcpy(&sockaddr.mAddress.mFields, addr, addr_len);
    sockaddr.mPort = port;

    otOpenUdpSocket(&conn->mSocket, &HandleUdpReceived, conn);
    otBindUdpSocket(&conn->mSocket, &sockaddr);
    return 0;
}

int conn_udp_sendto(const void *data, size_t len, const void *src, size_t src_len,
                    const void *dst, size_t dst_len, int family, uint16_t sport,
                    uint16_t dport)
{
	if(dst_len != 16 || src_len != 16)
		return -1;

    otMessage message;
	otUdpSocket mSocket;

    message = otNewUdpMessage();
    otSetMessageLength(message, len);
    otWriteMessage(message, 0, data, len);

	otMessageInfo mPeer;

	//Set source address
	memcpy(&mPeer.mSockAddr.mFields, src, src_len);

	//Set source port
	mSocket.mSockName.mPort = 74;

	//Set dest address
	memcpy(&mPeer.mPeerAddr.mFields, dst, dst_len);

	//Set dest port
    mPeer.mPeerPort = dport;

	//Send UDP packet through OT
    otSendUdp(&mSocket, message, &mPeer);
	return 0;
}

void conn_udp_close(conn_udp_t *conn)
{
	otCloseUdpSocket(&conn->mSocket);
}

int conn_udp_getlocaladdr(conn_udp_t *conn, void *addr, uint16_t *port)
{
	memcpy(addr, &sockaddr.mAddress.mFields, 16);
    *port = sockaddr.mPort;
    return 16;
}

int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len, uint16_t *port)
{
	msg_t msg;
	conn->receiver_pid = thread_getpid();
	conn_udp_msg_t *cudp;
	int msg_length, msg_offset;
	while(1)
	{
		msg_receive(&msg);
		if(msg.type == OPENTHREAD_MSG_TYPE_RECV)
		{
			cudp = (conn_udp_msg_t*) msg.content.ptr;
			msg_offset = otGetMessageOffset(cudp->message);
			msg_length = otGetMessageLength(cudp->message)-msg_offset;
			otReadMessage(cudp->message, msg_offset, data, msg_length);
			memcpy(addr, &cudp->message_info->mPeerAddr.mFields, 16);
			*addr_len = 16;
			*port = cudp->message_info->mPeerPort;
			break;
		}
	}
	return 0;
}
