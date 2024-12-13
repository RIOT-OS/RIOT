/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dwax509m183x0
 * @{
 *
 * @file
 * @brief       Device driver implementation for the dwax509m183x0
 *
 * @author      Jan Thies <jan.thies@haw-hamburg.de>
 *
 * @}
 */

#include "dwax509m183x0.h"
#include "dwax509m183x0_constants.h"
#include "dwax509m183x0_params.h"

static float dwax509m183x0_response_curve(float const voltage)
{

    float const voltage_2 = voltage * voltage;
    float const voltage_3 = voltage_2 * voltage;

    // Polynomial regression of 3rd degree as per response value diagram
    // depicted in the datasheet for dwax509m183x0
    float const b0 = -0.0163;
    float const b1 = 0.5231;
    float const b2 = 0.1502;
    float const b3 = -0.0103;

    float const distance = b3 * voltage_3 +
                            b2 * voltage_2 + 
                            b1 * voltage +
                            b0;

    return distance < 0 ? 0 : distance;
}

static int adc_bits_to_resulution(int adc_bits)
{
    switch(adc_bits)
    {
        case ADC_RES_6BIT: return 1 << 6;
        case ADC_RES_8BIT: return 1 << 8;
        case ADC_RES_10BIT: return 1 << 10;
        case ADC_RES_12BIT: return 1 << 12;
        case ADC_RES_14BIT: return 1 << 14;
        case ADC_RES_16BIT: return 1 << 16;
    }
    
    return -1;
}

static int convert_adc_to_distance_um(int const adc_raw, int const adc_resolution)
{

    float const dwax_voltage_out = ((float)adc_raw / (float)(adc_resolution - 1)) * DWAX509M183X0_MAX_VOLTAGE_OUTPUT;

    if (dwax_voltage_out < 0 || dwax_voltage_out > DWAX509M183X0_MAX_VOLTAGE_OUTPUT) {
        printf("[ERROR]: Voltage out of range: %f V\n", dwax_voltage_out);
        return -1;
    }


    float const distance_mm = dwax509m183x0_response_curve(dwax_voltage_out);

    if (distance_mm < 0) {
        printf("[ERROR]: Computed distance is negative: %f mm\n", distance_mm);
        return -1;
    }

    // Convert distance to micrometers and return as an integer
    int const distance_um_pruned = (int)(distance_mm * 1000.f);

    return distance_um_pruned;
}

int dwax509m183x0_init(dwax509m183x0_t* dev, const dwax509m183x0_params_t* params)
{
    *dev = *params;

    if(adc_init(dev->adc_line) == -1)
    {
        printf("[ERROR]: Couldn't initialize adc line %i\n", dev->adc_line);
        dev->adc_line = 0;
        dev->resolution = -1;
        return -1;
    }

    return 0; 
}

int dwax509m183x0_distance_um(dwax509m183x0_t* dev)
{
    int const sample_raw = adc_sample(dev->adc_line, dev->resolution);

    int const distance_um = convert_adc_to_distance_um(
                                sample_raw, adc_bits_to_resulution(dev->resolution)
                            );

    return distance_um;
}



