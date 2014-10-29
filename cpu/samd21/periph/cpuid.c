/*
 * Copyright (C) 2014 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc2538
 * @{
 *
 * @file        cpuid.c
 * @brief       CPU-ID driver implementation
 *
 * The CC2538 provides a 64-bit unique identifier, that is unique for each shipped unit.
 *
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "cpu-conf.h"
#include "periph/cpuid.h"

//#define BITS_PER_BYTE 8

/**
 * \internal Pointer to the NVM MEMORY region start address
 */
#define NVM_MEMORY  ((volatile uint16_t *)FLASH_ADDR)

#define Word0   (0x0080A00C)
#define Word1   (0x0080A040)
#define Word2   (0x0080A044)
#define Word3   (0x0080A048)

void cpuid_get(void *id)
{    
    /* Get a pointer to the module hardware instance */
    Nvmctrl *const nvm_module = NVMCTRL;    

    /* wait for the device to become ready */
    while(!nvm_module->INTFLAG.reg & NVMCTRL_INTFLAG_READY) { /* waiting for the world to change! */}

    /* Clear error flags */
    nvm_module->STATUS.reg |= NVMCTRL_STATUS_MASK;

    uint8_t buffer[CPUID_ID_LEN];
    uint32_t source_address[] = {Word0, Word1, Word2, Word3};

    for(int i = 0; i < 4; i++)
    {
        uint32_t page_address = (source_address[i] / 2);

            /* NVM _must_ be accessed as a series of 16-bit words, perform manual copy
         * to ensure alignment */
        for (uint16_t i = 0; i < CPUID_ID_LEN; i += 2) 
        {
                /* Fetch next 16-bit chunk from the NVM memory space */
                uint16_t data = NVM_MEMORY[page_address++];

                /* Copy first byte of the 16-bit chunk to the destination buffer */
                buffer[i] = (data & 0xFF);

                /* If we are not at the end of a read request with an odd byte count,
                 * store the next byte of data as well */
                if (i < (CPUID_ID_LEN - 1)) {
                    buffer[i + 1] = (data >> 8);
                }
            }
    }
    
    memcpy(id, (void*) buffer, CPUID_ID_LEN);
}
/** @} */