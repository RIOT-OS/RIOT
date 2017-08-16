/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  cpu_sam0_common
 * @{
 *
 * @file
 * @brief       Low-level CPUID driver implementation
 *
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "periph/cpuid.h"

#include "atmega_regs_common.h"

#include "at86rf2xx_netdev.h"

#include "debug.h"
#define ENABLE_DEBUG 1

/* Define CPU ID with CPU and transceiver IDs
 *
 * <1 Byte>
 * <JTAG Manufacturer ID>
 *
 * <3 Byte>
 * <Device Signature Part Numb>
 *
 * <1 Byte> \\ only first byte contains data
 * <JEDEC Manufacture ID 0 byte>
 *
 * <1 Byte>
 * <VERSION_NUM - Device Identification Register (Version Number)>
 *
 * <1 Byte>
 * < PART_NUM - Device Identification Register (Part Number) >
 *
 * < 1 Byte>
 * <Random Number> \\ MAc is initialized
 *
 * CPU ID: 1f:02:a8:1e:1f:3:94:<xx>
 *
 * The random part is initialized because HWaddr, Long HWaddr and
 * inet6 addr are derived from the CPUID.
 * So to initialize different addresses to different nodes the
 * random part is introduced.
 */
void cpuid_get(void *id)
{
	/* use random number generator to get last byte for the ID */
	uint8_t random;
	at86rf2xx_get_random_num( &random, 1);

	uint8_t addr[] = {
			random,
			PART_NUM,
			VERSION_NUM,
			MAN_ID_0,
			SIGNATURE_0,
			SIGNATURE_1,
			SIGNATURE_2,
			MAN_ID_0
	};

    memcpy( id , addr, 7);
}
