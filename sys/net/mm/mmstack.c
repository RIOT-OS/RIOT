/******************************************************************************
Copyright 2009, Freie Universitaet Berlin (FUB). All rights reserved.

These sources were developed at the Freie Universitaet Berlin, Computer Systems
and Telematics group (http://cst.mi.fu-berlin.de).
-------------------------------------------------------------------------------
This file is part of RIOT.

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

RIOT is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see http://www.gnu.org/licenses/ .
--------------------------------------------------------------------------------
For further information and questions please use the web site
	http://scatterweb.mi.fu-berlin.de
and the mailinglist (subscription via web site)
	scatterweb@lists.spline.inf.fu-berlin.de
*******************************************************************************/

#include "configure.h"
#include "mmstack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utimer.h"
#include "kernel.h"
#include "thread.h"
#include "msg.h"

#include "net.h"
#include "mmr.h"
#include "trans.h"
#include "clock.h"
#include "cmdengine.h"

/**
 * @file
 * @brief
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author 		Thomas Hillebrandt <hillebra@inf.fu-berlin.de>
 * @version     $Revision: 3854 $
 *
 * @note		$Id: mmstack.c 3854 2011-12-06 15:27:01Z hwill $
 */

/*---------------------------------------------------------------------------*/
//				  Data structures for build in ping
/*---------------------------------------------------------------------------*/

#define MMS_PING_PACKETS		(4)		///< Default number of packets in a ping
#define MMS_PING_ECHO_REQUEST	(8)		///< Ping type: echo request
#define MMS_PING_ECHO_REPLY		(0)		///< Ping type: echo reply

/**
 * Represents a ping message.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |   Identifier  |          SeqNum               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                           Timestamp                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct __attribute__((packed)) {
    uint8_t type;			///< Type of ping message (request or reply)
    uint8_t identifier;		///< Unique identifier for ping process
    uint16_t seq_num;		///< Sequence number (increasing number within that process)
    uint32_t timestamp;		///< Timestamp of request message
} mms_ping_message_t;

/**
 * @brief	Send ping echo request to destination address.
 *
 * @param	destination	The destination address.
 * @param	seq	Sequence number of ping message.
 * @param	prio Priority of ping message.
 * @param	ttl TTL value for network message.
 */
static void mms_ping(uint16_t destination, uint8_t seq, uint8_t prio, int ttl);

static uint8_t mms_ping_last_proc_id;	///< Random process id of last ping request
static uint8_t mms_ping_packets;		///< Number of ping replies received
static int mms_ping_dups;				///< Duplicates
static bool ping_bc_mode;				///< If option -b is set and address is BC
static bool ping_silent_mode;			///< If option -s is set
static bool *dups;						///< Helper buffer to check for DUPs
static float rtt_min;					///< Min. RTT
static float rtt_max;					///< Max. RTT
static float rtt_avg;					///< Avg. RTT

static struct utimer mms_ping_utimer;	///< utimer for ping
static uint16_t mms_ping_pid;			///< Process ID of process ping is running within

/*---------------------------------------------------------------------------*/
//				  Data structures for build in SSH
/*---------------------------------------------------------------------------*/

#define MMS_SSH_CON_REQUEST			(1)		///< SSH type: connection request
#define MMS_SSH_CON_ACCEPT			(2)		///< SSH type: connection accept
#define MMS_SSH_CON_REJECT			(3)		///< SSH type: connection reject
#define MMS_SSH_CON_CLOSE			(4)		///< SSH type: connection close
#define MMS_SSH_DATA				(5)		///< SSH type: data packet

#define MMS_SSH_DATA_MAX		   (43)		///< Maximum SSH data packet size

/**
 * Represents a SSH message.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct __attribute__((packed)) {
    uint8_t type;			///< Type of SSH message
} mms_ssh_message_t;

/**
 * Represents a SSH data message.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Type      |                 Data                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct __attribute__((packed)) {
    uint8_t type;					///< Type of SSH message
    uint8_t data[MMS_SSH_DATA_MAX];	///< Message information
} mms_ssh_data_message_t;

/**
 * @brief	Make SSH connection request with given node.
 *
 * @param	destination The destination address.
 */
