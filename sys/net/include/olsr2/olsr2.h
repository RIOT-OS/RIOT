#include "sixlowpan/types.h"

/**
 * @brief	Start routing using olsr2
 *		This initializes and starts all neccecary
 *		background threads for OLSRv2 routing, control
 *		messages will be send and received on MANET_PORT
 */
void olsr_init(void);

/**
 * @brief	prints all known routers of the MANET
 */
void print_topology_set(void);

#ifdef ENABLE_NAME
/**
 * @brief	get the IP address of a MANET router from it's hostname
 */
ipv6_addr_t* get_ip_by_name(char* name);
#endif
