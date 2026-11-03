/*
 * SPDX-FileCopyrightText: 2026 Hochschule für Angewandte Wissenschaften Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "msg.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"
#include "net/ipv6/addr.h"
#include "platform/hal_io.h"
#include "platform/hal_task.h"
#include "platform/hal_time.h"
#include "shell.h"
#include "ud3tn/agent_manager.h"
#include "ud3tn/agent_util.h"
#include "ud3tn/bundle_processor.h"
#include "ud3tn/cmdline.h"
#include "ud3tn/init.h"
#include "ztimer.h"

// Define your generated 64-bit ULA Prefix (fd00::/64)
#define ULA_PREFIX      "fd12:dead:beef:1::"
#define ULA_PREFIX_LEN  64

#define MAIN_MSG_QUEUE_SIZE (8)
static msg_t _main_msg_queue[MAIN_MSG_QUEUE_SIZE];

#define CONFIG_AGENT_ID "config"
#define CHAT_SINK_ID "chat"

extern struct bundle_agent_interface bundle_agent_interface;

#ifdef NODE_A
#  define LOCAL_EID "dtn://node-a.dtn/"
#  define PEER_EID "dtn://node-b.dtn/"
#  define PEER_EID_C "dtn://node-c.dtn/"
#  define LOCAL_IP "fd12:dead:beef:1:8c82:41ff:fe6c:4b32"
#  define PEER_IP "fd12:dead:beef:1:fc8b:caff:fee1:111e"
#  define PEER_PORT "4557"
#  define PEER_IP_C "fd12:dead:beef:1:48bd:97ff:fee0:e4ec"
#  define PEER_PORT_C "4558"
#endif
#ifdef NODE_B
#  define LOCAL_EID "dtn://node-b.dtn/"
#  define PEER_EID "dtn://node-a.dtn/"
#  define PEER_EID_C "dtn://node-c.dtn/"
#  define LOCAL_IP "fd12:dead:beef:1:fc8b:caff:fee1:111e"
#  define LOCAL_PORT "4557"
#  define PEER_IP "fd12:dead:beef:1:8c82:41ff:fe6c:4b32"
#  define PEER_PORT "4556"
#  define PEER_IP_C "fd12:dead:beef:1:48bd:97ff:fee0:e4ec"
#  define PEER_PORT_C "4558"
#endif
#ifdef NODE_C
#  define LOCAL_EID "dtn://node-c.dtn/"
#  define PEER_EID "dtn://node-b.dtn/"
#  define LOCAL_IP "fd12:dead:beef:1:48bd:97ff:fee0:e4ec"
#  define PEER_IP "fd12:dead:beef:1:88f5:d9ff:fe8d:21f"
#  define PEER_PORT "4557"
#endif
#ifdef NODE_A_ESP32
#  define LOCAL_EID "dtn://node-a.dtn/"
#  define PEER_EID "dtn://node-b.dtn/"
#  define PEER_EID_C "dtn://node-c.dtn/"
#  define LOCAL_IP "fd12:dead:beef:1:7ae3:6dff:fe09:5e7c"
#  define PEER_IP "fd12:dead:beef:1:cac9:a3ff:feca:7404"
#  define PEER_PORT "4557"
#endif
#ifdef NODE_B_ESP32
#  define LOCAL_EID "dtn://node-b.dtn/"
#  define PEER_EID "dtn://node-a.dtn/"
#  define PEER_EID_C "dtn://node-c.dtn/"
#  define LOCAL_IP "fd12:dead:beef:1:cac9:a3ff:feca:7404"
#  define LOCAL_PORT "4557"
#  define PEER_IP "fd12:dead:beef:1:7ae3:6dff:fe09:5e7c"
#  define PEER_PORT "4556"
#  define PEER_IP_C "fd12:dead:beef:1:3685:18ff:fe22:4938"
#  define PEER_PORT_C "4558"
#endif
#ifdef NODE_C_ESP32
#  define LOCAL_EID "dtn://node-c.dtn/"
#  define PEER_EID "dtn://node-b.dtn/"
#  define LOCAL_IP "fd12:dead:beef:1:3685:18ff:fe22:4938"
#  define LOCAL_PORT "4558"
#  define PEER_IP "fd12:dead:beef:1:cac9:a3ff:feca:7404"
#  define PEER_PORT "4557"
#endif

void send_chat_message(const char *message, const char *peer_eid);
void advertise_route(const char *peer_ip, const char *peer_port, const char *peer_sink_id,
                     const char *peer_eid, const char *advertise_ip, const char *advertise_port,
                     const char *advertise_sink_id, const char *advertise_eid);
void add_route(const char *sink_id, const char *peer_ip, const char *peer_port,
               const char *peer_eid);
void auto_init_ula(void);

static void chat_callback(struct bundle_adu data, void *param, const void *bp_context)
{
    (void)param;
    (void)bp_context;

    char msg[data.length + 1];
    memcpy(msg, data.payload, data.length);
    msg[data.length] = '\0';

    printf("\n[DTN RECEIVE] From Node %s: %s\n> ", data.source, msg);
    fflush(stdout);
    bundle_adu_free_members(data);
}

static void dtn_chat_thread(void *arg)
{
    (void)arg;
    ztimer_sleep(ZTIMER_MSEC, 2000);

    const struct agent chat_agent = {
        .sink_identifier = CHAT_SINK_ID,
        .callback = chat_callback,
        .param = NULL,
    };

    bundle_processor_perform_agent_action(bundle_agent_interface.bundle_signaling_queue,
                                          BP_SIGNAL_AGENT_REGISTER, // Register agent
                                          chat_agent, false);
#if defined(NODE_A) || defined(NODE_A_ESP32)
    add_route(CHAT_SINK_ID, PEER_IP, PEER_PORT, PEER_EID);
#endif
#if defined(NODE_B) || defined(NODE_B_ESP32)
    add_route(CHAT_SINK_ID, PEER_IP_C, PEER_PORT_C, PEER_EID_C);
    add_route(CONFIG_AGENT_ID, PEER_IP, PEER_PORT, PEER_EID);
    add_route(CHAT_SINK_ID, PEER_IP, PEER_PORT, PEER_EID);
    advertise_route(PEER_IP, PEER_PORT, CONFIG_AGENT_ID, PEER_EID, LOCAL_IP, LOCAL_PORT,
                    CHAT_SINK_ID, PEER_EID_C);
#endif

    printf("DTN Chat Thread gestartet.\n");

    int counter = 0;
    char payload[128];
    while (1) {
#if defined(NODE_A) || defined(NODE_A_ESP32)
        if (counter % 2 == 0) {
            snprintf(payload, sizeof(payload), "Hello World | Counter: %d", ++counter);
            send_chat_message(payload, PEER_EID);
        }
        else {
            snprintf(payload, sizeof(payload), "Hello World | Counter: %d", ++counter);
            send_chat_message(payload, PEER_EID_C);
        }
#endif
#if defined(NODE_B) || defined(NODE_B_ESP32)
        snprintf(payload, sizeof(payload), "Hello World | Counter: %d", ++counter);
        send_chat_message(payload, PEER_EID);
#endif
        ztimer_sleep(ZTIMER_MSEC, 5000); // send every 5 seconds
    }
}

void send_chat_message(const char *message, const char *peer_eid)
{
    char dest_eid[128];
    char payload[128];

    snprintf(dest_eid, sizeof(dest_eid), "%schat", peer_eid);
    snprintf(payload, sizeof(payload), "%s  SRC: %s, DST: %s", message, LOCAL_EID, dest_eid);
    printf("[DTN SEND] %s\n", payload);

    char *p_copy = strdup(payload);

    agent_create_forward_bundle(&bundle_agent_interface, 7, CHAT_SINK_ID, dest_eid,
                                hal_time_get_timestamp_ms(), 0, 60000, p_copy, strlen(p_copy), 0);
}

void advertise_route(const char *peer_ip, const char *peer_port, const char *peer_sink_id,
                     const char *peer_eid, const char *advertise_ip, const char *advertise_port,
                     const char *advertise_sink_id, const char *advertise_eid)
{
    char config_eid[128];
    char dest_eid[128];
    char advertise_cla[64];
    char advertise_dest_eid[128];
    char config_cmd[320];

    if (!peer_ip || !peer_port || !peer_sink_id || !peer_eid || !advertise_ip || !advertise_port ||
        !advertise_sink_id || !advertise_eid) {
        return;
    }

    /* The route is advertised to the peer's config agent. */
    snprintf(config_eid, sizeof(config_eid), "%s%s", peer_eid, CONFIG_AGENT_ID);
    snprintf(dest_eid, sizeof(dest_eid), "%s%s", peer_eid, peer_sink_id);
    snprintf(advertise_dest_eid, sizeof(advertise_dest_eid), "%s%s", advertise_eid,
             advertise_sink_id);
    snprintf(advertise_cla, sizeof(advertise_cla), "tcpclv3_riot:[%s]:%s", advertise_ip,
             advertise_port);

    /* Example resulting command:
     * (dtn://node-c.dtn/):(tcpclv3_riot:[fd12:dead:beef:1:88f5:d9ff:fe8d:21f]:4557):[(dtn://node-c.dtn/chat)]:[{0,4102444800,100000}];
     */
    snprintf(config_cmd, sizeof(config_cmd), "(%s):(%s):[(%s)]:[{0,4102444800,100000}];",
             advertise_eid, advertise_cla, advertise_dest_eid);

    const size_t cmd_len = strlen(config_cmd);
    const size_t payload_len = cmd_len + 1;
    uint8_t *payload = malloc(payload_len);

    if (!payload) {
        return;
    }

    payload[0] = 0x31; /* ROUTER_COMMAND_ADD */
    memcpy(&payload[1], config_cmd, cmd_len);

    agent_create_forward_bundle(&bundle_agent_interface, 7, (char *)peer_sink_id, config_eid,
                                hal_time_get_timestamp_ms(), 0, 60000, payload, payload_len, 0);

    printf("> Route for %s sent to %s via the configuration agent (%s).\n", advertise_dest_eid,
           peer_eid, advertise_cla);
}