static void mms_ssh_connect(uint16_t destination);

/**
 * @brief	Accept or reject SSH connection request.
 *
 * @param	destination The destination address.
 * @param	socket The socket to use or -1.
 * @param	accept Whether to accept or reject the connection request.
 */
static void mms_ssh_reply_connect(uint16_t destination, int socket, bool accept);

/**
 * @brief	Close SSH connection.
 *
 * @param	destination The destination address.
 */
static void mms_ssh_close(uint16_t destination);

static volatile int ssh_socket = -1;

/*---------------------------------------------------------------------------*/
//						Interface definitions
/*---------------------------------------------------------------------------*/

static route_interface_t r_iface = {
    rt_add_route,
    rt_add_fix_route,
    rt_lookup_route,
    mmr_peek,
    mmr_send,
    mmr_packet_dropped,
    mmr_receive,
    mmr_print_stats
};

/*---------------------------------------------------------------------------*/
//							Build in MMS commands
/*---------------------------------------------------------------------------*/

//#if CMD_ISLEVEL(CMD_LEVEL_SYSTEM_DEBUG | CMD_LEVEL_HUMAN_USER | CMD_LEVEL_OPERATION)
ASCCMD(route, CMDFLAG_SERIAL, "[-adfFg] print kernel route table");
CMD_FUNCTION(route, cmdargs)
{
    if(cmdargs->arg_size > 0) {
        char *msg = (char *)cmdargs->args;

        while(*msg == ' ') {
            msg++;
        }

        if(*msg == '-' && *(msg + 1) == 'f') {
            mms_flush_routes(false);
            printf("Kernel route table flushed (non-static)!\n");
            return CMD_SUCCESS;
        }
        else if(*msg == '-' && *(msg + 1) == 'F') {
            mms_flush_routes(true);
            printf("Kernel route table flushed (static)!\n");
            return CMD_SUCCESS;
        }
        else if(*msg == '-' && *(msg + 1) == 'd') {
            msg++;
            msg++;

            while(*msg == ' ') {
                msg++;
            }

            uint16_t address = net_strtoaddr(msg, &msg);

            if(rt_remove_static_route(address)) {
                printf("Static route deleted successfully!\n");
                return CMD_SUCCESS;
            }

            return CMD_ERROR;
        }
        else if(*msg == '-' && *(msg + 1) == 'g') {
            msg++;
            msg++;

            while(*msg == ' ') {
                msg++;
            }

            uint16_t address = net_strtoaddr(msg, &msg);
            int c = rt_remove_static_gateway_routes(address);
            printf("%u static route(s) deleted!\n", c);
            return CMD_SUCCESS;
        }
        else if(*msg == '-' && *(msg + 1) == 'a') {
            msg++;
            msg++;

            while(*msg == ' ') {
                msg++;
            }

            uint16_t address = net_strtoaddr(msg, &msg);
            uint16_t gateway = net_strtoaddr(msg, &msg);
            int metric = (int)strtoul(msg, &msg, 0);
            int iface = (int)strtoul(msg, &msg, 0);

            if(address != 0 && gateway != 0) {
                if(rt_add_static_route(address, gateway, metric, iface)) {
                    printf("Static route added successfully!\n");
                    return CMD_SUCCESS;
                }
            }

            return CMD_ERROR;
        }
        else {
            printf("Usage: route [-adfFg] print kernel route table\n\n");
            printf("           -a <DST> <GW> <M> <IFACE>, add static route to kernel route table\n");
            printf("           -d <DST>, delete static route out of kernel route table\n");
            printf("           -f, flush non-static routes out of kernel route table\n");
            printf("           -F, flush static routes out of kernel route table\n");
            printf("           -g <GW>, delete static routes out of kernel route table with a\n"
                   "                    specific gateway\n\n");
            printf("           <DST> = destination network address\n");
            printf("           <GW> = gateway network address\n");
            printf("           <M> = metric, e.g. number of hops\n");
            printf("           <IFACE> = network interface number\n\n");
            return CMD_ERROR;
        }
    }
    else {
        mms_print_routes();
        return CMD_SUCCESS;
    }
}

