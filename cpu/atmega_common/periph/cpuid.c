/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt, Steffen Robertz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
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
#include <stdint.h>
#include "avr/boot.h"

/*
 * This uses the RC calibration byte as CPU ID, so it may not be very unique.
 * The first three bytes are constant across different MCUs of the same series.
 */
void cpuid_get(void *id)
{
    uint8_t *out = id;
    out[0] = boot_signature_byte_get(0x00);     /* Atmel manufacturer ID (0x1E) */
    out[1] = boot_signature_byte_get(0x02);     /* Part Number high byte */
    out[2] = boot_signature_byte_get(0x04);     /* Part Number low byte */
    out[3] = boot_signature_byte_get(0x01);     /* internal RC calibration byte */
}
