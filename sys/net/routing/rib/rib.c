#include "rib.h"
#include <stdio.h>


// mutex for exclusive operations on the list
mutex_t mtx_table;

// default set value for a maximum number of entries persisting in the routing table
size_t max_entries;

// the current number of entries in the routing table
size_t current_entries;

// pointer to the starting entry of the routing table
rib_t    *routing_table;

// pointer to quickly access the default gateway
rib_t    *default_gateway;

// the current next hop is stored here;
ipv6_addr_t     current_next_hop;

// pointer to the starting registered routing protocol
routing_handler_t   *routing_protocols_registered;


void route_register_protocol(routing_handler_t *protocol)
{
    mutex_lock(&mtx_table);
    if (protocol == NULL)
    {
        puts("[error] invalid parameter for protocol!");
        mutex_unlock(&mtx_table);
        return;
    }
    routing_handler_t *root = routing_protocols_registered;

    if (routing_protocols_registered == NULL) {
        routing_protocols_registered = (routing_handler_t *) malloc(sizeof(routing_handler_t));
        routing_protocols_registered->next = NULL;
        root = routing_protocols_registered;
        *root = *protocol;
    }
    else {
        while (root != NULL) {
            if (root->protocol_id == protocol->protocol_id) {
                // update protocol
                protocol->next = root->next;
                *root = *protocol;
                mutex_unlock(&mtx_table);
                return;
            }

            root = root->next;
        }

        root = (routing_handler_t *) malloc(sizeof(routing_handler_t));
        *root = *protocol;
    }

    mutex_unlock(&mtx_table);
}

void route_remove_protocol(uint32_t protocol_id)
{
    mutex_lock(&mtx_table);
    routing_handler_t *root = routing_protocols_registered;
    routing_handler_t *previous = routing_protocols_registered;

    while (root != NULL) {
        if (root->protocol_id == protocol_id) {
            if (root == routing_protocols_registered) {
                routing_protocols_registered = routing_protocols_registered->next;
                free(root);
                root = routing_protocols_registered;
                previous = root;
            }
            else {
                // remove reference to node
                previous->next = root->next;
                free(root);
                root = previous->next;
            }
        }
        else {
            previous = root;
            root = root->next;
        }
    }

    mutex_unlock(&mtx_table);
}

void route_init(size_t max)
{
    int err;
    err = mutex_init(&mtx_table);

    if (err < 1) {
        printf("[!!!] mutex_init failed with %d\n", err);
    }

    mutex_lock(&mtx_table);
    max_entries = max;
    mutex_unlock(&mtx_table);

    prune_table();
}

void route_add(ipv6_addr_t *dst_net_addr, ipv6_addr_t *gw_net_addr, int *iface_id, metric_t *metric)
{
    mutex_lock(&mtx_table);

    if (dst_net_addr == NULL && gw_net_addr == NULL) {
        puts("[error] invalid parameter for destination and gateway!");
        mutex_unlock(&mtx_table);
        return;
    }

    if (current_entries == max_entries) {
        // we need to make room for the new entry
        // but how to decide the expandability of an entry????
        // probably callback all routing protocols for assistance is th only way
        //current_entries--;
    }

    rib_t *root = routing_table;

    if (routing_table == NULL) {
        routing_table = (rib_t *) malloc(sizeof(rib_t));
        routing_table->next = NULL;
        // begin new list
        root = routing_table;
    }
    else {
        // traverse to last element and add a node
        while (root->next != NULL) {
            root = root->next;
        }

        root->next = (rib_t *) malloc(sizeof(rib_t));
        root = root->next;
    }

    root->next = NULL;
    root->flags = ROUTING_ROUTE_IS_UP;
    // if no network interface is specified use the first one (TODO: check if exists???)
    /*
    if (iface_id != NULL && net_if_get_interface(*iface_id) == NULL)
    {
        printf("[error] the given network interface 0x%02X is not available!", *iface_id);
        // route is down
        root.next->flags &= ~(1 << ROUTING_ROUTE_IS_UP);
        // return;
    }
    */
    root->iface_id = (iface_id == NULL) ? (0x0) : (*iface_id);

    if (metric != NULL) {
        root->metric = (metric_t *)malloc(sizeof(metric_t));
        *root->metric = *metric;
    }
    else {
        // we have no metric
        root->metric = metric;
    }

    root->next_hop = (gw_net_addr == NULL) ? (*dst_net_addr) : (*gw_net_addr);

    // the default gateway should be set
    if (dst_net_addr == NULL) {
        root->destination = *gw_net_addr;
        root->flags |= (ROUTING_TARGET_IS_GATEWAY | ROUTING_ROUTE_IS_DEFAULT_GATEWAY);

        if (default_gateway != NULL) {
            // reset the default gateway flag
            default_gateway->flags &= ~(1 << ROUTING_ROUTE_IS_DEFAULT_GATEWAY);
        }

        // set the new default gateway
        default_gateway = root;

    }
    else {
        root->destination = (*dst_net_addr);

        // destination is a network (no EUI-64)
        if ((root->destination.uint32[2] == 0 && root->destination.uint32[3] == 0)) {
            root->flags |= (ROUTING_TARGET_IS_GATEWAY);
        }
        else {
            // destination is a host (has EUI-64)
            root->flags |= (ROUTING_TARGET_IS_HOST);
        }
    }

    current_entries++;
    mutex_unlock(&mtx_table);
}

