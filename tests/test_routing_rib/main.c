/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   functional tests for the routing information base
 *
 * @author  Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * @}
 */

 #include <stdio.h>
 #include "rib.h"

#define MAX_ROUTE_ENTRIES (10)



void ipv6_addr_init(ipv6_addr_t *out, uint16_t addr0, uint16_t addr1,
                    uint16_t addr2, uint16_t addr3, uint16_t addr4,
                    uint16_t addr5, uint16_t addr6, uint16_t addr7)
{
    out->uint16[0] = HTONS(addr0);
    out->uint16[1] = HTONS(addr1);
    out->uint16[2] = HTONS(addr2);
    out->uint16[3] = HTONS(addr3);
    out->uint16[4] = HTONS(addr4);
    out->uint16[5] = HTONS(addr5);
    out->uint16[6] = HTONS(addr6);
    out->uint16[7] = HTONS(addr7);
}


void print_ipv6_addr(ipv6_addr_t *in)
{
    if (in == NULL) {
        puts("no address!");
    }
    for (int i = 0; i < 8; ++i) {
        printf("%0X:", (in->uint16[i]));
    }
    puts("");
}

rib_t *choose_nexthop(rib_t **aCandidates, size_t candidatesSize)
{

    for (int i = 0; i < candidatesSize; ++i) {
        printf("Nexthop candidate RIB %i of %i\n", i+1,candidatesSize );
        print_rib_t(aCandidates[i]);
    }
    
    return aCandidates[0];
}

int main(void)
{
    route_init(MAX_ROUTE_ENTRIES);

    routing_handler_t stProtocol;
    stProtocol.next = NULL;
    stProtocol.protocol_id = 1;
    stProtocol.supported_metric_id = create_empty_metrics(2);
    stProtocol.choose_nexthop = &choose_nexthop;

    stProtocol.supported_metric_id->origin_protocol_id = stProtocol.protocol_id;
    stProtocol.supported_metric_id->next->origin_protocol_id = stProtocol.protocol_id;

    puts("-= prepared protocol: =-");
    print_routing_handler_t(&stProtocol);
    route_register_protocol(&stProtocol);

    ipv6_addr_t ipDefaultGateway;
    ipv6_addr_init(&ipDefaultGateway, 0xfe80, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007);
    route_add(NULL, &ipDefaultGateway, NULL, NULL);
    
    int nIface = 0;
    ipv6_addr_t dG;
    route_get_default_gateway(&dG, &nIface);
    
    puts("\n-= default Gateway: =-");
    for (int i = 0; i < 8; ++i) {
        printf("%0X:", (dG.uint16[i]));
    }
    puts("");

    ipv6_addr_t ipSomeHop;
    ipv6_addr_init(&ipSomeHop, 0xfe80, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0xFF07);    
    route_add(&ipSomeHop, NULL, NULL, NULL);

    ipv6_addr_init(&ipSomeHop, 0xfe80, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0xEE07);
    route_add(&ipSomeHop, &dG, NULL, NULL);

    puts("\n-= routing table: =-");
    print_table();
    
    puts("\n-= find nexthop: =-");
    ipv6_addr_t *pNext;
    pNext = route_get_next_hop(&ipSomeHop);

    puts("\n-= nexthop is: =-");
    print_ipv6_addr(pNext);

    prune_metrics(stProtocol.supported_metric_id);
    route_remove_protocol(1);

 	return 0;
}
