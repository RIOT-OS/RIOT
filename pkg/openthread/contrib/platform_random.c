#include <stdint.h>

#include <platform/random.h>
#include "random.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/* OpenThread will call this for getting a random value */

void ot_random_init(void)
{
#ifdef CPUID_LEN
    char cpu_id[CPUID_LEN];
    cpuid_get(cpu_id);
    uint32_t seed = 0;
    for(int i=0;i<(int) CPUID_LEN;i++)
    {
        seed += cpu_id[i];
    }
    random_init(seed);
#else
    random_init(0);
#endif
}

uint32_t otPlatRandomGet(void)
{
	return random_uint32();
}