void route_del(ipv6_addr_t *dst_net_addr, int *iface_id)
{
    mutex_lock(&mtx_table);
    rib_t *root = routing_table;
    rib_t *previous = root;

    if (root == NULL) {
        mutex_unlock(&mtx_table);
        return;
    }

    while (root != NULL) {
        if (ipv6_addr_is_equal(dst_net_addr, &(root->destination))) {
            // delete route for all interfaces
            if (iface_id == NULL || *iface_id == root->iface_id) {

                prune_metrics(root->metric);

                if (root == routing_table) {
                    routing_table = routing_table->next;
                    free(root);
                    root = routing_table;
                    previous = root;
                }
                else {
                    // remove reference to node
                    previous->next = root->next;
                    free(root);
                    root = previous->next;
                }
            }

        }
        else {
            previous = root;
            root = root->next;
        }
    }

    current_entries--;
    mutex_unlock(&mtx_table);
}


void route_get_default_gateway(ipv6_addr_t *default_gw_net_addr_out, int *iface_id_out)
{
    mutex_lock(&mtx_table);
    if (default_gateway == NULL) {
       default_gw_net_addr_out = NULL;
    }
    else {
        *default_gw_net_addr_out = default_gateway->destination;
    }
    *iface_id_out = (default_gateway == NULL) ? -1 : (default_gateway->iface_id);
    mutex_unlock(&mtx_table);
}

ipv6_addr_t *route_get_next_hop(ipv6_addr_t *dst_net_addr_in)
{
    if (route_handle_candidates(dst_net_addr_in, &current_next_hop) == ROUTING_RET_NO_ROUTE_FOUND) {
        return NULL;
    }

    return &current_next_hop;
}


int route_handle_candidates(ipv6_addr_t *dst_net_addr_in, ipv6_addr_t *next_hop_out)
{
    mutex_lock(&mtx_table);
    // 1. get all entries with longest common prefix
    rib_t **aFound = NULL;
    size_t nSize = 0;
    // get the count
    find_longest_common_prefix(dst_net_addr_in, aFound, &nSize);

    if (nSize == 0) {
        // no specific entry has been found in the current routing information base
        // so the default_gateway entry is the only candidate
        if (default_gateway == NULL) {
            // anyway if there is no default gateway no routing is possible for the destination
            mutex_unlock(&mtx_table);
            return ROUTING_RET_NO_ROUTE_FOUND;
        }

        *next_hop_out = default_gateway->next_hop;
        mutex_unlock(&mtx_table);
        return ROUTING_RET_ONLY_DEFAULT_GATEWAY;
    }

    // prepare memory and get the found entries
    aFound = (rib_t **)malloc(nSize * (sizeof(rib_t *)));
    find_longest_common_prefix(dst_net_addr_in, aFound, &nSize);

    /*
    if (nSize == 1) {
        // there is only one matching entry so we can use it without bother the specific protocol
        *next_hop = *aFound[0]->next_hop;
        free(aFound);
        mutex_unlock(&mtx_table);

        return ROUTING_RET_OK;
    }
    */

    // we have multiple possibilities to route to the destination
    // so we must ask the specific protocol for the actual next_hop
    if (routing_protocols_registered != NULL) {

        // We ask the routing protocol to choose_nexthop
        // Note: the routing protocol must change metric specific values, e.g. lifetime or use-count,
        //       before returning the chosen routing entry pointer
        rib_t *chosen_next_hop_entry = routing_protocols_registered[0].choose_nexthop(aFound, nSize);

        if (chosen_next_hop_entry == NULL) {
            // the routing protocol decided none of the candidates should be used

            if (default_gateway == NULL) {
                // and we dont have a default gateway so no routing is possible for the given destination
                free(aFound);
                mutex_unlock(&mtx_table);
                return ROUTING_RET_NO_ROUTE_FOUND;
            }

            free(aFound);
            mutex_unlock(&mtx_table);
            *next_hop_out = default_gateway->next_hop;
            return ROUTING_RET_ONLY_DEFAULT_GATEWAY;
        }

        *next_hop_out = chosen_next_hop_entry->next_hop;
    }
    else {
        // probably the routes have been set manually because no protocol is registered
        // so we take the first available next_hop
        *next_hop_out = aFound[0]->next_hop;
    }

    free(aFound);
    mutex_unlock(&mtx_table);

    return ROUTING_RET_OK;
}


