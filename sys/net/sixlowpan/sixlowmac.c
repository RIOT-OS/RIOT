/* 6LoWPAN MAC - layer 2 implementations */

#include "sixlowmac.h"
#include "drivers/cc110x/cc1100.h"
#include <stdio.h>
#include <stdint.h>

static void init_msba2_mac(mac_addr addr){
    addr.oui1 = MSBA2_OUI >> 8;
    addr.oui2 = 0x0 | MSBA2_OUI;
    addr.ext_ident = MSBA2_R8BIT;
    addr.ext_ident = cc1100_get_address();
}

static void init_mac_address(mac_addr addr){
    
#ifdef SIXLOWPAN_MSBA2
    init_msba2_mac(addr);
#endif    
}