ASCCMD(ifconfig, CMDFLAG_SERIAL, "[IFACE]: print interface configuration");
CMD_FUNCTION(ifconfig, cmdargs)
{
    if(cmdargs->arg_size > 0) {
        char *msg;
        int iface = (int)strtoul(cmdargs->args, &msg, 0);

        if(cmdargs->arg_size > 1) {
            while(*msg == ' ') {
                msg++;
            }

            if(*msg == '-' && (*(msg + 1) == 'P' || *(msg + 1) == 'p')) {
                msg++;
                msg++;
                uint8_t power = (uint8_t)strtoul(msg, &msg, 0);

                if(*msg != '\0') {
                    return CMD_ERROR;
                }

                if(mms_set_output_power(iface, power)) {
                    printf("Output power set!\n");
                    return CMD_SUCCESS;
                }

                return CMD_ERROR;
            }
            else if(*msg == '-' && (*(msg + 1) == 'A' || *(msg + 1) == 'a')) {
                msg++;
                msg++;

                while(*msg == ' ') {
                    msg++;
                }

                uint16_t address = net_strtoaddr(msg, &msg);

                if(mms_set_interface_address(iface, address)) {
                    printf("Interface address set!\n");
                    return CMD_SUCCESS;
                }

                return CMD_ERROR;
            }
            else {
                printf("Usage: ifconfig [IFACE] [-AP] print interface configuration\n\n");
                printf("           -A <ADDR>, set interface address, e.g. ifconfig 0 -A 1.1\n");
                printf("           -P <POWER>, set output power on interface\n\n");
                return CMD_ERROR;
            }
        }
        else {
            mms_print_ifconfig(iface);
        }
    }
    else {
        mms_print_ifconfig(-1);
    }

    return CMD_SUCCESS;
}

