/**
 * @addtogroup  driver_periph
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Kubaszek Mateusz <mathir.km.riot@gmail.com>
 */

#include <string.h>

#include "periph/cpuid.h"

#define EFM32WG_CPUID_ADDR (0x0FE081F0)

void cpuid_get(void *id)
{
    memcpy(id, (void *)(EFM32WG_CPUID_ADDR), CPUID_ID_LEN);
}

/** @} */
