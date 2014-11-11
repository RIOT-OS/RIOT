
#include <string.h>

#include "cpu.h"
#include "cpu-conf.h"
#include "periph/cpuid.h"

#define Word0 (*(volatile uint32_t *)0x0080A00C)
#define Word1 (*(volatile uint32_t *)0x0080A040)
#define Word2 (*(volatile uint32_t *)0x0080A044)
#define Word3 (*(volatile uint32_t *)0x0080A048)


void cpuid_get(void *id)
{
    uint32_t source_address[] = {Word0, Word1, Word2, Word3};
    memcpy(id, (void*) source_address, CPUID_ID_LEN);
}
