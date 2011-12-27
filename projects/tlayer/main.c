#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>
#include <vtimer.h>
#include <ltc4150.h>
#include <thread.h>
#include <cc110x_ng.h>
#include <transceiver.h>
#include <time.h>
#include <rtc.h>
#include "sys/net/sixlowpan/sixlowmac.h"
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowborder.h"
#include "sys/net/sixlowpan/sixlowpan.h"
#include "sys/net/sixlowpan/sixlowerror.h"
#include "sys/net/destiny/udp.h"
#include "sys/net/destiny/tcp.h"
#include "sys/net/destiny/socket.h"
#include "sys/net/destiny/in.h"
#include "sys/net/destiny/destiny.h"
#include "sys/net/net_help/net_help.h"

uint8_t udp_server_thread_pid;
char udp_server_stack_buffer[UDP_STACK_SIZE];

uint8_t tcp_server_thread_pid;
char tcp_server_stack_buffer[TCP_STACK_SIZE];

uint8_t tcp_cht_pid;
char tcp_cht_stack_buffer[TCP_STACK_SIZE];

typedef struct tcp_msg_t
	{
	int		 	node_number;
	char		tcp_string_msg[50];
	}tcp_message_t;
tcp_message_t current_message;

void init_tl (char *str)
	{
	init_transport_layer();
	}

void tcp_ch(void)
	{
	uint32_t state = 0;
	msg_t recv_msg;
	sockaddr6_t stSockAddr;

	msg_receive(&recv_msg);

	int SocketFD = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);

	if (-1 == SocketFD)
		{
		printf("cannot create socket");
		}

	memset(&stSockAddr, 0, sizeof(stSockAddr));

	stSockAddr.sin6_family = AF_INET6;
	stSockAddr.sin6_port = HTONS(1100);
	ipv6_init_address(&stSockAddr.sin6_addr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, current_message.node_number);
	ipv6_print_addr(&stSockAddr.sin6_addr);

	if (-1 == connect(SocketFD, &stSockAddr, sizeof(stSockAddr), tcp_cht_pid))
		{
		printf("connect failed");
		close(SocketFD);
		}
	msg_receive(&recv_msg);
	state = recv_msg.content.value;
	while (state == 1)
		{
		printf("Trying to send data!\n");
		if (send(SocketFD, (void*) current_message.tcp_string_msg, strlen(current_message.tcp_string_msg)+1, 0) < 0)
			{
			printf("Could not send %s!\n", current_message.tcp_string_msg);
			}

		msg_receive(&recv_msg);
		state = recv_msg.content.value;
		}

	close(SocketFD);

	}

void init_udp_server(void)
	{
	sockaddr6_t sa;
	char buffer_main[256];
	ssize_t recsize;
	uint32_t fromlen;
	int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	memset(&sa, 0, sizeof sa);
	sa.sin6_family = AF_INET;
	sa.sin6_port = HTONS(7654);
	fromlen = sizeof(sa);
	if (-1 == bind(sock, &sa, sizeof(sa), udp_server_thread_pid))
		{
		printf("Error bind failed!\n");
		close(sock);
		}
	for (;;)
		{
		recsize = recvfrom(sock, (void *)buffer_main, 256, 0, &sa, &fromlen);
		if (recsize < 0)
			{
			printf("ERROR: recsize < 0!\n");
			}
		printf("recsize: %i\n ", recsize);
		printf("datagram: %s\n", buffer_main);
		}
	}

void init_tcp_server(void)
	{
	sockaddr6_t stSockAddr;
	uint8_t read_bytes;
	char buff_msg[MAX_TCP_BUFFER];
	int SocketFD = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);

	if(-1 == SocketFD)
		{
		perror("can not create socket");
		exit(EXIT_FAILURE);
		}

	memset(&stSockAddr, 0, sizeof(stSockAddr));

	stSockAddr.sin6_family = AF_INET6;
	stSockAddr.sin6_port = HTONS(1100);
	// TODO: HARDCODED! Use one of the IPv6 methods after initializing the node to get the correct ipv6 address!
	ipv6_init_address(&stSockAddr.sin6_addr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, get_radio_address());
	ipv6_print_addr(&stSockAddr.sin6_addr);

	if(-1 == bind(SocketFD, &stSockAddr, sizeof(stSockAddr), tcp_server_thread_pid))
		{
		perror("error bind failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
		}
	print_internal_socket(getSocket(SocketFD));
	if(-1 == listen(SocketFD, 10))
		{
		perror("error listen failed");
		close(SocketFD);
	 	exit(EXIT_FAILURE);
		}

	for(;;)
		{
		// Decide whether a new thread should be used to handle the new connection or the same (other queued
		// connections would not be handled!)
		int ConnectFD = accept(SocketFD, NULL, 0, tcp_server_thread_pid);
		if(0 > ConnectFD)
			{
			perror("error accept failed");
			close(SocketFD);
			exit(EXIT_FAILURE);
			}

		while (1)
			{
			read_bytes = recv(ConnectFD, buff_msg, MAX_TCP_BUFFER, 0);
			printf("--- Message: %s ---\n", buff_msg);
			}

		close(ConnectFD);
		}
	}

