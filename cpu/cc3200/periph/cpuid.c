/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file
 * @brief       CPU-ID driver implementation
 *
 * The cc3200 provides a 48-bit unique MAC identifier.
 *
 * @author      Attilio Dona'
 *
 * @}
 */

#include <string.h>

#include "cpu.h"
#include "periph/cpuid.h"

#include "device.h"

void cpuid_get(void *id) {
    unsigned char macAddressVal[SL_MAC_ADDR_LEN];
    unsigned char macAddressLen = SL_MAC_ADDR_LEN;
    unsigned char* addr_ptr = id;

    sl_NetCfgGet(SL_MAC_ADDRESS_GET, NULL, &macAddressLen,
            (unsigned char *) macAddressVal);

   memcpy(addr_ptr, macAddressVal, 3); // OUI
   addr_ptr[3] = 0xFF;
   addr_ptr[4] = 0xFE;
   memcpy(addr_ptr+5, macAddressVal+3, 3); // NIC

}
