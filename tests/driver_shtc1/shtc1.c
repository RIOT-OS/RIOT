/*
 * Copyright (C) 2017 RWTH Aachen, Steffen Robertz, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       SHTC1 temperature and humidity sensor shell command implementation
 *
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 *
 * @}
 */

#define ENABLE_DEBUG    (0)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "periph_conf.h"
#include "debug.h"
#include "shtc1.h"
#include "shtc1_params.h"

int shtc1_cmd(int argc, char **argv){
    DEBUG("shtc1_cmd called \n");
    if (argc != 2) {
        printf("usage: %s  [temp/humidity/all]", argv[0]);
        return 1;
    }
    shtc1_t shtc;
    if (shtc1_init(&shtc, &shtc1_params[0])!= SHTC1_OK){
        printf("can't initialize the sensor");
        return 1;
    }
    if (shtc1_measure(&shtc) == SHTC1_OK) {
        if (!strcmp(argv[1], "temp")) {
            /*print temp value*/
            printf("Temperature: %.2f C\n", shtc.values.temp);
            return 0;
        }
        else if (!strcmp(argv[1], "humidity")) {
            /*print temp value*/
            printf("Humidity: %.2f%%\n", shtc.values.rel_humidity);
            return 0;
        }
        else if (!strcmp(argv[1], "all")) {
            /*print temp value*/
            printf("Temperature: %.2f C \n Humidity: %.2f%%\n", shtc.values.temp, shtc.values.rel_humidity);
            return 0;
        }
        else {
            printf("usage: %s  [temp/humidity/all]", argv[0]);
            return 1;
        }
    }
    return 0;
}