void init_udp_server_thread(char *str)
	{
	udp_server_thread_pid = thread_create(udp_server_stack_buffer, UDP_STACK_SIZE, PRIORITY_MAIN, CREATE_STACKTEST, init_udp_server, "init_udp_server");
	printf("UDP SERVER THREAD PID: %i\n", udp_server_thread_pid);
	}

void init_tcp_server_thread(char *str)
	{
	tcp_server_thread_pid = thread_create(tcp_server_stack_buffer, TCP_STACK_SIZE, PRIORITY_MAIN, CREATE_STACKTEST, init_tcp_server, "init_tcp_server");
	printf("TCP SERVER THREAD PID: %i\n", tcp_server_thread_pid);
	}

// Init TCP connection handler thread
void init_tcp_cht(char *str)
	{
	tcp_cht_pid = thread_create(		tcp_cht_stack_buffer,
											TCP_STACK_SIZE,
											PRIORITY_MAIN,
											CREATE_STACKTEST,
											tcp_ch,
											"init_connection_handler");
	printf("TCP CONNECTION HANDLER THREAD PID: %i\n", tcp_cht_pid);
	}

void send_tcp_msg(char *str)
	{
	msg_t send_msg;
	sscanf(str, "send_tcp %s", current_message.tcp_string_msg);
	send_msg.content.value = 1;
	msg_send(&send_msg, tcp_cht_pid, 0);
	}

void connect_tcp(char *str)
	{
	msg_t send_msg;
	sscanf(str, "connect_tcp %i", &current_message.node_number);

	send_msg.content.value = 1;
	msg_send(&send_msg, tcp_cht_pid, 0);
	}

void disconnect_tcp(char *str)
	{
	msg_t send_msg;

	send_msg.content.value = 0;
	msg_send(&send_msg, tcp_cht_pid, 0);
	}

