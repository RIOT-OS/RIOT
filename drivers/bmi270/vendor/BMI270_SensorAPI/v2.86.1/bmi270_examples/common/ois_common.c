/**
 * Copyright (C) 2023 Bosch Sensortec GmbH. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ois_common.h"

/*! Structure to hold interface configurations */
static struct coines_ois_intf_config ois_intf_conf;

/******************************************************************************/
/*!                User interface functions                                   */

/*!
 * SPI read function map to COINES platform
 */
BMI2_INTF_RETURN_TYPE bmi2_ois_spi_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    struct coines_ois_intf_config intf_info = *(struct coines_ois_intf_config *)intf_ptr;

    return coines_read_spi(intf_info.bus, intf_info.dev_addr, reg_addr, reg_data, (uint16_t)len);
}

/*!
 * SPI write function map to COINES platform
 */
BMI2_INTF_RETURN_TYPE bmi2_ois_spi_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr)
{
    struct coines_ois_intf_config intf_info = *(struct coines_ois_intf_config *)intf_ptr;

    return coines_write_spi(intf_info.bus, intf_info.dev_addr, reg_addr, (uint8_t *)reg_data, (uint16_t)len);
}

/*!
 * Delay function map to COINES platform
 */
void bmi2_ois_delay_us(uint32_t period, void *intf_ptr)
{
    coines_delay_usec(period);
}

#if !defined(MCU_APP20)

/*!
 *  @brief Function to initialize the OIS (SPI) interface.
 */
int8_t bmi2_ois_interface_init(struct bmi2_ois_dev *bmi2_ois)
{
    int8_t rslt = BMI2_OK;

    if (bmi2_ois != NULL)
    {
        /* Bus configuration : SPI */
        printf("\nSPI Interface \n");

        /* To initialize the user SPI function */
        bmi2_ois->ois_read = bmi2_ois_spi_read;
        bmi2_ois->ois_write = bmi2_ois_spi_write;

        coines_config_spi_bus(COINES_SPI_BUS_1, COINES_SPI_SPEED_10_MHZ, COINES_SPI_MODE3);
        coines_set_pin_config(COINES_MINI_SHUTTLE_PIN_2_5, COINES_PIN_DIRECTION_OUT, COINES_PIN_VALUE_HIGH);

        /* Assign device address and bus instance to interface pointer */
        ois_intf_conf.bus = COINES_SPI_BUS_1;
        ois_intf_conf.dev_addr = COINES_MINI_SHUTTLE_PIN_2_5;

        bmi2_ois->intf_ptr = ((void *)&ois_intf_conf);

        /* Configure delay in microseconds */
        bmi2_ois->ois_delay_us = bmi2_ois_delay_us;
    }
    else
    {
        rslt = BMI2_E_NULL_PTR;
    }

    return rslt;
}

#endif
