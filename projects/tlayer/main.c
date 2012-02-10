#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>

#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>
#include "hwtimer.h"
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
#include "sys/net/sixlowpan/sixlownd.h"
#include "sys/net/destiny/udp.h"
#include "sys/net/destiny/tcp.h"
#include "sys/net/destiny/socket.h"
#include "sys/net/destiny/in.h"
#include "sys/net/destiny/destiny.h"
#include "sys/net/destiny/tcp_timer.h"
#include "sys/net/net_help/net_help.h"
#include "sys/net/net_help/msg_help.h"

#define SEND_TCP_THREAD_SIZE		3072
#define TCP_CLOSE_THREAD_STACK_SIZE		3072

uint8_t udp_server_thread_pid;
char udp_server_stack_buffer[UDP_STACK_SIZE];

uint8_t tcp_server_thread_pid;
char tcp_server_stack_buffer[TCP_STACK_SIZE];

uint8_t tcp_cht_pid;
char tcp_cht_stack_buffer[TCP_STACK_SIZE];

// Socket ID used for sending/receiving packets via different threads
int tcp_socket_id = -1;

uint8_t tcp_send_pid = -1;
char tcp_send_stack_buffer[SEND_TCP_THREAD_SIZE];

char tcp_close_thread_stack[TCP_CLOSE_THREAD_STACK_SIZE];

static msg_t mesg;
static transceiver_command_t tcmd;

typedef struct tcp_msg_t
	{
	int		 	node_number;
	char		tcp_string_msg[80];
	}tcp_message_t;
tcp_message_t current_message;

void init_tl (char *str)
	{
	init_transport_layer();
	}

void tcp_ch(void)
	{
	msg_t recv_msg;
	int read_bytes = 0;
	char buff_msg[MAX_TCP_BUFFER];
	sockaddr6_t stSockAddr;
	msg_receive(&recv_msg);
	int SocketFD = socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (-1 == SocketFD)
		{
		printf("cannot create socket");
		return;
		}
	memset(&stSockAddr, 0, sizeof(stSockAddr));

	stSockAddr.sin6_family = AF_INET6;
	stSockAddr.sin6_port = HTONS(1100);

	ipv6_init_address(&stSockAddr.sin6_addr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, current_message.node_number);
	ipv6_print_addr(&stSockAddr.sin6_addr);

	if (-1 == connect(SocketFD, &stSockAddr, sizeof(stSockAddr)))
		{
		printf("Connect failed!\n");
		close(SocketFD);
		return;
		}
	tcp_socket_id = SocketFD;
	while (read_bytes != -1)
		{
		read_bytes = recv(SocketFD, buff_msg, MAX_TCP_BUFFER, 0);
		if (read_bytes > 0)
			{
			printf("--- Message: %s ---\n", buff_msg);
			}

		}
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
	if (-1 == bind(sock, &sa, sizeof(sa)))
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
	int read_bytes;
	char buff_msg[MAX_TCP_BUFFER];
	memset(buff_msg, 0, MAX_TCP_BUFFER);
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

	if(-1 == bind(SocketFD, &stSockAddr, sizeof(stSockAddr)))
		{
		printf("error bind failed\n");
		close(SocketFD);
		return;
		}
	print_internal_socket(getSocket(SocketFD));
	if(-1 == listen(SocketFD, 10))
		{
		printf("error listen failed\n");
		close(SocketFD);
	 	return;
		}
	while (1)
		{
		read_bytes = 0;
		printf("INFO: WAITING FOR INC CONNECTIONS!\n");
		int ConnectFD = accept(SocketFD, NULL, 0);
		if(0 > ConnectFD)
			{
			printf("error accept failed\n");
			close(SocketFD);
			return;
			}
		tcp_socket_id = ConnectFD;
		while (read_bytes != -1)
			{
			read_bytes = recv(ConnectFD, buff_msg, MAX_TCP_BUFFER, 0);

			if (read_bytes > 0)
				{
				printf("--- Read bytes: %i, Strlen(): %i, Message: %s ---\n", read_bytes, strlen(buff_msg), buff_msg);
				}
			}
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
											"init_conn_handler");
	printf("TCP CONNECTION HANDLER THREAD PID: %i\n", tcp_cht_pid);
	}

void send_tcp_thread (void)
	{
	msg_t recv_msg, send_msg;
	while (1)
		{
		msg_receive(&recv_msg);
		if (send(tcp_socket_id, (void*) current_message.tcp_string_msg, strlen(current_message.tcp_string_msg)+1, 0) < 0)
			{
			printf("Could not send %s!\n", current_message.tcp_string_msg);
			}
		msg_reply(&recv_msg, &send_msg);
		}
	}

void send_tcp_msg(char *str)
	{
	msg_t send_msg, recv_msg;
	sscanf(str, "send_tcp %s", current_message.tcp_string_msg);
//	printf("Message: %s\n", current_message.tcp_string_msg);
	if (strcmp(current_message.tcp_string_msg, "close") == 0)
		{
		send_msg.content.value = 0;
		}
	else
		{
		send_msg.content.value = 1;
		}
	msg_send_receive(&send_msg, &recv_msg, tcp_send_pid);
	}

