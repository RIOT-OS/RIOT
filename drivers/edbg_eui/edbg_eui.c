/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_edbg_eui
 *
 * @{
 * @file
 * @brief       Driver for getting a unique ID from the Atmel Embedded Debugger.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "edbg_eui.h"
#include "periph/i2c.h"
#include "net/l2util.h"

#ifndef EDBG_I2C_DEV
#define EDBG_I2C_DEV I2C_DEV(0)
#endif

#define EDBG_ADDR   (0x28)
#define TOKEN_EUI64 (0xD2)
#define TOKEN_EXT   (0xE1)

int edbg_get_eui64(eui64_t *addr)
{
    int res = 0;
    i2c_acquire(EDBG_I2C_DEV);

    /* dummy read for another token - following edbg user manual, section 4.3.1
     * only returns the EUI-64 once, this work-around works reliable though.
     */
    if ((res = i2c_read_regs(EDBG_I2C_DEV, EDBG_ADDR, TOKEN_EXT, addr, 2, 0))) {
        goto out;
    }

    /* EDBG provides an EUI-64 for the internal radio */
    if ((res = i2c_read_regs(EDBG_I2C_DEV, EDBG_ADDR, TOKEN_EUI64, addr, sizeof(*addr), 0))) {
        goto out;
    }

out:
    i2c_release(EDBG_I2C_DEV);
    return res;
}
