/* 6LoWPAN layer 3 implementation */

#include <stdio.h>
#include "drivers/cc110x/cc1100.h"
#include "sixlowmac.h"


static void output(void){
	
	// TODO
	// 1. get dest addr from function param
	// 2. get pointer to packet buffer
	// 3. if dest is null send broadcast
	// 4. hc1 or hc1x header compression
	// 5. check if ip-packet is to large to fit into a single mac-packet
	// 	if no, fragment the packet
			// frag1 dispatch + header
			// fragn dispatch
			// set fragments into queue and send it 	
}

/* convert 48-bit MAC address to IPv6 modified EUI-64 Identifier*/
static eui64 get_eui64_from_mac(void){
    mac_addr maddr;
    eui64 ident;
    
    init_mac_address(maddr);
    
    /* change bit 7 from oui1 to 1 */
    ident.oui1 = (maddr.oui_1 >> 8) | 0x2;
    ident.oui2 =  (maddr.oui_1 << 8) | maddr.oui_2;
    ident.pattern = MAC_TO_EUI64_PTRN;
    ident.ext_ident_1 = maddr.ext_ident_1;
    ident.ext_ident_2 = maddr.ext_ident_1;  

    return ident;
}

static void set_6lowpan_address(uint64_t prefix){
        
}
