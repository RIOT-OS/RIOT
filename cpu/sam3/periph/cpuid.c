/*
 * Copyright (C) 2015 <nqdinhddt@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @ingroup     drivers_periph_cpuid
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Dinh Nguyen <nqdinhddt@gmail.com>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

#define EFC_FCMD_STUI    0x0E   /* Start unique ID */
#define EFC_FCMD_SPUI    0x0F   /* Stop unique ID */
#define EFC_KEY          0x5A

/* During the sequence, the software can not run out of Flash */

__attribute__ ((section (".ramfunc")))
void cpuid_get(void *id)
{
    uint32_t cpuid[4];              /* 128 bits sam3 unique indentifier */

    /*
    Reading SAM3 Unique Identifier process (P 19.3.3.8):
    - Send the Start Read unique Identifier command (STUI) by writing the Flash Command Register with the
    STUI command.
    - When the Unique Identifier is ready to be read, the FRDY bit in the Flash Programming Status Register
    (EEFC_FSR) falls
    - The Unique Identifier is located in the first 128 bits of the Flash memory mapping. So, at the address
    0x400000-0x40008F.
    - To stop the Unique Identifier mode, the user needs to send the Stop Read unique Identifier command (SPUI)
    by writing the Flash Command Register with the SPUI command.
    - When the Stop read Unique Identifier command (SPUI) has been performed, the FRDY bit in the Flash
    Programming Status Register (EEFC_FSR) rises. If an interrupt was enabled by setting the FRDY bit in
    EEFC_FMR, the interrupt line of the NVIC is activated.
    */

    /* Send STUI */
    EFC1->EEFC_FCR = EEFC_FCR_FKEY(EFC_KEY) | EFC_FCMD_STUI;

    /* Wait for FRDY bit falls */
    while (EFC1->EEFC_FSR & EEFC_FSR_FRDY) {}

    /* Read UID */
    cpuid[0] = *(uint32_t *)IFLASH1_ADDR;
    cpuid[1] = *(uint32_t *)(IFLASH1_ADDR + 4);
    cpuid[2] = *(uint32_t *)(IFLASH1_ADDR + 8);
    cpuid[3] = *(uint32_t *)(IFLASH1_ADDR + 12);

    /* Send SPUI */
    EFC1->EEFC_FCR = EEFC_FCR_FKEY(EFC_KEY) | EFC_FCMD_SPUI ;

    /* Wait for FRDY bit rises. */
    while (0 == (EFC1->EEFC_FSR & EEFC_FSR_FRDY)) {}

    memcpy(id, (void*)cpuid, CPUID_LEN);
}
