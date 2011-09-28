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
#include "sys/net/destiny/socket.h"
#include "sys/net/destiny/in.h"
#include "sys/net/destiny/destiny.h"



uint8_t server_thread_pid;
char server_stack_buffer[2048];

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

	struct ipv6_hdr_t *test_ipv6_header = ((struct ipv6_hdr_t*)(&send_buffer));
	struct udp_hdr_t *test_udp_header = ((struct udp_hdr_t*)(&send_buffer[IPV6_HDR_LEN]));
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

    printf("Content of UDP Packet: src_port: %i, dst_port: %i, length: %i, checksum: %x\n", test_udp_header->src_port, test_udp_header->dst_port, test_udp_header->length, test_udp_header->checksum);
    sixlowpan_send(&ipaddr, (uint8_t*)(test_udp_header), test_udp_header->length, IPPROTO_UDP);
}

void send_udp(char *str)
	{
	int sock;
	struct sockaddr_in6 sa;
	ipv6_addr_t ipaddr;
	int bytes_sent;
	uint8_t text[20];
	sscanf(str, "send_udp %s", text);

	sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == sock)
		{
		printf("Error Creating Socket!");
		exit(EXIT_FAILURE);
		}

	memset(&sa, 0, sizeof sa);

	ipv6_init_address(&ipaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00, 0x0005);
	ipv6_print_addr(&ipaddr);

	sa.sin6_family = AF_INET;
	memcpy(&sa.sin6_addr, &ipaddr, 16);
	sa.sin6_port = 7654;

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

void init_tl (char *str)
	{
	init_transport_layer();
	}

void init_server(void)
	{
	int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);
	printf("Socket Number: %i\n", sock);
	struct sockaddr_in6 sa;
	char buffer[1024];
	ssize_t recsize;
	uint32_t fromlen;
	memset(&sa, 0, sizeof sa);
	sa.sin6_family = AF_INET;
	sa.sin6_port = 7654;
	fromlen = sizeof(sa);
	if (-1 == bind(sock, &sa, sizeof(sa), server_thread_pid))
		{
		printf("Error bind failed!\n");
		close(sock);
		}
	for (;;)
		{
		print_socket(sock);
		recsize = recvfrom(sock, (void *)buffer, 1024, 0, &sa, &fromlen);
		if (recsize < 0)
			{
			printf("ERROR: recsize < 0!\n");
			}
		printf("recsize: %i\n ", recsize);
		printf("datagram: %.*s\n", (int)recsize, buffer);
		}
	}

void init_server_thread(char *str)
	{
	server_thread_pid = thread_create(server_stack_buffer, 2048, PRIORITY_MAIN, CREATE_STACKTEST, init_server, "init_server");
	printf("SERVER THREAD PID: %i\n", server_thread_pid);
	}

const shell_command_t shell_commands[] = {
    //{"send", "", send_packet},
    {"init", "", init},
    {"addr", "", get_r_address},
    {"set_chann", "", set_radio_chann},
    {"boot", "", bootstrapping},
    {"ip", "", ip},
    {"context", "", context},
    {"init_tl", "", init_tl},
    {"init_server_thread", "", init_server_thread},
    {"send_udp", "", send_udp},
    {NULL, NULL, NULL}
};

int main(void) {
    printf("6LoWPAN Transport Layers\n");
    vtimer_init();
    posix_open(uart0_handler_pid, 0);
    shell_t shell;
    shell_init(&shell, shell_commands, uart0_readc, uart0_putc);
    shell_run(&shell);
    return 0;
}


