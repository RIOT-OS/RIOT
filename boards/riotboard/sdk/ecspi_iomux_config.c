/*
 * Copyright (C) 2012, [Your Company Here] All Rights Reserved.
 * IT IS EXPECTED THAT THIS TEXT BE REPLACED WITH THE BOARD SOFTWARE
 * PROVIDER'S COPYRIGHT INFORMATION. THIS TEXT WILL BE DISPLAYED AT
 * THE TOP OF ALL SOURCE FILES GENERATED FOR THIS BOARD DESIGN.
*/

// File: ecspi_iomux_config.c

/* ------------------------------------------------------------------------------
 * <auto-generated>
 *     This code was generated by a tool.
 *     Runtime Version:3.4.0.3
 *
 *     Changes to this file may cause incorrect behavior and will be lost if
 *     the code is regenerated.
 * </auto-generated>
 * ------------------------------------------------------------------------------
*/

#include "iomux_config.h"
#include "registers/regsiomuxc.h"
#include "registers/regsecspi.h"
#include "sdkio.h"
#include <assert.h>

void ecspi_iomux_config(int instance)
{
    switch (instance) {
        case HW_ECSPI1:
            return ecspi1_iomux_config();

        case HW_ECSPI2:
            return ecspi2_iomux_config();

        case HW_ECSPI3:
            return ecspi3_iomux_config();

        case HW_ECSPI4:
            return ecspi4_iomux_config();

        default:
            assert(false);
    }
}

