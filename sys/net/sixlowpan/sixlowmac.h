/* 6LoWPAN MAC header file */

#ifndef SIXLOWMAC_H
#define SIXLOWMAC_H

#include "sixlowip.h"
#include <stdio.h>
#include <stdint.h>

#define MSBA2_OUI	            0x005BA2	// 24bit OUI 
#define R8BIT	                0xA2		// random 8bit

#ifdef MSBA2_SENSOR_NODE
#define OUI                     0x005BA2
#else
#define OUI                     0x0
#endif

#define RADIO_CONF              radio_cc1100
#define RADIO                   RADIO_CONF

void create_link_local_prefix(ipv6_addr *ipaddr);

void create_all_routers_mcast_addr(ipv6_addr *ipaddr);

void set_eui64(ipv6_addr *ipaddr);

void get_link_layer_addr_from_ipaddr(ipv6_addr *ipaddr, link_layer_addr *lla);

void bootstrapping(void);
#endif
