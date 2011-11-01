
#include <stdio.h>
#include <thread.h>
#include <string.h>

#include "udp.h"
#include "msg.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "socket.h"
#include "in.h"
#include "sys/net/net_help/net_help.h"

void printArrayRange_udp(uint8_t *array, uint16_t len)
	{
	int i = 0;
	printf("-------------MEMORY-------------\n");
	for (i = 0; i < len; i++)
		{
		printf("%#x ", *(array+i));
		}
	printf("-------------MEMORY-------------\n");
	}

uint16_t udp_csum(ipv6_hdr_t *ipv6_header, udp_hdr_t *udp_header)
	{
    uint16_t sum;
    uint16_t len = udp_header->length;

	sum = len + IPPROTO_UDP;
	sum = csum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
	sum = csum(sum, (uint8_t*)udp_header, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
	}

void udp_packet_handler(void)
	{
	msg_t m_recv_ip, m_send_ip, m_recv_udp, m_send_udp;
	ipv6_hdr_t *ipv6_header;
	udp_hdr_t *udp_header;
	uint8_t *payload;
	socket_internal_t *udp_socket = NULL;
	uint16_t chksum;

	while (1)
		{
		msg_receive(&m_recv_ip);
		printf("Inside UDP handler!\n");
		ipv6_header = ((ipv6_hdr_t*)&buffer_udp);
		udp_header = ((udp_hdr_t*)(&buffer_udp[IPV6_HDR_LEN]));
		payload = &buffer_udp[IPV6_HDR_LEN+UDP_HDR_LEN];

		chksum = udp_csum(ipv6_header, udp_header);

		if (chksum == 0xffff)
			{
			m_send_udp.content.ptr = (char*)buffer;
			m_send_udp.content.value = IPV6_HDR_LEN + UDP_HDR_LEN + udp_header->length;
			udp_socket = get_udp_socket(ipv6_header, udp_header);
			if (udp_socket != NULL)
				{
				msg_send_receive(&m_send_udp, &m_recv_udp, udp_socket->pid);
				}
			else
				{
				printf("Dropped UDP Message because no process ID was found for delivery!\n");
				}
			}
		else
			{
			printf("Wrong checksum (%x)!\n", chksum);
			}

		udp_socket = NULL;
		msg_reply(&m_recv_ip, &m_send_ip);
		}
	}


