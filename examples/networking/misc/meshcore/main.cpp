/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       MeshCore node: repeater, advertisements, text messages and Public channel
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

#include "lora.h"

#include "meshcore_riot_node.h"

static meshcore_riot::Node *_node;

static void _print_info(void)
{
    const meshcore_radio_params_t &params = _node->getRadio().getParams();

    printf("Name: %s, %s\n", _node->getName(), _node->isRepeater() ? "repeater" : "node");
    printf("Public key: ");
    for (unsigned i = 0; i < PUB_KEY_SIZE; i++) {
        printf("%02x", _node->self_id.pub_key[i]);
    }
    printf("\nTime: %" PRIu32 "\n", _node->getRTCClock()->getCurrentTime());
    printf("Radio: %" PRIu32 " Hz, %" PRIu32 " Hz bandwidth, SF%u, CR 4/%u, %d dBm\n",
           params.frequency, lora_bw_to_hz(params.bandwidth), params.spreading_factor,
           params.coding_rate + 4, params.tx_power);
}

static void _print_contacts(void)
{
    for (unsigned i = 0; i < _node->getNumContacts(); i++) {
        const meshcore_riot::Contact &contact = _node->getContact(i);
        printf("%-20s %-8s %u hop(s)\n", contact.name,
               (contact.type == ADV_TYPE_REPEATER) ? "repeater" : "node",
               (unsigned)contact.hops);
    }
}

static void _print_usage(const char *cmd)
{
    printf("usage: %s info\n", cmd);
    printf("       %s name <name>\n", cmd);
    printf("       %s time <seconds since the epoch>\n", cmd);
    printf("       %s repeat <on|off>\n", cmd);
    printf("       %s log <on|off>\n", cmd);
    printf("       %s advert\n", cmd);
    printf("       %s contacts\n", cmd);
    printf("       %s msg <name> <text>\n", cmd);
    printf("       %s public <text>\n", cmd);
}

static int _mc_cmd(int argc, char **argv)
{
    if (argc < 2) {
        _print_usage(argv[0]);
        return 1;
    }

    int res = 0;
    _node->lock();

    if ((strcmp(argv[1], "info") == 0) && (argc == 2)) {
        _print_info();
    }
    else if ((strcmp(argv[1], "name") == 0) && (argc == 3)) {
        if (!_node->setName(argv[2])) {
            puts("Invalid name");
            res = 1;
        }
    }
    else if ((strcmp(argv[1], "time") == 0) && (argc == 3)) {
        _node->getRTCClock()->setCurrentTime(strtoul(argv[2], NULL, 10));
    }
    else if ((strcmp(argv[1], "repeat") == 0) && (argc == 3)) {
        _node->setRepeater(strcmp(argv[2], "on") == 0);
    }
    else if ((strcmp(argv[1], "log") == 0) && (argc == 3)) {
        _node->setLogPackets(strcmp(argv[2], "on") == 0);
    }
    else if ((strcmp(argv[1], "advert") == 0) && (argc == 2)) {
        _node->sendAdvert();
    }
    else if ((strcmp(argv[1], "contacts") == 0) && (argc == 2)) {
        _print_contacts();
    }
    else if ((strcmp(argv[1], "msg") == 0) && (argc == 4)) {
        if (!_node->sendText(argv[2], argv[3])) {
            puts("Unknown contact or message too long");
            res = 1;
        }
    }
    else if ((strcmp(argv[1], "public") == 0) && (argc == 3)) {
        if (!_node->sendPublic(argv[2])) {
            puts("Message too long or no free packet");
            res = 1;
        }
    }
    else {
        _print_usage(argv[0]);
        res = 1;
    }

    _node->unlock();
    return res;
}

static const shell_command_t _commands[] = {
    { "mc", "MeshCore node", _mc_cmd },
    { NULL, NULL, NULL },
};

int main(void)
{
    puts("MeshCore node");

    _node = meshcore_riot::Node::start(lora_setup());
    if (_node == NULL) {
        return 1;
    }

    _node->lock();
    _print_info();
    _node->unlock();

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