ASCCMD(ping, CMDFLAG_SERIAL, "ping [-bchpstw] destination");
CMD_FUNCTION(ping, cmdargs)
{
    if(cmdargs->arg_size > 0) {
        int i;
        msg m;
        uint8_t count = MMS_PING_PACKETS;
        uint8_t prio = PRIORITY_DATA;
        bool bc = false;
        ping_bc_mode = false;
        ping_silent_mode = false;
        int ttl = 10;
        long timeout = 3;
        char adrbuf[10];
        const char *msg = cmdargs->args;
    read_ping_cmd:

        while(*msg == ' ') {
            msg++;
        }

        if(*msg == '-' && (*(msg + 1) == 'B' || *(msg + 1) == 'b')) {
            bc = true;
            msg++;
            msg++;
            goto read_ping_cmd;
        }
        else if(*msg == '-' && (*(msg + 1) == 'C' || *(msg + 1) == 'c')) {
            msg++;
            msg++;
            unsigned long tc = strtoul(msg, (char **)&msg, 0);

            if(tc == 0) {
                return CMD_ERROR;
            }

            if(tc > 255) {
                puts("Not more than 255 ping messages allowed!");
                return CMD_ERROR;
            }

            count = (uint8_t) tc;
            goto read_ping_cmd;
        }
        else if(*msg == '-' && (*(msg + 1) == 'H' || *(msg + 1) == 'h')) {
            printf("Usage: ping [-bchpstw] destination\n\n");
            printf("        -b, do a broadcast ping\n");
            printf("        -c <COUNT>, set number of ping messages\n");
            printf("        -h, print a help synopsis\n");
            printf("        -p <PRIO>, set the ping message priority\n");
            printf("            PRIO = 1: alarm\n");
            printf("            PRIO = 2: warning\n");
            printf("            PRIO = 3: data (default)\n");
            printf("        -s, silent mode (no messages printed out)\n");
            printf("        -t <TTL>, TTL value of ping messages (default: 10)\n");
            printf("        -w <WAIT>, time to wait for a response, in seconds (default: 3)\n\n");
            return CMD_SUCCESS;
        }
        else if(*msg == '-' && (*(msg + 1) == 'p' || *(msg + 1) == 'P')) {
            msg++;
            msg++;
            unsigned long tp = strtoul(msg, (char **)&msg, 0);

            if(tp == 0) {
                return CMD_ERROR;
            }

            if(tp == 1) {
                prio = 0;
            }
            else if(tp == 2) {
                prio = 1;
            }
            else {
                prio = 2;
            }

            goto read_ping_cmd;
        }
        else if(*msg == '-' && (*(msg + 1) == 's' || *(msg + 1) == 'S')) {
            ping_silent_mode = true;
            msg++;
            msg++;
            goto read_ping_cmd;
        }
        else if(*msg == '-' && (*(msg + 1) == 't' || *(msg + 1) == 'T')) {
            msg++;
            msg++;
            unsigned long to = strtoul(msg, (char **)&msg, 0);

            if(to == 0 || to > 255) {
                return CMD_ERROR;
            }

            ttl = to;
            goto read_ping_cmd;
        }
        else if(*msg == '-' && (*(msg + 1) == 'w' || *(msg + 1) == 'W')) {
            msg++;
            msg++;
            unsigned long to = strtoul(msg, (char **)&msg, 0);

            if(to == 0) {
                return CMD_ERROR;
            }

            timeout = to;
            goto read_ping_cmd;
        }

        uint16_t address = net_strtoaddr((char *)msg, (char **)&msg);

        if(address == 0) {
            return CMD_ERROR;
        }

        int iface_addr = net_get_address_in_subnet(address);

        /* No ping to unsupported network or own address */
        if(iface_addr == 0 || iface_addr == address) {
            return CMD_ERROR;
        }

        /* If broadcast destination address, limit TTL to one hop */
        if(address == NETWORK_ADDR_BC(address)) {
            if(!bc) {
                puts("Do you want to ping broadcast? Then -b");
                return CMD_ERROR;
            }

            ttl = 1;
            ping_bc_mode = true;
        }

        /* Try to malloc duplicate detection buffer */
        dups = (bool *) malloc(count * sizeof(bool));

        if(dups == NULL) {
            puts("Not enough system memory to fulfill your request!");
            return CMD_ERROR;
        }

        net_addrtostr(address, adrbuf, sizeof(adrbuf));
        printf("PING %s %lu bytes of data.\n", adrbuf, sizeof(mms_ping_message_t));
        mms_ping_packets = 0;
        mms_ping_dups = 0;
        mms_ping_last_proc_id = (rand() % 255) + 1;
        rtt_min = 0xffffffff;
        rtt_max = 0x00000000;
        rtt_avg = 0x00000000;
        mms_ping_pid = fk_thread->pid;
        long ts_start = (uint32_t)clock_get_systemtime();

        for(i = 1; i <= count; i++) {
            /* No duplicate for this sequence number possible */
            dups[i - 1] = false;
            /* Send ping echo request to destination */
            mms_ping(address, i, prio, ttl);
            /* Set timeout for next ping echo request packet to ::timeout seconds */
            utimer_set_wpid(&mms_ping_utimer, timeout, mms_ping_pid, NULL);
            /* Wait for ping echo reply or timeout */
            msg_receive(&m);
            /* Remove user timer because maybe woken up by ping response */
            utimer_remove(&mms_ping_utimer);
        }

        ts_start = (uint32_t)clock_get_systemtime() - ts_start;
        printf("--- %s ping statistics ---\n", adrbuf);

        if(mms_ping_dups == 0) {
            printf("%u packets transmitted, %u received, %u%% packet loss, time %lu ms\n", count,
                   mms_ping_packets, ((count - mms_ping_packets) * 100) / count, ts_start);
        }
        else {
            printf("%u packets transmitted, %u received, +%i duplicates, %u%% packet loss, time %lu ms\n", count,
                   mms_ping_packets, mms_ping_dups, ((count - mms_ping_packets) * 100) / count, ts_start);
        }

        if(mms_ping_packets > 0) {
            printf("rtt min/avg/max = %.2f/%.2f/%.2f ms\n", rtt_min, rtt_avg / (mms_ping_packets + mms_ping_dups), rtt_max);
        }

        if(!ping_bc_mode && mms_ping_packets == count) {
            /* Calculate approximate throughput */
            printf("--- %s throughput statistics ---\n", adrbuf);
            float bw = (count * (8 + 4 + 62 + 2) * 1000) / (float)ts_start; /* for CC1100 */
            printf("approximate throughput (air): %.2f byte/sec\n", 2 * bw);
            bw = (count * 58 * 1000) / (float)ts_start; /* for CC1100 */
            printf("approximate throughput (dll): %.2f byte/sec\n", 2 * bw);
            bw = (count * NET_MESSAGE_PAYLOAD_LENGTH * 1000) / (float)ts_start;
            printf("approximate throughput (net): %.2f byte/sec\n", 2 * bw);
            bw = (count * UDPL_MESSAGE_LENGTH * 1000) / (float)ts_start;
            printf("approximate throughput (trans/UDPL): %.2f byte/sec\n", 2 * bw);
            bw = (count * TCPL_MESSAGE_LENGTH * 1000) / (float)ts_start;
            printf("approximate throughput (trans/TCPL): %.2f byte/sec\n", bw);
        }

        /* Ping is over, clear random ping process id and buffer */
        mms_ping_last_proc_id = 0;
        free(dups);
        return CMD_SUCCESS;
    }

    return CMD_ERROR;
}

