#include <stdio.h>
#include <string.h>

#include <transceiver.h>
#include <vtimer.h>
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowborder.h"
#include "sys/net/sixlowpan/sixlowerror.h"

int main(void)
{        
    border_initialize(TRANSCEIVER_CC1100, NULL);
}
