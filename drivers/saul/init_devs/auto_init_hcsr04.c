#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "hcsr04.h"
#include "hcsr04_params.h"

/**
* @brief   Define the number of configured sensors
*/
#define HCSR04_NUM             ARRAY_SIZE(hcsr04_params)

/**
* @brief   Allocate memory for the device descriptors
*/
static hcsr04_t hcsr04_devs[HCSR04_NUM];

/**
* @brief   Memory for the SAUL registry entries
*/
static saul_reg_t saul_entries[HCSR04_NUM];

/**
* @brief   Define the number of saul info
*/
#define HCSR04_INFO_NUM        ARRAY_SIZE(hcsr04_saul_info)

/**
* @brief   Reference the driver struct
*/
extern const saul_driver_t hcsr04_distance_saul_driver;

void auto_init_hcsr04(void)
{
    assert(HCSR04_NUM == HCSR04_INFO_NUM);

    for (unsigned i = 0; i < HCSR04_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hcsr04 #%u\n", i);

        if (hcsr04_init(&hcsr04_devs[i], &hcsr04_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing hcsr04 #%u\n", i);
            continue;
        }
        /* temperature */
        saul_entries[i].dev = &(hcsr04_devs[i]);
        saul_entries[i].name = hcsr04_saul_info[i].name;
        saul_entries[i].driver = &hcsr04_distance_saul_driver;

        saul_reg_add(&(saul_entries[i]));
    }
}