ASCCMD(ssh, CMDFLAG_SERIAL, "Usage: ssh [-q] destination");
CMD_FUNCTION(ssh, cmdargs)
{
    if(cmdargs->arg_size > 0) {
        bool quit = false;
        const char *msg = cmdargs->args;

        while(*msg == ' ') {
            msg++;
        }

        if(*msg == '-' && (*(msg + 1) == 'Q' || *(msg + 1) == 'q')) {
            quit = true;
            msg++;
            msg++;
        }

        uint16_t address = net_strtoaddr((char *)msg, (char **)&msg);

        if(address == 0) {
            return CMD_ERROR;
        }

        int iface_addr = net_get_address_in_subnet(address);

        /* No ssh to unsupported network or own address */
        if(iface_addr == 0 || iface_addr == address) {
            return CMD_ERROR;
        }

        /* If broadcast destination address, also exit here */
        if(address == NETWORK_ADDR_BC(address)) {
            return CMD_ERROR;
        }

        if(!quit) {
            mms_ssh_connect(address);
        }
        else {
            mms_ssh_close(address);
        }

        return CMD_SUCCESS;
    }

    return CMD_ERROR;
}
//#endif

/*---------------------------------------------------------------------------*/
//					Ping message handler and send function
/*---------------------------------------------------------------------------*/

static void mms_ping_handler(void *message, int message_size,
                             packet_info_t *packet_info)
{
    mms_ping_message_t *ping = (mms_ping_message_t *)message;

    if(ping->type == MMS_PING_ECHO_REQUEST) {
        ping->type = MMS_PING_ECHO_REPLY;
        net_send((void *)ping, sizeof(mms_ping_message_t), packet_info->source,
                 LAYER_2_PROTOCOL_PING, packet_info->tos, 10);
    }
    else if(ping->type == MMS_PING_ECHO_REPLY) {
        if(ping->identifier == mms_ping_last_proc_id) {
            msg m;
            bool wasDup = false;
            char *msgDup;
            char buf[10];

            if(dups[ping->seq_num - 1]) {
                wasDup = true;
                mms_ping_dups++;
                msgDup = "(DUP!)";
            }
            else {
                mms_ping_packets++;
                dups[ping->seq_num - 1] = true;
                msgDup = "";
            }

            if(!ping_bc_mode && !wasDup) {
                utimer_remove(&mms_ping_utimer); /* Stop timeout timer */
            }

            float ms = ((uint32_t)clock_get_systemtime() - ping->timestamp);

            if(ms < rtt_min) {
                rtt_min = ms;
            }

            if(ms > rtt_max) {
                rtt_max = ms;
            }

            rtt_avg += ms;

            if(!ping_silent_mode) {
                net_addrtostr(packet_info->source, buf, sizeof(buf));
                printf("%lu bytes from %s: seq=%u ttl=%u time=%.2f ms %s\n",
                       sizeof(mms_ping_message_t), buf,
                       ping->seq_num, *packet_info->ttl_ptr, ms, msgDup);
            }

            if(!ping_bc_mode && !wasDup) {
                msg_send(&m, mms_ping_pid, false);
            }
        }
    }
}

