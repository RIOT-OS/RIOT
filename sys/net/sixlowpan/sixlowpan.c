#include <stdio.h>
#include "sixlowmac.h"
#include "sixlowip.h"

#include "radio/radio.h"


void sixlowpan_init(radio_t *rd){
    radio_driver = rd;                 
}
