#include <stdint.h>

#ifdef MODULE_XTIMER
#include "xtimer.h"
static uint32_t _last_event;
#endif

#if defined(URANDOM)
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

uint32_t entropy;
static unsigned _n = 0;

#ifndef ENTROPY_OP
#define ENTROPY_OP(x) (x & 0xFF)
#endif

void entropy_collect(void)
{
#ifdef MODULE_XTIMER
    uint32_t now = _lltimer_now();
    uint32_t diff = now - _last_event;
    _last_event = now;
#elif defined(URANDOM)
    int fd = open("/dev/urandom", O_RDONLY);
    char randbyte;
    read(fd, &randbyte, 1);
    uint32_t diff = (uint8_t) randbyte;
    close(fd);
#endif

    entropy ^= ((ENTROPY_OP(diff)) << ((_n++ & 0x3) * 8));
}
