/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <thread.h>
#include <vtimer.h>
#include <rtc.h>
#include <random.h>
#include <destiny.h>
#include <transceiver.h>
#include <sixlowpan/icmp.h>

#include <shell.h>
#include <posix_io.h>
#include <board_uart0.h>

#include <olsr2/olsr2.h>

#define IF_ID (0)

#if defined(USE_PID)
#include <unistd.h>
#include <sys/types.h>
static uint16_t get_node_id(void) {
	return getpid();
}
#else
#include <config.h>

static uint16_t get_node_id(void) {
	return sysconfig.id;
}
#endif

#ifdef ENABLE_NAME

static void ping(int argc, char **argv) {
	static uint16_t id = 0;

	if (argc < 2) {
		puts("usage: ping [node]");
		return;
	}

	id++;
	int packets = 10;

	ipv6_addr_t* dest = get_ip_by_name(argv[1]);
	if (dest == NULL) {
		printf("Unknown node: %s\n", argv[1]);
		return;
	}

	char addr_str[IPV6_MAX_ADDR_STR_LEN];
	ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, dest);

	uint8_t payload[] = "foobar";

	for (int i = 0; i < packets; ++i) {
		printf("sending %u bytes to %s\n", sizeof payload, addr_str);
		icmpv6_send_echo_request(dest, id, i, payload, sizeof payload);
		vtimer_usleep(1000000);
	}
}
#endif /* ENABLE_NAME */

static void set_id(int argc, char **argv) {
	if (argc < 2) {
		puts("usage: set_id [id] [name]");
		return;
	}

	uint16_t id = atoi(argv[1]);
	sysconfig.id = id;
	sysconfig.radio_address = (uint8_t) id;

#ifdef ENABLE_NAME
	if (argc > 2)
		strncpy(sysconfig.name, argv[2], CONFIG_NAME_LEN);
#endif
	config_save();
}

static void init(void) {
	ipv6_addr_t tmp;

	rtc_enable();
	genrand_init(get_node_id());
	net_if_set_hardware_address(IF_ID, get_node_id());

	ipv6_addr_set_link_local_prefix(&tmp);
	ipv6_addr_set_by_eui64(&tmp, IF_ID, &tmp);
	ipv6_net_if_add_addr(IF_ID, &tmp, NDP_ADDR_STATE_PREFERRED,
                              NDP_OPT_PI_VLIFETIME_INFINITE,
                              NDP_OPT_PI_PLIFETIME_INFINITE, 0);

	ipv6_addr_set_all_nodes_addr(&tmp);
	ipv6_net_if_add_addr(IF_ID, &tmp, NDP_ADDR_STATE_PREFERRED,
                              NDP_OPT_PI_VLIFETIME_INFINITE,
                              NDP_OPT_PI_PLIFETIME_INFINITE, 0);

	olsr_init();
}

const shell_command_t shell_commands[] = {
	{"routes", "print all known nodes and routes", print_topology_set},
	{"set_id", "set node ID and name", set_id},
#ifdef ENABLE_NAME
	{"ping", "send packets to a node", ping},
#endif
	{NULL, NULL, NULL}
};

int main(void) {
	init();

	posix_open(uart0_handler_pid, 0);

	shell_t shell;
	shell_init(&shell, shell_commands, UART0_BUFSIZE, uart0_readc, uart0_putc);

	shell_run(&shell);

	return 0;
}