void add_route(const char *sink_id, const char *peer_ip, const char *peer_port,
               const char *peer_eid)
{
    char dest_eid[128];
    char config_eid[128];

    snprintf(dest_eid, sizeof(dest_eid), "%s%s", peer_eid, sink_id);
    snprintf(config_eid, sizeof(config_eid), "%s%s", LOCAL_EID, CONFIG_AGENT_ID);

    {
        char peer_cla[64];
        char config_cmd[320];

        snprintf(peer_cla, sizeof(peer_cla), "tcpclv3_riot:[%s]:%s", peer_ip, peer_port);
        snprintf(config_cmd, sizeof(config_cmd), "(%s):(%s):[(%s)]:[{0,4102444800,100000}];",
                 peer_eid, peer_cla, dest_eid);

        printf(config_cmd);

        const size_t cmd_len = strlen(config_cmd);
        const size_t payload_len = cmd_len + 1;
        uint8_t *payload = malloc(payload_len);

        if (payload) {
            payload[0] = 0x31;
            memcpy(&payload[1], config_cmd, cmd_len);

            agent_create_forward_bundle(&bundle_agent_interface, 7, sink_id, config_eid,
                                        hal_time_get_timestamp_ms(), 0, 60000, payload, payload_len,
                                        0);

            printf("> Route for %s requested via the configuration agent (%s).\n", dest_eid,
                   peer_cla);
        }
    }
}

