/***********************************************
 * This file is for easy testing on Linux only *
 ***********************************************/

#ifndef RIOT
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "rfc5444/rfc5444_writer.h"

#include "constants.h"
#include "debug.h"
#include "node.h"
#include "olsr.h"
#include "reader.h"
#include "writer.h"

int sockfd;
struct sockaddr_in servaddr;

sigset_t block_io;

struct ip_lite {
	struct netaddr src;
	size_t length;
};

void write_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
	struct rfc5444_writer_target *iface __attribute__((unused)),
	void *buffer, size_t length) {

	DEBUG("write_packet(%zd bytes)", length);

	struct ip_lite* new_buffer = malloc(sizeof(struct ip_lite) + length);
	memcpy(new_buffer + 1, buffer, length);
	memcpy(&new_buffer->src, get_local_addr(), sizeof(struct netaddr));
	new_buffer->length = length;

	sendto(sockfd, new_buffer, sizeof(struct ip_lite) + new_buffer->length, 0,
		(struct sockaddr*) &servaddr, sizeof(servaddr));
}

void enable_receive(void) {
	sigprocmask (SIG_UNBLOCK, &block_io, NULL);
}

void disable_receive(void) {
	sigprocmask (SIG_BLOCK, &block_io, NULL);
}

void sigio_handler(int sig __attribute__((unused))) {
	char buffer[1500];
	struct sockaddr_storage sender;
	socklen_t sendsize = sizeof(sender);

	disable_receive();

	while (recvfrom(sockfd, &buffer, sizeof buffer, 0, (struct sockaddr*)&sender, &sendsize) > 0) {
		struct ip_lite* header = (struct ip_lite*) &buffer;

		reader_handle_packet(header + 1, header->length, &header->src);
	}
	enable_receive();
}

void init_socket(in_addr_t addr, int port) {
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&servaddr, 0, sizeof servaddr);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = addr;
	servaddr.sin_port = htons(port);
}

int enable_asynch(int sock) {
	int flags;
	struct sigaction sa;

	flags = fcntl(sock, F_GETFL);
	fcntl(sock, F_SETFL, flags | O_ASYNC | O_NONBLOCK);

	sa.sa_flags = 0;
	sa.sa_handler = sigio_handler;
	sigemptyset(&sa.sa_mask);

	if (sigaction(SIGIO, &sa, NULL))
		return -1;

	if (fcntl(sock, F_SETOWN, getpid()) < 0)
		return -1;

	if (fcntl(sock, F_SETSIG, SIGIO) < 0)
		return -1;

	return 0;
}

int main(int argc, char** argv) {
	const char* this_ip;

	if (argc != 4) {
		printf("usage:  %s <server IP address> <port> <node IP6 address>\n", argv[0]);
		return -1;
	}

	this_ip = argv[3];

	init_socket(inet_addr(argv[1]), atoi(argv[2]));

	/* set timeout for name probing */
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	DEBUG("probing for name…");
	/* send HELLO */
	sendto(sockfd, this_ip, strlen(this_ip), 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
	/* get our name */
	char this_name[32];
	ssize_t size = recvfrom(sockfd, this_name, sizeof this_name, 0, 0, 0);
	if (size < 0) {
		strcpy(this_name, "(null)");
	} else
		this_name[size] = 0;

#ifdef ENABLE_NAME
	local_name = strdup(this_name);
#endif

	node_init();

	get_local_addr()->_type = AF_INET6;
	get_local_addr()->_prefix_len = 128;
	inet_pton(AF_INET6, this_ip, get_local_addr()->_addr);

	DEBUG("This is node %s with IP %s",
		local_name, netaddr_to_str_s(&nbuf[0], get_local_addr()));

	reader_init();
	writer_init(write_packet);

	/* Initialize the signal mask. */
	sigemptyset (&block_io);
	sigaddset (&block_io, SIGIO);

	enable_asynch(sockfd);

	bool dont_skip_tc = true;
	while (1) {
		/* TC messages have a longer interval */
		dont_skip_tc = !dont_skip_tc;
		sleep_s(HELLO_REFRESH_INTERVAL);

		disable_receive();

		// print_neighbors();
		print_topology_set();
		print_routing_graph();

		writer_send_hello();
		if (dont_skip_tc)
			writer_send_tc();

		DEBUG_TICK;
		enable_receive();
	}

	reader_cleanup();
	writer_cleanup();

	return 0;
}

#endif /* no RIOT */