static void mms_ping(uint16_t destination, uint8_t seq, uint8_t prio, int ttl)
{
    mms_ping_message_t ping_request;
    ping_request.type = MMS_PING_ECHO_REQUEST;
    ping_request.identifier = 0;
    ping_request.identifier = mms_ping_last_proc_id;
    ping_request.seq_num = seq;
    ping_request.timestamp = (uint32_t)clock_get_systemtime();
    net_send((void *)&ping_request, sizeof(mms_ping_message_t),
             destination, LAYER_2_PROTOCOL_PING, prio, ttl);
}

/*---------------------------------------------------------------------------*/
//					SSH message handler and send functions
/*---------------------------------------------------------------------------*/

static void mms_ssh_connect(uint16_t destination)
{
    mms_ssh_message_t ssh;
    ssh.type = MMS_SSH_CON_REQUEST;
    mms_send((void *)&ssh, sizeof(mms_ssh_message_t), destination,
             LAYER_3_PROTOCOL_SSH, PRIORITY_DATA);
}

static void mms_ssh_reply_connect(uint16_t destination, int socket, bool accept)
{
    mms_ssh_message_t ssh;
    ssh.type = accept ? MMS_SSH_CON_ACCEPT : MMS_SSH_CON_REJECT;
    trans_sendto(socket, (void *)&ssh, sizeof(mms_ssh_message_t),
                 LAYER_3_PROTOCOL_SSH, PRIORITY_DATA, destination);
}

static void mms_ssh_close(uint16_t destination)
{
    mms_ssh_message_t ssh;
    ssh.type = MMS_SSH_CON_CLOSE;
    mms_send((void *)&ssh, sizeof(mms_ssh_message_t), destination,
             LAYER_3_PROTOCOL_SSH, PRIORITY_DATA);
}

static void mms_ssh_handler(void *message, int message_size,
                            packet_info_t *packet_info)
{
    char adrbuf[10];
    mms_ssh_message_t *ssh = (mms_ssh_message_t *)message;

    if(ssh->type == MMS_SSH_CON_REQUEST) {
        if(ssh_socket > -1) {
            mms_ssh_reply_connect(packet_info->source, -1, false);
            return;
        }

        ssh_socket = trans_socket(SOCK_TCPL);

        if(ssh_socket < 0) {
            mms_ssh_reply_connect(packet_info->source, -1, false);
            return;
        }

        trans_connect(ssh_socket, packet_info->source);
        mms_ssh_reply_connect(packet_info->source, ssh_socket, true);
    }
    else if(ssh->type == MMS_SSH_CON_ACCEPT) {
        net_addrtostr(packet_info->source, adrbuf, sizeof(adrbuf));
        printf("SSH connection accepted by %s\n", adrbuf);
    }
    else if(ssh->type == MMS_SSH_CON_REJECT) {
        net_addrtostr(packet_info->source, adrbuf, sizeof(adrbuf));
        printf("SSH connection rejected by %s\n", adrbuf);
    }
    else if(ssh->type == MMS_SSH_CON_CLOSE) {
        if(ssh_socket > -1) {
            uint16_t peer;
            trans_getpeername(ssh_socket, &peer);

            if(peer == packet_info->source) {
                if(trans_close(ssh_socket, CLOSE_IMMEDIATE) == 1) {
                    ssh_socket = -1;
                }
            }
        }
    }
    else if(ssh->type == MMS_SSH_DATA) {
        mms_ssh_data_message_t *ssh_data = (mms_ssh_data_message_t *)message;
        printf((char *)ssh_data->data);
        fflush(stderr);
    }
}

void mms_net_printf(const char *format)
{
    if(ssh_socket > -1) {
        mms_ssh_data_message_t ssh_data;
        ssh_data.type = MMS_SSH_DATA;
        int i = 0;
        int len = strlen(format);

        while(i < len) {
            int chunk = len - i > (MMS_SSH_DATA_MAX - 1) ? (MMS_SSH_DATA_MAX - 1) : len - i;
            memset(ssh_data.data, 0, sizeof(ssh_data.data));
            memcpy(ssh_data.data, format + i, chunk);

            if(trans_send(ssh_socket, (void *)&ssh_data, sizeof(mms_ssh_data_message_t),
                          LAYER_3_PROTOCOL_SSH, PRIORITY_DATA) < 0) {
                break;
            }

            i += chunk;
        }
    }
    else {
        printf(format);
    }
}