void auto_init_ula(void)
{
    gnrc_netif_t *netif = NULL;
    ipv6_addr_t addr;
    eui64_t iid;

    if (ipv6_addr_from_str(&addr, ULA_PREFIX) == NULL) {
        puts("Error: Invalid ULA prefix string format.");
        return;
    }

    while ((netif = gnrc_netif_iter(netif))) {
        ipv6_addr_t if_addr = addr;

        if (gnrc_netif_ipv6_get_iid(netif, &iid) < 0) {
            continue;
        }

        ipv6_addr_set_aiid(&if_addr, iid.uint8);

        if (gnrc_netif_ipv6_addr_add(netif, &if_addr, ULA_PREFIX_LEN, 0) < 0) {
            printf("Error: Failed to add ULA to interface %d\n", netif->pid);
        }
        else {
            char addr_str[IPV6_ADDR_MAX_STR_LEN];
            ipv6_addr_to_str(addr_str, &if_addr, sizeof(addr_str));
            printf("Successfully configured ULA %s on interface %d\n", addr_str, netif->pid);
        }
    }
}

int main(void)
{
    printf("Starte DTN Node: %s\n", LOCAL_EID);
    auto_init_ula();
    int tlsf_add_global_pool(void *mem, size_t bytes);
    ztimer_sleep(ZTIMER_MSEC, 1000);

    int argc = 6;
    char *argv[6];
    argv[0] = "ud3tn";
    argv[1] = "-e";
    argv[2] = LOCAL_EID;
    argv[3] = "-c";

#if defined(NODE_A) || defined(NODE_A_ESP32)
    argv[4] = "tcpclv3_riot:*,4556";
#endif
#if defined(NODE_B) || defined(NODE_B_ESP32)
    argv[4] = "tcpclv3_riot:*,4557";
#endif

#if defined(NODE_C) || defined(NODE_C_ESP32)
    argv[4] = "tcpclv3_riot:*,4558";
#endif
#ifdef CLA_OPTS_OVERRIDE
    argv[4] = CLA_OPTS_OVERRIDE; /* only for tests */
#endif
    argv[5] = "-R"; /* Enables remote configuration capability*/

    init(argc, argv);
    start_tasks(parse_cmdline(argc, argv));
    hal_task_create(dtn_chat_thread, NULL, true, NULL);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    msg_init_queue(_main_msg_queue, MAIN_MSG_QUEUE_SIZE);
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return start_os();
}
