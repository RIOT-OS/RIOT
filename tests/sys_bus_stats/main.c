/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for bus stats
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>

#include "bus_stats.h"
#include "test_utils/expect.h"
#include "shell.h"

#if defined(MODULE_BME680_SPI) || (MODULE_BME680_I2C)
#include "bme680_params.h"
#endif

#if defined(MODULE_BMX280_SPI) || (MODULE_BMX280_I2C)
#include "bmx280_params.h"
#endif

/* Adding example configuration for BMx280 and BME680, as they can be used
 * either in SPI or in I2C mode they are ideal test subjects. */

#ifdef MODULE_BME680_SPI
XFA(bus_stats_spi_xfa, 0) bus_stats_spi_t _bme680_spi_bus_stats = {
    .endpoint = {
        .bus = BME680_PARAM_SPI_DEV,
        .cs = BME680_PARAM_SPI_NSS_PIN,
        .name = "BME680",
    }
};
#endif

#ifdef MODULE_BME680_I2C
XFA(bus_stats_i2c_xfa, 0) bus_stats_i2c_t _bme680_i2c_bus_stats = {
    .endpoint = {
        .bus = BME680_PARAM_I2C_DEV,
        .addr = BME680_PARAM_I2C_ADDR,
        .name = "BME680",
    }
};
#endif

#ifdef MODULE_BMX280_SPI
XFA(bus_stats_spi_xfa, 0) bus_stats_spi_t _bmx280_spi_bus_stats = {
    .endpoint = {
        .bus = BMX280_PARAM_SPI,
        .cs = BMX280_PARAM_CS,
        .name = "BMx280",
    }
};
#endif

#ifdef MODULE_BMX280_I2C
XFA(bus_stats_i2c_xfa, 0) bus_stats_i2c_t _bmx280_i2c_bus_stats = {
    .endpoint = {
        .bus = BMX280_PARAM_I2C_DEV,
        .addr = BMX280_PARAM_I2C_ADDR,
        .name = "BMx280",
    }
};
#endif

int main(void)
{
    /* bus usage statistics are not sane until the first time stats are cleared
     * due to last_clear_us member being nonsense on boot. So let's just
     * clear them now. */
    for (size_t idx = 0; idx < bus_stats_i2c_numof(); idx++) {
        bus_stats_i2c_clear(idx);
    }
    for (size_t idx = 0; idx < bus_stats_spi_numof(); idx++) {
        bus_stats_spi_clear(idx);
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
