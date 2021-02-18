#include <stdio.h>
#include <stdlib.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include "xtimer.h"

#include "fido2/ctap.h"
#include "fido2/ctap/transport/ctap_transport.h"

int main(void)
{
    /* sleep in order to see early DEBUG outputs */
    xtimer_sleep(3);
    fido2_ctap_transport_init();
}
