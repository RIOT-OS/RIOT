#include <stdint.h>

#ifdef MODULE_XTIMER
#include "xtimer.h"
#endif

uint32_t entropy;
static unsigned _n = 0;
static uint32_t _last_event;

#ifndef ENTROPY_OP
#define ENTROPY_OP(x) (x & 0xFF)
#endif

void entropy_collect(void)
{
#ifdef MODULE_XTIMER
    uint32_t now = _lltimer_now();
    uint32_t diff = now - _last_event;
    _last_event = now;

    entropy += entropy ^ ((ENTROPY_OP(diff)) << ((_n++ & 0x3) * 8));
#endif
}
