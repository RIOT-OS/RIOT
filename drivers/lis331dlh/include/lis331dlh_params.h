#pragma once

#include "lis331dlh.h"

static const lis331dlh_params_t lis331dlh_params[] = {
    {
        .i2c = I2C_DEV(0),
        .addr = LIS331DLH_ADDR_DEFAULT
    }
};
