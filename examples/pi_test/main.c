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
//#include "test_driver/test_driver.h"
//#include "xtimer.h"

const int PIN = 9;

int useTestDevice(void);

int main(void)
{
    /*if(0){
        return useTestDevice();
    }*/
    //blink if read bytes non-zero
    //i2c addr retrived from:
    // https://github.com/sparkfun/SparkFun_VCNL4040_Arduino_Library/blob/master/src/SparkFun_VCNL4040_Arduino_Library.cpp
    const vcnl40x0_params_t initParams = {0x01, 0x20, 0, 0, 0, 0};
    vcnl40x0_t dev = {initParams};

    int8_t status = vcnl40x0_init(&dev, &initParams);
    if(status == 0){
        printf("Successfully initalised vcnl40x0 !");
        gpio_init(PIN, GPIO_OUT);
        gpio_init(PIN-1, GPIO_OUT);
        //gpio_set(PIN);
        uint16_t readresp = vcnl40x0_read_ambient_light(&dev);
        if (readresp > 0){
            gpio_set(PIN);
        }
        else{
            gpio_set(PIN-1);
        }
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
    else if (status == -1){
        gpio_init(10, GPIO_OUT);
        gpio_set(10);
    }
    else if (status == -2){
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

/*int useTestDevice(void){
    gpio_init(PIN-1, GPIO_OUT);
    gpio_set(PIN-1);
    test_dev_t device = {2,0};
    int result = test_driver_read_value(&device);
    if(result == test_driver_reading){
        gpio_init(PIN, GPIO_OUT);
        gpio_set(PIN);
    }
    return 0;
}*/
