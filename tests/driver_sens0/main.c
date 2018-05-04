/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @file
 * @brief       Basic test of Sens0 sensors.
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 */

#include <stdio.h>

#include "xtimer.h"
#include "sens0.h"
#include "sens0/i2c.h"
#include "sens0/specs/fxos8700cq.h"
#include "sens0/specs/fxas21002c.h"
#ifdef MODULE_SAUL_REG
#include "saul_reg.h"
#endif

/**
 * @brief   Read sensors every second
 */
#define INTERVAL            (1LU * US_PER_SEC)


/* The sensor configuration below was written for a FRDM-K64F board with a
 * FRDM-STBC-AGM01 9DOF shield, but might work on other boards, especially other
 * NXP FRDM variants */
static const sens0_i2c_t sens0devs[] = {
    {
        .specs = &sens0_specs_fxos8700cq,
        .params = &(sens0_i2c_params_t){ .i2c = I2C_DEV(0), .addr = 0x1d, },
    },
    {
        .specs = &sens0_specs_fxos8700cq,
        .params = &(sens0_i2c_params_t){ .i2c = I2C_DEV(0), .addr = 0x1e, },
    },
    {
        .specs = &sens0_specs_fxas21002c,
        .params = &(sens0_i2c_params_t){ .i2c = I2C_DEV(0), .addr = 0x20, },
    },
};
#ifdef MODULE_SAUL_REG
static saul_reg_t saul_entries[] = {
    { .name = "sens0_frdm_acc", .dev = (void *)&sens0devs[0], .driver = &sens0_saul_drv_fxos8700cq_i2c_acc, },
    { .name = "sens0_frdm_mag", .dev = (void *)&sens0devs[0], .driver = &sens0_saul_drv_fxos8700cq_i2c_mag, },
    { .name = "sens0_agm01_acc", .dev = (void *)&sens0devs[1], .driver = &sens0_saul_drv_fxos8700cq_i2c_acc, },
    { .name = "sens0_agm01_mag", .dev = (void *)&sens0devs[1], .driver = &sens0_saul_drv_fxos8700cq_i2c_mag, },
    { .name = "sens0_agm01_gyro", .dev = (void *)&sens0devs[2], .driver = &sens0_saul_drv_fxas21002c_i2c, },
};
#endif /* MODULE_SAUL_REG */

int main(void)
{
    xtimer_ticks32_t last_wakeup = xtimer_now();

    puts("Sens0 test application");

    puts("Initialize sensors");
#ifdef MODULE_SAUL_REG
    unsigned j = 0;
#endif /* MODULE_SAUL_REG */
    for (unsigned k = 0; k < sizeof(sens0devs) / sizeof(sens0devs[0]); ++k) {
        const sens0_i2c_t *s0 = &sens0devs[k];
        int res = sens0_i2c_init(s0);
        if (res < 0) {
            printf("sens0 init %u failed, res = %d\n", k, res);
#ifdef MODULE_SAUL_REG
            /* Skip all SAUL entries for this device to avoid leaving entries
             * for uninitialized/missing devices */
            /* This assumes the saul_entries array is sorted in the same order
             * as the sens0devs array */
            while ((j < sizeof(saul_entries) / sizeof(saul_entries[0])) &&
                   (saul_entries[j].dev == s0)) {
                ++j;
            }
#endif /* MODULE_SAUL_REG */
            continue;
        }
        /* Bring sensor out of standby */
        sens0_i2c_set_active(s0);
#ifdef MODULE_SAUL_REG
        while ((j < sizeof(saul_entries) / sizeof(saul_entries[0])) &&
               (saul_entries[j].dev == s0)) {
            saul_reg_add(&(saul_entries[j]));
            ++j;
        }
#endif /* MODULE_SAUL_REG */
    }

    while (1) {
        puts("Direct sens0 API read:");
        phydat_t buf;
        for (unsigned k = 0; k < sizeof(sens0devs) / sizeof(sens0devs[0]); ++k) {
            const sens0_i2c_t *s0 = &sens0devs[k];
            for (unsigned ch = 0; ch < s0->specs->outputs_nelem; ++ch) {
                printf("dev %u, channel %u:\n", k, ch);
                int dim = sens0_i2c_read(s0, &buf, ch);
                if (dim < 0) {
                    printf("   Read error: %d\n", dim);
                    continue;
                }
                phydat_dump(&buf, dim);
            }
        }
#ifdef MODULE_SAUL_REG
        puts("SAUL devices:");
        saul_reg_t *dev = saul_reg;

        if (dev == NULL) {
            puts("No SAUL devices present");
            return 1;
        }

        while (dev) {
            int dim = saul_reg_read(dev, &buf);
            printf("\nDev: %s\tType: %s\n", dev->name,
                   saul_class_to_str(dev->driver->type));
            phydat_dump(&buf, dim);
            dev = dev->next;
        }
#endif /* MODULE_SAUL_REG */
        puts("\n##########################");

        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
    }

    return 0;
}
