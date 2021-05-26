#include <stdio.h>
#include "xtimer.h"
#include "assert.h"

#include "phydat.h"
#include "saul_reg.h"

#include "hcsr04_params.h"
#include "hcsr04.h"


#define TEST_TIME                               (3e6)
#define TEST_CONVERTION_MACRO(x)                


int main(void) {
    puts("********** Test HC-SR04 RIOT driver! **********");

    // deactivate saul auto-initialization, due to conflict when both work at the same time
    saul_reg_rm(saul_reg_find_name(hcsr04_saul_info[0].name));

    int retval;
    hcsr04_t dev;
    uint32_t timer;

    // ================ Initialize the device ================
    puts("\n********** Testing hcsr04 sensor initialization **********\n");
    retval = hcsr04_init(&dev, &hcsr04_params[0]);
    if (retval != 0) {
        printf("Initialization failed with error code %d\n", retval);
        return retval;
    }
    puts("\n********** HC-SR04 initialized succesfully! **********\n"); 

    // ================ Test Temperature setup ================
    puts("\n********** Testing temp/speed of sound calculation **********\n");

    hcsr04_set_temp(&dev, 23500);               // 23.5 Celsius
    
    hcsr04_set_temp(&dev, 24500);               // 23.5 Celsius
    
    hcsr04_set_temp(&dev, 25500);               // 23.5 Celsius
    
    hcsr04_set_temp(&dev, 26500);               // 23.5 Celsius
    

    // ================ Test Reading with trigger/read ================
    puts("\n********** Testing hcsr04 trigger/read **********\n");
    
    uint16_t distance;
    timer = xtimer_now_usec();
    while ((xtimer_now_usec() - timer) < TEST_TIME) {
        assert(hcsr04_trigger(&dev) == 0);
        xtimer_msleep(30);
        assert(hcsr04_read(&dev, &distance) == 0);
        printf("Distance measure: %i mm\n", distance);
        xtimer_msleep(170);
    }

    // ================ Test Reading with get_distance ================
    puts("\n********** Testing hcsr04 get_distance **********\n");
    
    timer = xtimer_now_usec();
    while ((xtimer_now_usec() - timer) < TEST_TIME) {
        assert(hcsr04_get_distance(&dev, &distance) == 0);
        printf("Distance measure: %i mm\n", distance);
        xtimer_msleep(170);
    }

    // ================ Test reading with SAUL ================
    puts("\n********** Testing hcsr04 saul read **********\n");
    
    // settings up again saul registry
    phydat_t res;
    saul_reg_t hcsr04_saul_entry;

    extern const saul_driver_t hcsr04_distance_saul_driver;

    hcsr04_saul_entry.dev = &(dev);
    hcsr04_saul_entry.name = hcsr04_saul_info[0].name;
    hcsr04_saul_entry.driver = &hcsr04_distance_saul_driver;

    saul_reg_add(&hcsr04_saul_entry);    

    timer = xtimer_now_usec();
    while ((xtimer_now_usec() - timer) < TEST_TIME) {
        int dim = saul_reg_read(&hcsr04_saul_entry, &res);
        phydat_dump(&res, dim);
        printf("%i\n", dim);
        xtimer_msleep(170);
    }

    puts("\n********** HC-SR04 testing finished **********\n");

    return 0;
}



