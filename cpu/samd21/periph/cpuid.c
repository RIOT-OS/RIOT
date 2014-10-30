
#include <string.h>

#include "cpu.h"
#include "cpu-conf.h"
#include "periph/cpuid.h"





/**
 * @brief   Gets the serial number of the CPU.
 *
 * @param[out] id   The serial number of the CPU of length CPU_ID_LEN (must be
 *                  defined in the CPU's cpu-conf.h)
 */
void cpuid_get(void *id)
{
	uint8_t serial[CPUID_ID_LEN];
	memcpy(id, (void*)serial, CPUID_ID_LEN);
}
