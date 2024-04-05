/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "board.h"
#include "periph/i2c.h"
#include "vcnl40x0.h"
//#include "xtimer.h"

const int PIN = 9;

int main(void)
{
    //blink if read bytes non-zero
    const vcnl40x0_params_t initParams = {1, 0, 0, 0, 0, 0};
    vcnl40x0_t dev = {initParams};

    int8_t status = vcnl40x0_init(&dev, &initParams);
    if(status == 0){
        printf("Successfully initalised vcnl40x0 !");
        gpio_init(PIN, GPIO_OUT);
        gpio_set(PIN);
        uint16_t readresp = vcnl40x0_read_ambient_light(&dev);
        //if (readresp > 0){
        //    gpio_set(PIN);
        //}
        // blink loop for testing light
        /*for(int j = 0; j < 1000; j++){
            for(long unsigned int i = 0; i < 100 * MHZ(1); i++){
                    __asm("");
            }
            gpio_set(26);
            for(long unsigned int i = 0; i < 100 * MHZ(1); i++){
                    __asm("");
            }
            gpio_clear(26);
        }*/


    }
    else if (status < 0){
        gpio_init(10, GPIO_OUT);
        gpio_set(10);
    }
    else if (status > 0){
        gpio_init(11, GPIO_OUT);
        gpio_set(11);
    }
    else if (status == -3){
        gpio_init(12, GPIO_OUT);
        gpio_set(12);
    }
    else if (status == 4){
        gpio_init(13, GPIO_OUT);
        gpio_set(13);
    }
}