void send_tcp_bulk(char *str)
	{
	int i = 0, count;
	char command[61];
	char msg_string[50];
	sscanf(str, "send_tcp_bulk %i %s", &count, msg_string);
	for (i = 0; i < count; i++)
		{
		sprintf(command, "send_tcp %s%.5i", msg_string, i);
		send_tcp_msg(command);
		}
	}

void send_tcp_bandwidth_test(char *str)
	{
	timex_t start, end, total;
	uint32_t secs;

	int i = 0, count;
	char command[80];
	char msg_string[] = "abcdefghijklmnopqrstuvwxyz0123456789!-=/%$";

	sscanf(str, "tcp_bw %i", &count);
	start = vtimer_now();
	for (i = 0; i < count; i++)
		{
		sprintf(command, "send_tcp %s%.5i", msg_string, i);
		send_tcp_msg(command);
		}
	end = vtimer_now();
	total = timex_sub(end, start);
	secs = total.microseconds / 1000000;
	printf("Start: %lu, End: %lu, Total: %lu\n", start.microseconds, end.microseconds, total.microseconds);
	secs = total.microseconds / 1000000;
	printf("Time: %lu seconds, Bandwidth: %lu byte/second\n", secs, (count*48)/secs);
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
    tcp_send_pid = thread_create(	tcp_send_stack_buffer,
									SEND_TCP_THREAD_SIZE,
									PRIORITY_MAIN,
									CREATE_STACKTEST,
									send_tcp_thread,
									"send_tcp_thread");

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
	timex_t start, end, total;
	float secs;
	int sock;
	sockaddr6_t sa;
	ipv6_addr_t ipaddr;
	int bytes_sent;
	int address, count;
	uint8_t text[] = "abcdefghijklmnopqrstuvwxyz0123456789!-=$%&/()";
	sscanf(str, "send_udp %i %i", &count, &address);

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
	start = vtimer_now();
	for (int i = 0; i < count; i++)
		{
		bytes_sent = sendto(sock, (char*)text, strlen((char*)text)+1, 0, &sa, sizeof sa);
		if (bytes_sent < 0)
			{
			printf("Error sending packet!\n");
			}
		}
	end = vtimer_now();
	total = timex_sub(end, start);
	printf("Start: %lu, End: %lu, Total: %lu\n", start.microseconds, end.microseconds, total.microseconds);
	secs = total.microseconds / 1000000;
	printf("Time: %f seconds, Bandwidth: %f byte/second\n", secs, (count*48)/secs);
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

void kill_process(char *str)
	{
	msg_t send;
	int mypid;
	send.type = RETURNNOW;
	sscanf(str, "kill_process %i", &mypid);
	msg_send(&send, mypid, 0);
	}

void continue_process(char *str)
	{
	msg_t send;
	int pid;
	sscanf(str, "continue_process %i", &pid);
	send.type = TCP_CONTINUE;
	msg_send(&send, pid, 0);
	}

void close_tcp_thread(void)
	{
	close(tcp_socket_id);
	}

void close_tcp (char *str)
	{
	thread_create(tcp_close_thread_stack, TCP_CLOSE_THREAD_STACK_SIZE, PRIORITY_MAIN,
			CREATE_STACKTEST, close_tcp_thread, "tcp_close_thread");
	}

void boot_server(char *str)
	{
	bootstrapping(NULL);
	vtimer_usleep(1000*1000*2);
	init_tcp_server_thread(NULL);
	}

void boot_client(char *str)
	{
	init_tcp_cht(NULL);
	vtimer_usleep(1000*1000*2);
	connect_tcp("connect_tcp 2");
	}

void show_nbr_cache(char *str)
	{
	print_nbr_cache();
	}

#ifdef DBG_IGNORE
void ignore(char *addr) {
    uint16_t a;
    mesg.type = DBG_IGN;
    mesg.content.ptr = (char*) &tcmd;

    tcmd.transceivers = TRANSCEIVER_CC1100;
    tcmd.data = &a;
    if (sscanf(addr, "ign %hu", &a) == 1) {
        msg_send(&mesg, transceiver_pid, 1);
    }
    else {
        puts("Usage:\tign <addr>");
    }
}
#endif

const shell_command_t shell_commands[] = {
    {"init", "", init},
    {"addr", "", get_r_address},
    {"set_chann", "", set_radio_chann},
    {"boot", "", bootstrapping},
    {"ip", "", ip},
    {"shows", "Show Sockets", shows},
    {"show_reas", "Show reassembly Buffers", showReas},
    {"context", "", context},
    {"init_udp_server_thread", "", init_udp_server_thread},
    {"init_tcp_server_thread", "", init_tcp_server_thread},
    {"init_tcp_cht", "", init_tcp_cht},
    {"connect_tcp", "", connect_tcp},
    {"send_tcp", "", send_tcp_msg},
    {"send_udp", "", send_udp},
    {"send_tcp_bulk", "send_tcp_bulk NO_OF_PACKETS PAYLOAD", send_tcp_bulk},
    {"kill_process", "", kill_process},
    {"continue_process", "", continue_process},
    {"close_tcp", "", close_tcp},
    {"tcp_bw", "tcp_bw NO_OF_PACKETS", send_tcp_bandwidth_test},
    {"boots", "", boot_server},
    {"bootc", "", boot_client},
    {"print_nbr_cache", "", show_nbr_cache},
#ifdef DBG_IGNORE
    {"ign", "ignore node", ignore},
#endif
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