void init(char *str){
    char command;
    uint16_t r_addr;
    ipv6_addr_t std_addr;

    int res = sscanf(str, "init %c %hu", &command, &r_addr);

    if(res < 1){
        printf("Usage: init {h | r | a | e} radio_address\n");
        printf("\th\tinitialize as host\n");
        printf("\tr\tinitialize as router\n");
        printf("\ta\tinitialize as ad-hoc router\n");
        printf("\tb\tinitialize as border router\n\n");
        printf("\tradio_address must be an 8 bit integer\n");
    }

    ipv6_init_address(&std_addr,0xABCD,0,0,0,0x1034,0x00FF,0xFE00,r_addr);

    switch (command) {
        case 'h':
            printf("INFO: Initialize as host on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            sixlowpan_init(TRANSCEIVER_CC1100,r_addr,0);
            break;
        case 'r':
            printf("INFO: Initialize as router on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            sixlowpan_init(TRANSCEIVER_CC1100, r_addr,0);
            ipv6_init_iface_as_router();
            break;
        case 'a':
            printf("INFO: Initialize as adhoc router on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            sixlowpan_adhoc_init(TRANSCEIVER_CC1100, &std_addr, r_addr);
            break;
        case 'b':
            printf("INFO: Initialize as border router on radio address %hu\n", r_addr);
            if (r_addr > 255) {
                printf("ERROR: radio_address not an 8 bit integer\n");
                return;
            }
            res = border_initialize(TRANSCEIVER_CC1100, &std_addr);
            switch (res) {
                case (SUCCESS): printf("INFO: Border router initialized.\n"); break;
                case (SIXLOWERROR_ADDRESS): printf("ERROR: Illegal IP address: ");
                        ipv6_print_addr(&std_addr); break;
                default: printf("ERROR: Unknown error (%d).\n", res); break;
            }
            break;
        default:
            printf("ERROR: Unknown command '%c'\n", command);
            break;
    }

}

void bootstrapping(char *str){
    sixlowpan_bootstrapping();
}

void send_packet(char *str){
	uint8_t send_buffer[UDP_STACK_SIZE];
	uint8_t text[20];
	sscanf(str, "send %s", text);

	ipv6_hdr_t *test_ipv6_header = ((ipv6_hdr_t*)(&send_buffer));
	udp_hdr_t *test_udp_header = ((udp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));
	uint8_t *payload = &send_buffer[IPV6_HDR_LEN+UDP_HDR_LEN];

	ipv6_addr_t ipaddr;
	ipv6_init_address(&ipaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, 0x0005);
	ipv6_print_addr(&ipaddr);

	memcpy(&(test_ipv6_header->destaddr), &ipaddr, 16);
	ipv6_get_saddr(&(test_ipv6_header->srcaddr), &(test_ipv6_header->destaddr));
	test_ipv6_header->version_trafficclass = IPV6_VER;
	test_ipv6_header->trafficclass_flowlabel = 0;
	test_ipv6_header->flowlabel = 0;
	test_ipv6_header->nextheader = IPPROTO_UDP;
	test_ipv6_header->hoplimit = MULTIHOP_HOPLIMIT;
	test_ipv6_header->length = sizeof(test_udp_header);

    test_udp_header->src_port = 9821;
    test_udp_header->dst_port = 7654;
    test_udp_header->checksum = 0;

    memcpy(payload, text, strlen((char*)text)+1);
    test_udp_header->length = 8 + strlen((char*)text)+1;

    test_udp_header->checksum = ~udp_csum(test_ipv6_header, test_udp_header);

    sixlowpan_send(&ipaddr, (uint8_t*)(test_udp_header), test_udp_header->length, IPPROTO_UDP);
}

void send_udp(char *str)
	{
	int sock;
	sockaddr6_t sa;
	ipv6_addr_t ipaddr;
	int bytes_sent;
	int address;
	uint8_t text[20];
	sscanf(str, "send_udp %s %i", text, &address);

	sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
		{
		printf("Error Creating Socket!");
		exit(EXIT_FAILURE);
		}

	memset(&sa, 0, sizeof sa);

	ipv6_init_address(&ipaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, (uint16_t)address);
	ipv6_print_addr(&ipaddr);

	sa.sin6_family = AF_INET;
	memcpy(&sa.sin6_addr, &ipaddr, 16);
	sa.sin6_port = HTONS(7654);

	bytes_sent = sendto(sock, (char*)text, strlen((char*)text)+1, 0, &sa, sizeof sa);
	if (bytes_sent < 0)
		{
		printf("Error sending packet!\n");
		}

	close(sock);
	}

void set_radio_chann(char *str){
    uint16_t chann;
    int res = sscanf(str, "set_chann %hu", &chann);
    if(res < 1){
        printf("Usage: set_chann [channel]\n");
    }

    cc110x_set_channel(chann);
}

void get_r_address(char *str){
    printf("radio: %hu\n", cc110x_get_address());
}

void ip(char *str){
    ipv6_iface_print_addrs();
}

void context(char *str){
    uint8_t i;
    lowpan_context_t *context;

    for(i = 0; i < LOWPAN_CONTEXT_MAX; i++){
        context = lowpan_context_num_lookup(i);
        if (context != NULL) {
            printf("%2d\tLifetime: %5u\tLength: %3d\t",context->num,context->lifetime,context->length);
            ipv6_print_addr(&(context->prefix));
        }
    }
}

void shows(char *str)
	{
	print_sockets();
	}

void showReas(char *str)
	{
	printReasBuffers();
	}

const shell_command_t shell_commands[] = {
    {"init", "", init},
    {"addr", "", get_r_address},
    {"set_chann", "", set_radio_chann},
    {"boot", "", bootstrapping},
    {"ip", "", ip},
    {"shows", "Show Sockets", shows},
    {"show_reas", "Show reassembly Buffers", showReas},
    {"context", "", context},
    {"init_tl", "", init_tl},
    {"init_udp_server_thread", "", init_udp_server_thread},
    {"init_tcp_server_thread", "", init_tcp_server_thread},
    {"init_tcp_cht", "", init_tcp_cht},
    {"connect_tcp", "", connect_tcp},
    {"send_tcp", "", send_tcp_msg},
    {"send_udp", "", send_udp},
    {NULL, NULL, NULL}
};

int main(void) {
    printf("6LoWPAN Transport Layers\n");
    posix_open(uart0_handler_pid, 0);
    init_tl(NULL);
    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);

    shell_run(&shell);

    return 0;
}