/*---------------------------------------------------------------------------*/
//							Micro Mesh Stack API
/*---------------------------------------------------------------------------*/

void mms_init(void)
{
    mms_initp(LAYER_2_PROTOCOL_MMR, &r_iface);
}

void mms_initp(protocol_t rp, route_interface_t *ri)
{
    /* Initialize routing & network layer */
    mmr_init();
    net_init(ri);
    /* Initialize transport layer */
    trans_init();
    /* Add routing as protocol handler for given route messages */
    net_set_protocol_handler(rp, ri->receive);
    /* Add transport as protocol handler for UDPL and TCPL messages */
    net_set_protocol_handler(LAYER_2_PROTOCOL_UDPL, trans_receive_udpl);
    net_set_protocol_handler(LAYER_2_PROTOCOL_TCPL, trans_receive_tcpl);
    /* Add MMS ping handler */
    net_set_protocol_handler(LAYER_2_PROTOCOL_PING, mms_ping_handler);
    /* Add SSH handler */
    trans_set_protocol_handler(LAYER_3_PROTOCOL_SSH, mms_ssh_handler);
}

/*---------------------------------------------------------------------------*/

int mms_add_interface(const char *name, uint16_t addr, const radio_t *radio)
{
    return net_add_interface(name, addr, radio);
}

/*---------------------------------------------------------------------------*/

uint16_t mms_get_interface_address(int interface_id)
{
    return net_get_interface_address(interface_id);
}

/*---------------------------------------------------------------------------*/

bool mms_set_interface_address(int interface_id, uint16_t addr)
{
    return net_set_interface_address(interface_id, addr);
}

/*---------------------------------------------------------------------------*/

bool mms_set_output_power(int interface_id, uint8_t pa_idx)
{
    return net_set_output_power(interface_id, pa_idx);
}

/*---------------------------------------------------------------------------*/

int mms_set_protocol_handler(protocol_t protocol, packet_handler_t handler)
{
    return trans_set_protocol_handler(protocol, handler);
}

/*---------------------------------------------------------------------------*/

void mms_receive(void *msg, int msg_size, packet_info_t *packet_info)
{
    net_receive(msg, msg_size, packet_info);
}

/*---------------------------------------------------------------------------*/

int mms_socket(int type)
{
    return trans_socket(type);
}

/*---------------------------------------------------------------------------*/

int mms_connect(int socket, uint16_t dest_addr)
{
    return trans_connect(socket, dest_addr);
}

/*---------------------------------------------------------------------------*/

int mms_sock_send(int socket, void *buffer, int length, protocol_t protocol,
                  uint8_t priority)
{
    return trans_send(socket, buffer, length, protocol, priority);
}

/*---------------------------------------------------------------------------*/

int mms_sock_sendto(int socket, void *buffer, int length, protocol_t protocol,
                    uint8_t priority, uint16_t dest_addr)
{
    return trans_sendto(socket, buffer, length, protocol, priority, dest_addr);
}

/*---------------------------------------------------------------------------*/

int mms_send(void *msg, int msg_len, uint16_t dst, protocol_t protocol,
             uint8_t priority)
{
    return trans_sendto(-1, msg, msg_len, protocol, priority, dst);
}

/*---------------------------------------------------------------------------*/

int mms_poll(int socket)
{
    return trans_poll(socket);
}

/*---------------------------------------------------------------------------*/

int mms_close(int socket, int how)
{
    return trans_close(socket, how);
}

/*---------------------------------------------------------------------------*/

void mms_flush_routes(bool flush_static)
{
    rt_flush_routes(flush_static);
}

/*---------------------------------------------------------------------------*/

void mms_print_routes(void)
{
    rt_print_routes();
}

/*---------------------------------------------------------------------------*/

void mms_print_ifconfig(int iface)
{
    net_print_ifconfig(iface);
}

/*---------------------------------------------------------------------------*/

void mms_print_stats(void)
{
    net_print_stats();
    printf("\r\n");
    r_iface.print_stats();
}
