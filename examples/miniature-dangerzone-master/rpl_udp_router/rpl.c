#include <stdio.h>
#include <string.h>
#include "vtimer.h"
#include "thread.h"
#include "sixlowpan.h"
#include "destiny.h"
#include "rpl.h"
#include "rpl_dodag.h"
#include "demo.h"

#ifdef MODULE_NATIVENET
#define TRANSCEIVER TRANSCEIVER_NATIVE
#else
#define TRANSCEIVER TRANSCEIVER_CC1100
#endif

char monitor_stack_buffer[MONITOR_STACK_SIZE];
radio_address_t id;
ipv6_addr_t std_addr;

uint8_t is_root = 0;

void init(char *str)
{
    transceiver_command_t tcmd;
    msg_t m;
    uint8_t chan = RADIO_CHANNEL;

    char command;

    int res = sscanf(str, "init %c", &command);

    if (res < 1) {
        printf("Usage: init (r|n)\n");
        printf("\tr\tinitialize as root\n");
        printf("\tn\tinitialize as node router\n");
    }

    uint8_t state;

    if ((command == 'n') || (command == 'r')) {
        printf("INFO: Initialize as %s on address %d\n", ((command == 'n') ? "node" : "root"), id);
        if (!id || (id > 255)) {
            printf("ERROR: address not a valid 8 bit integer\n");
            return;
        }

        state = rpl_init(TRANSCEIVER, id);

        if (state != SIXLOWERROR_SUCCESS) {
            printf("Error initializing RPL\n");
        }
        else {
            puts("6LoWPAN and RPL initialized.");
        }

        if (command == 'r') {
            rpl_init_root();
            is_root = 1;
        }
        else {
            ipv6_iface_set_routing_provider(rpl_get_next_hop);
        }
        int monitor_pid = thread_create(monitor_stack_buffer, MONITOR_STACK_SIZE, PRIORITY_MAIN-2, CREATE_STACKTEST, monitor, "monitor");
        transceiver_register(TRANSCEIVER, monitor_pid);
        ipv6_register_packet_handler(monitor_pid);
        //sixlowpan_lowpan_register(monitor_pid);
    }
    else {
        printf("ERROR: Unknown command '%c'\n", command);
        return;
    }

    /* TODO: check if this works as intended */
    ipv6_addr_t prefix, tmp;
    ipv6_addr_init(&std_addr, 0xABCD, 0xEF12, 0, 0, 0x1034, 0x00FF, 0xFE00, id);
    ipv6_addr_init_prefix(&prefix, &std_addr, 64);
    plist_add(&prefix, 64, NDP_OPT_PI_VLIFETIME_INFINITE, 0, 1, ICMPV6_NDP_OPT_PI_FLAG_AUTONOM);
    ipv6_init_iface_as_router();
    /* add global address */
    ipv6_addr_set_by_eui64(&tmp, &std_addr);
    ipv6_iface_add_addr(&tmp, IPV6_ADDR_TYPE_GLOBAL, NDP_ADDR_STATE_PREFERRED, 0, 0);
            
    /* set channel to 10 */
    tcmd.transceivers = TRANSCEIVER;
    tcmd.data = &chan;
    m.type = SET_CHANNEL;
    m.content.ptr = (void *) &tcmd;

    msg_send_receive(&m, &m, transceiver_pid);
    printf("Channel set to %u\n", RADIO_CHANNEL);

    destiny_init_transport_layer();
    puts("Destiny initialized");
    /* start transceiver watchdog */
}

void loop(char *unused)
{
    (void) unused;

    rpl_routing_entry_t *rtable;

    rtable = rpl_get_routing_table();
    rpl_dodag_t *mydodag = rpl_get_my_dodag();

    if (mydodag == NULL) {
        return;
    }

    printf("---------------------------\n");
    printf("OUTPUT\n");
    printf("my rank: %d\n", mydodag->my_rank);
    if (!is_root) {
        printf("my preferred parent:\n");
        printf("%s\n", ipv6_addr_to_str(addr_str, (&mydodag->my_preferred_parent->addr)));
        printf("parent lifetime: %d\n", mydodag->my_preferred_parent->lifetime);
    }
    printf("---------------------------$\n");

    for (int i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (rtable[i].used) {
            printf("%s\n", ipv6_addr_to_str(addr_str, (&rtable[i].address)));
            puts("next hop");
            printf("%s\n", ipv6_addr_to_str(addr_str, (&rtable[i].next_hop)));
            printf("entry %d lifetime %d\n", i, rtable[i].lifetime);

            if (!rpl_equal_id(&rtable[i].address, &rtable[i].next_hop)) {
                puts("multi-hop");
            }

            printf("---------------------------$\n");
        }
    }

    printf("########################\n");
}

void table(char *unused)
{
    (void) unused;

    rpl_routing_entry_t *rtable;
    rtable = rpl_get_routing_table();
    printf("---------------------------\n");
    printf("OUTPUT\n");
    printf("---------------------------\n");

    for (int i = 0; i < RPL_MAX_ROUTING_ENTRIES; i++) {
        if (rtable[i].used) {
            printf("%s\n", ipv6_addr_to_str(addr_str, (&rtable[i].address)));
            printf("entry %d lifetime %d\n", i, rtable[i].lifetime);

            if (!rpl_equal_id(&rtable[i].address, &rtable[i].next_hop)) {
                puts("multi-hop");
            }

            printf("--------------\n");
        }
    }

    printf("$\n");
}

void dodag(char *unused)
{
    (void) unused;

    printf("---------------------------\n");
    rpl_dodag_t *mydodag = rpl_get_my_dodag();

    if (mydodag == NULL) {
        printf("Not part of a dodag\n");
        printf("---------------------------$\n");
        return;
    }

    printf("Part of Dodag:\n");
    printf("%s\n", ipv6_addr_to_str(addr_str, (&mydodag->dodag_id)));
    printf("my rank: %d\n", mydodag->my_rank);
    if (!is_root) {
        printf("my preferred parent:\n");
        printf("%s\n", ipv6_addr_to_str(addr_str, (&mydodag->my_preferred_parent->addr)));
    }
    printf("---------------------------$\n");
}
