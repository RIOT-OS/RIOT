 /* Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides a shell command to access bus statistics
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus_stats.h"
#include "modules.h"
#ifdef MODULE_PERIPH_GPIO_LL
#include "periph/gpio_ll.h"
#endif
#include "shell.h"
#include "ztimer.h"

static int _bus_stats_cmd(int argc, char **argv)
{
    if (argc < 1) {
        return -EINVAL;
    }

    if (argc == 2) {
        if (0 == strcmp("clear", argv[1])) {
            if (IS_USED(MODULE_BUS_STATS_SPI)) {
                for (size_t idx = 0; idx < bus_stats_spi_numof(); idx++) {
                    bus_stats_spi_clear(idx);
                }
            }

            if (IS_USED(MODULE_BUS_STATS_I2C)) {
                for (size_t idx = 0; idx < bus_stats_i2c_numof(); idx++) {
                    bus_stats_i2c_clear(idx);
                }
            }

            return 0;
        }
        else {
            printf("Usage: %s [clear]\n", argv[0]);
            return -EINVAL;
        }
    }

    if (IS_USED(MODULE_BUS_STATS_SPI)) {
        for (size_t idx = 0; idx < bus_stats_spi_numof(); idx++) {
            bus_stats_t stats;
            const bus_stats_spi_endpoint_t *ep = bus_stats_spi_endpoint_get(idx);
            bus_stats_spi_get(&stats, idx);
            if (ep->cs == SPI_CS_UNDEF) {
                printf("%s @ SPI bus #%u, CS undefined:\n", ep->name, (unsigned)ep->bus);
            }
            else {
#ifdef MODULE_PERIPH_GPIO_LL
                unsigned cs_port = GPIO_PORT_NUM(gpio_get_port(ep->cs));
                unsigned cs_pin = gpio_get_pin_num(ep->cs);
                printf("%s @ SPI bus #%u, CS on P%c%u / P%u.%u:s\n",
                       ep->name, (unsigned)ep->bus,
                       'A' + cs_port, cs_pin, cs_port, cs_pin);

#else
                printf("%s @ SPI bus #%u:\n", ep->name, (unsigned)ep->bus);
#endif
            }
            bus_stats_print(&stats);
        }
    }

    if (IS_USED(MODULE_BUS_STATS_I2C)) {
        for (size_t idx = 0; idx < bus_stats_i2c_numof(); idx++) {
            bus_stats_t stats;
            const bus_stats_i2c_endpoint_t *ep = bus_stats_i2c_endpoint_get(idx);
            bus_stats_i2c_get(&stats, idx);
            printf("%s @ I2C bus #%u address 0x%04x:\n",
                   ep->name, (unsigned)ep->bus, (unsigned)ep->addr);
            bus_stats_print(&stats);
        }
    }

    return 0;
}

SHELL_COMMAND(bus_stats, "access bus statistics", _bus_stats_cmd);
