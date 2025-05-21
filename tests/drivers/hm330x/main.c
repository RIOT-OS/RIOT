/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       HM330X driver test application
 *
 * @author      Francisco Molina <francois-xavier.molinas@inria.fr>
 *
 * @}
 */

#include "fmt.h"
#include "fmt_table.h"
#include "time_units.h"
#include "ztimer.h"

#include "hm330x.h"
#include "hm330x_params.h"

int main(void)
{
    hm330x_t dev;

    print_str("HM330X test application\n");

    print_str("+------------Initializing------------+\n");

    /* initialize the sensor with default configuration parameters */
    if (hm330x_init(&dev, &hm330x_params[0])) {
        print_str("Initialization failed\n");
        return 1;
    }

#if IS_USED(MODULE_HM3302)
    print_str(
        "+------------------------+------------------------+----------------------------------------------+\n"
        "| Standard concentration | Atmospheric Environment|   # Particles in 0.1l air of diameter >=     |\n"
        "| PM1.0 | PM2.5 | PM10.0 | PM1.0 | PM2.5 | PM10.0 | 0.3µm | 0.5µm | 1.0µm | 2.5µm | 5.0µm | 10µm |\n"
        "+-------+-------+--------+-------+-------+--------+-------+-------+-------+-------+-------+------+\n"
    );
#else
    print_str(
        "+------------------------+------------------------+\n"
        "| Standard concentration | Atmospheric Environment|\n"
        "| PM1.0 | PM2.5 | PM10.0 | PM1.0 | PM2.5 | PM10.0 |\n"
        "+-------+-------+--------+-------+-------+--------+\n"
    );
#endif

    hm330x_data_t data;
    while (1) {
        ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

        /* read the data and print them on success */
        if (hm330x_read(&dev, &data) == 0) {
            print("|", 1);
            print_col_u32_dec(data.mc_pm_1, 7);
            print("|", 1);
            print_col_u32_dec(data.mc_pm_2p5, 7);
            print("|", 1);
            print_col_u32_dec(data.mc_pm_10, 8);
            print("|", 1);
            print_col_u32_dec(data.amc_pm_1, 7);
            print("|", 1);
            print_col_u32_dec(data.amc_pm_2p5, 7);
            print("|", 1);
            print_col_u32_dec(data.amc_pm_10, 8);
#if IS_USED(MODULE_HM3302)
            print("|", 1);
            print_col_u32_dec(data.nc_pm_0p3, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_0p5, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_1, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_2p5, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_5, 7);
            print("|", 1);
            print_col_u32_dec(data.nc_pm_10, 6);
#endif
            print("|\n", 2);
        }
        else {
            print_str("Could not read data from sensor\n");
        }
    }

    return 0;
}
