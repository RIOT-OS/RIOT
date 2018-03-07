#include <string.h>

#include "periph/sleepmem.h"

#include "cpu.h"

#ifndef RTC_BKP_NUMBER
#define RTC_BKP_NUMBER (20u)
#endif

extern void rtc_lock(void);
extern void rtc_enable(void);
extern void rtc_unlock(void);

size_t sleepmem_size = (RTC_BKP_NUMBER * 4);

void sleepmem_init(void)
{
    rtc_enable();
}

size_t sleepmem_put(void *src, size_t n)
{
    n = (n <= sleepmem_size) ? n : sleepmem_size;

    volatile uint32_t *rtcregs = &RTC->BKP0R;

    rtc_unlock();

    while (n) {
        unsigned to_copy = (n >= 4) ? 4 : n;
        uint32_t tmp;
        memcpy(&tmp, src, to_copy);
        *rtcregs++ = tmp;
        n -= to_copy;
        src += to_copy;
    }

    rtc_lock();

    return n;
}

size_t sleepmem_get(void *target, size_t n)
{
    n = (n <= sleepmem_size) ? n : sleepmem_size;

    volatile uint32_t *rtcregs = &RTC->BKP0R;

    while (n) {
        unsigned to_copy = (n >= 4) ? 4 : n;
        uint32_t tmp = *rtcregs++;
        memcpy(target, &tmp, to_copy);
        n -= to_copy;
        target += to_copy;
    }

    return n;
}
