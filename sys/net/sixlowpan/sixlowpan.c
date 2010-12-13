#include <stdio.h>
#include "sixlowmac.h"
#include "sixlowpan.h"
#include "transceiver.h"

void sixlowpan_init(transceiver_type_t trans){
    sixlowmac_init(trans);                     
}
