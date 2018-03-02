/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega256rfr2
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

#include "atmega_regs_common.h"
#include "avr/boot.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief CPU_ID build from MCU register
 *
 *  CPIUD is taken from MCU Control Register and Signature bytes.
 *  CPUID:  1e a8 02 1f 94 03 ff ff
 *  CPUID:  1e a8 02 1f 94 92 XX XX
 *  MEGA62/128/256_RFR2 [MANUAL] p.505
 *  MEGA62/128/256_RFR2 [MANUAL] p.138
 *  MEGA62/128/256_RFR2 [MANUAL] p.492
 *
 *  usr_sign_0/1 are configurable values on flash page 1.
 */
void cpuid_get(void *id)
{
    uint8_t  signature_0 = boot_signature_byte_get(0x00);
    uint8_t  signature_1 = boot_signature_byte_get(0x02);
    uint8_t  signature_2 = boot_signature_byte_get(0x04);

    uint8_t  usr_sign_0 = boot_signature_byte_get(0x0100);
    uint8_t  usr_sign_1 = boot_signature_byte_get(0x0102);

    uint8_t addr[CPUID_LEN] = {
            signature_0,        /* 0x1E Atmel manufacturer ID */
            signature_1,        /* 0xA8 Part Number high byte */
            signature_2,        /* 0x02 Part Number low byte */
            MAN_ID_0,            /* 0x1F Atmel JEDEC manufacturer ID */
            PART_NUM,           /* 0x94 PART_NUM  Identification */
            VERSION_NUM,        /* 0x02 VERSION_NUM  Identification */
/* last two bytes can be set to flash page 1. for differentiation of different boards */
            usr_sign_0,         /* user signature 0 */
            usr_sign_1,         /* user signature 1 */
    };

#if defined(ENABLE_DEBUG)
    DEBUG("CPUID: " );
    for (uint8_t i=0; i<CPUID_LEN; i++)
    {
        DEBUG(" %02x ", addr[i] );
    }
    DEBUG("\n" );
#endif

    memcpy( id , addr, CPUID_LEN);
}