void find_longest_common_prefix(ipv6_addr_t *dst_net_addr_in, rib_t **aRIBentries, size_t *aRIBentries_size_in_out)
{
    rib_t *root = routing_table;
    int nLongestMatch = 0;
    int nCount = 0;

    // find and count longest prefix matches
    while (root != NULL) {
        for (int i = sizeof(ipv6_addr_t); i >= nLongestMatch; --i) {
            // only search for exact host matches or gateways with longest common prefix
            if (i == sizeof(ipv6_addr_t) || (root->flags & ROUTING_TARGET_IS_GATEWAY)) {
                int n = memcmp(&root->destination.uint8[0], &dst_net_addr_in->uint8[0], i);

                if (n == 0) {
                    if (i > nLongestMatch) {
                        nCount = 0;
                    }

                    nLongestMatch = i;
                    nCount++;
                }
            }
        }

        root = root->next;
    }

    // fill array of RIB entries
    if (aRIBentries != NULL && nCount != 0 && (int)(*aRIBentries_size_in_out) >= nCount) {
        int nEntry = 0;
        root = routing_table;

        while (root != NULL) {
            int n = memcmp(&root->destination.uint8[0], &dst_net_addr_in->uint8[0], nLongestMatch);

            if (n == 0) {
                aRIBentries[nEntry++] = root;
            }

            root = root->next;
        }
    }

    *aRIBentries_size_in_out = nCount;
}

int set_metric_values(metric_t *metric_querry_in_out, metric_t *metric_target)
{
    // set all matching values (and ignore missing ones)
    metric_t *pMetQ = metric_querry_in_out;
    metric_t *pMetT = metric_target;
    int nQmetrics = 0;

    while (pMetQ != NULL) {
        pMetQ->met_id = ROUTING_METRIC_ID_DEFAULT;
        nQmetrics++;

        while (pMetT != NULL) {
            if (pMetT->met_id == pMetQ->met_id) {
                pMetQ->met_val = pMetT->met_val;
                nQmetrics--;
            }

            pMetT = pMetT->next;
        }

        pMetQ = pMetQ->next;
    }

    return nQmetrics;
}

void prune_table()
{
    mutex_lock(&mtx_table);
    // prune list
    rib_t *root = routing_table;
    default_gateway = NULL;

    while (root != NULL) {
        prune_metrics(root->metric);
        rib_t *ribRemove = root;
        root = root->next;
        free(ribRemove);
    }

    routing_table = NULL;

    current_entries = 0;
    mutex_unlock(&mtx_table);
}


metric_t *create_empty_metrics(size_t count)
{
    metric_t *pMetricReturn = (metric_t *)malloc(sizeof(metric_t));
    metric_t *root = pMetricReturn;
    root->origin_protocol_id = 0;
    root->met_id = ROUTING_METRIC_ID_DEFAULT;
    root->met_val = 0;

    for (int i = 1; i < (int)count; ++i) {
        root->next = (metric_t *)malloc(sizeof(metric_t));
        root = root->next;
        root->origin_protocol_id = 0;
        root->met_id = ROUTING_METRIC_ID_DEFAULT;
        root->met_val = 0;
    }

    root->next = NULL;

    return pMetricReturn;
}

void prune_metrics(metric_t *starting_entry)
{
    metric_t *pMet = starting_entry;

    while (pMet != NULL) {
        metric_t *pRemove = pMet;
        pMet = pMet->next;
        free(pRemove);
    }
}

void print_rib_t(rib_t *pRib)
{
    printf("next: %p\n", (void *)pRib->next);
    printf("destina.: ");

    for (int i = 0; i < 8; ++i) {
        printf("%0X:", pRib->destination.uint16[i]);
    }

    puts("");
    printf("next_hop: ");

    for (int i = 0; i < 8; ++i) {
        printf("%0X:", pRib->next_hop.uint16[i]);
    }

    puts("");
    printf("iface_id: %d\n", pRib->iface_id);
    printf("flags: 0x%02X\n", pRib->flags);
}

void print_table()
{
    mutex_lock(&mtx_table);
    rib_t *root = routing_table;

    if (root == NULL) {
        puts("routing table empty.");
    }

    int nIter = 0;

    while (root != NULL) {
        printf("======== %d =========\n", nIter++);
        print_rib_t(root);
        root = root->next;
    }

    mutex_unlock(&mtx_table);
}

void print_metric_t(metric_t *pMet)
{
    if (pMet == NULL) {
        puts("metric is null");
        return;
    }

    printf("next:    %p\n", (void *)pMet->next);
    printf("met_id:  %d\n", pMet->met_id);
    printf("met_val: %d\n", pMet->met_val);
}

void print_routing_handler_t(routing_handler_t *protocol)
{
    if (protocol == NULL) {
        puts("protocol is null");
        return;
    }

    printf("next:    %p\n", (void *)protocol->next);
    printf("prot_id:  %d\n", protocol->protocol_id);
    print_metric_t(protocol->supported_metric_id);
    printf("choose_nexthop:    %p\n", (void *)protocol->choose_nexthop);
}
