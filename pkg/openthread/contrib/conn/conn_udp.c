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

void HandleUdpReceived(void *aContext, otMessage aMessage, const otMessageInfo *aMessageInfo)
{
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
	if(dst_len != 16)
		return -1;
    otMessage message;
	otUdpSocket mSocket;

    ipv6_addr_t addr;
    const char *addr_str = dst;
    ipv6_addr_from_str(&addr, addr_str);

    otSockAddr peer = {};
    peer.mPort = dport;

	//Set dest addr
	memcpy(&peer.mAddress.mFields, &addr, 16);

    message = otNewUdpMessage();
    otSetMessageLength(message, len);
    otWriteMessage(message, 0, data, len);

	otMessageInfo mPeer;

	//Set source address
	mPeer.mSockAddr = mSocket.mSockName.mAddress;
	mPeer.mSockAddr.mFields.m8[0] = 0x69;

	//Set source port
	mSocket.mSockName.mPort = 74;

	//Set dest address
    mPeer.mPeerAddr = peer.mAddress;

	//Set dest port
    mPeer.mPeerPort = peer.mPort;

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
	/*otSockAddr address = conn->mSocket.mSockName;
	printf("\n");
    for(int i=0;i<16;i++)
    {
		printf("%i ",address.mAddress.mFields.m8[i]);
    }
	printf("\n");*/
    return 0;
}

int conn_udp_recvfrom(conn_udp_t *conn, void *data, size_t max_len, void *addr, size_t *addr_len, uint16_t *port)
{
	msg_t msg;
	conn->receiver_pid = thread_getpid();
	conn_udp_msg_t *cudp;
	int msg_length, msg_offset;;
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
