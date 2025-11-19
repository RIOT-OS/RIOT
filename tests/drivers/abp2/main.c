/*
 * SPDX-FileCopyrightText: 2024 CNRS, France
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the Honeywell ABP2 series
 *              pressure and temperature sensor driver.
 *
 * @author      David Picard <david.picard@clermont.in2p3.fr>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "ztimer.h"
#include "timex.h"
#include "phydat.h"
#include "abp2.h"
#include "abp2_params.h"
#include "saul_reg.h"

#define STATUS_SLEEP_MS         (1)
#define MEASUREMENT_SLEEP_MS    (1000)
#define MAX_LOOPS_STATUS        (10)
#define MAX_LOOPS_MEAS          (3)

/* supported acquisition modes */
enum {
    ABP2_NON_BLOCKING,
    ABP2_BLOCKING,
    ABP2_SAUL
} acqmode = ABP2_NON_BLOCKING;

static abp2_t dev;

int main(void)
{
    int32_t press = 0;
    int32_t temp = 0;
    abp2_params_t prms;
    phydat_t phyPress;
    phydat_t phyTemp;
    void *ptr;
    int cntMeas = 0;
    saul_reg_t *saulDev = saul_reg;
    phydat_t saulData;

    puts("ABP2 Honeywell series pressure and temperature sensor\n");

    if (saulDev == NULL) {
        puts("ERROR: no ABP2_SAUL devices detected");
    }

    ptr = memcpy(&prms, &abp2_params, sizeof(abp2_params));
    if (!ptr)
    {
       puts("main() >> ERROR: memcpy() failed");
       return 1;
    }

    printf("Initializing SPI bus %d\n", prms.spi);
    spi_init(prms.spi);

    printf("Initializing ABP2 at SPI_DEV(%i)... ", prms.spi);

    if (abp2_init(&dev, &prms) == 0)
    {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return -1;
    }

    puts("=========================");
    puts("        Measuring");
    puts("=========================");

    printf("Pressure range = %d .. %d\n", (int)prms.rangeMin, (int)prms.rangeMax);

    while (1) {
        int res;

        ztimer_sleep(ZTIMER_USEC, MEASUREMENT_SLEEP_MS * US_PER_MS);
        switch (acqmode) {
            case ABP2_NON_BLOCKING:
                res = abp2_read_nb(&dev, &press, &temp);
                if (res)
                {
                    printf("abp2_read_nb() >> ERROR errno = %d", res);
                    continue;
                }
                break;
            case ABP2_BLOCKING:
                res = abp2_read(&dev, &press, &temp);
                if (res)
                {
                    printf("abp2_read() >> ERROR errno = %d", res);
                    continue;
                }
                break;
            case ABP2_SAUL:
                while (saulDev) {
                    int dim = saul_reg_read(saulDev, &saulData);
                    printf("\nDev: %s\tType: %s" "\n", saulDev->name,
                            saul_class_to_str(saulDev->driver->type));
                    phydat_dump(&saulData, dim);
                    saulDev = saulDev->next;
                }
                saulDev = saul_reg; /* reset pointer for next read */
                break;
            default:
                acqmode = 0;
        }

        /* display data retrieved in blocking and non-blocking modes: */
        if (acqmode != ABP2_SAUL) {
            phyPress.val[0] = press / 100;  /* let int32_t fit into int16_t */
            phyPress.scale = -4;            /* and set the exponent accordingly */
            phyPress.unit = UNIT_BAR;       /* set the unit */
            phydat_dump(&phyPress, 1);      /* print the value in a pretty format */

            phyTemp.val[0] = temp;          /* temp is already in mdeg C */
            phyTemp.scale = -3;             /* 1 mdegC = 1e-03 degC */
            phyTemp.unit = UNIT_TEMP_C;     /* set the unit */
            phydat_dump(&phyTemp, 1);       /* print the value in a pretty format */
        }

        /* Switch modes periodically: */
        cntMeas++;
        if (cntMeas == MAX_LOOPS_MEAS) {
            cntMeas = 0;
            acqmode++;
            if (acqmode > ABP2_SAUL) {
                acqmode = 0;
            }
            puts("------------------------------");
            switch (acqmode) {
                case ABP2_NON_BLOCKING:
                    puts("Switch to non-blocking mode");
                    break;
                case ABP2_BLOCKING:
                    puts("Switch to blocking mode");
                    break;
                case ABP2_SAUL:
                    puts("Switch to ABP2_SAUL mode");
                    break;
                default:
                    acqmode = 0;
                    puts("ERROR: unsupported mode");
            }
            puts("------------------------------");
        }
    }
    return 0;
}
