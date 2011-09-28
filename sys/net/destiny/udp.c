
#include <stdio.h>
#include <thread.h>
#include <string.h>

#include "udp.h"
#include "msg.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "socket.h"
#include "in.h"

void udp_packet_handler(void)
	{
	msg_t m_recv_ip, m_send_ip, m_recv_udp, m_send_udp;
	struct ipv6_hdr_t *ipv6_header;
	struct udp_hdr_t *udp_header;
	uint8_t *payload;
	int udp_process = 0;

	while (1)
		{
		msg_receive(&m_recv_ip);
		ipv6_header = ((struct ipv6_hdr_t*)&buffer);
		udp_header = ((struct udp_hdr_t*)(&buffer[IPV6_HDR_LEN]));
		payload = &buffer[IPV6_HDR_LEN+UDP_HDR_LEN];

		printf("Checksum is %x\n", (udp_csum(ipv6_header, udp_header)));
		m_send_udp.content.ptr = (char*)buffer;
		m_send_udp.content.value = IPV6_HDR_LEN + UDP_HDR_LEN + udp_header->length;
		udp_process = get_udp_process(udp_header->dst_port);
		printf("UDP_PROCESS: %i\n", udp_process);
		if (udp_process != -1)
			{
			msg_send_receive(&m_send_udp, &m_recv_udp, udp_process);
			}
		else
			{
			printf("Dropped UDP Message because no process ID was found for delivery!\n");
			}
		udp_process = -1;
		msg_reply(&m_recv_ip, &m_send_ip);
		}
	}

uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header){
    uint16_t sum;
    uint16_t len = udp_header->length;

	sum = len + IPPROTO_UDP;
	sum = checksum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
	sum = checksum(sum, (uint8_t*)udp_header, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
}

void printArrayRange(uint8_t *udp_header, uint16_t len)
	{
	int i = 0;
	printf("-------------MEMORY-------------\n");
	for (i = 0; i < len; i++)
		{
		printf("%#x ", *(udp_header+i));
		}
	printf("-------------MEMORY-------------\n");
	}

// TODO: Global file for commonly used functions like checksum
uint16_t checksum(uint16_t sum, uint8_t *buf, uint16_t len)
	{
    int count;
    uint16_t carry;

    count = len >> 1;
    if(count)
		{
        if(count)
			{
            carry = 0;
            do
				{
                uint16_t t = (*buf << 8) + *(buf+1);
                count--;
                buf += 2;
                sum += carry;
                sum += t;
                carry = (t > sum);
				} while(count);
            sum += carry;
			}
		}
    if(len & 1)
		{
        uint16_t u = (*buf << 8);
        sum += (*buf << 8);
        if(sum < u)
			{
            sum++;
			}
		}

    return sum;
	}
