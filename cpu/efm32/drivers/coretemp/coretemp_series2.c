/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_efm32_drivers_coretemp
 * @{
 *
 * @file
 * @brief       Implementation of the EFM32 internal temperature sensor for
 *              Series 2 devices
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "coretemp.h"

#include "em_device.h"
#include "em_emu.h"

int16_t coretemp_read(void)
{
    /* read factory calibration values */
    int32_t cal_temp = ((DEVINFO->CALTEMP & _DEVINFO_CALTEMP_TEMP_MASK) >>
                        _DEVINFO_CALTEMP_TEMP_SHIFT);
    int32_t cal_value = ((DEVINFO->EMUTEMP & _DEVINFO_EMUTEMP_EMUTEMPROOM_MASK) >>
                         _DEVINFO_EMUTEMP_EMUTEMPROOM_SHIFT);

    /* no factory calibration values */
    if ((cal_temp == 0xFF) || (cal_value == 0x1FF)) {
        return -10000;
    }

    /* read the temperature sensor value in centi-Kelvin (the register has a
     * resolution of 0.25 K) */
    int32_t value = ((EMU->TEMP & (_EMU_TEMP_TEMP_MASK | _EMU_TEMP_TEMPLSB_MASK))
                     >> _EMU_TEMP_TEMPLSB_SHIFT) * 25;

    /* the raw sensor value has a mean error of a few degrees Celsius, which
     * is corrected using the calibration value (value and cal_value are
     * effectively in Kelvin, which cancels out the Kelvin offset of 273.15
     * after subtracting them) */
    return (int16_t)(value + ((cal_temp - cal_value) * 100));
}

int coretemp_init(void)
{
    /* the first temperature measurement is started automatically after reset,
     * so wait for it to complete before the sensor can be read */
    while (!EMU_TemperatureReady()) {}

    return 0;
}
