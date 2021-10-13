/*
 * Copyright (C) 2015 Daniel Krebs
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
 * @brief       Provides shell commands to test AT30TSE75x temperature sensor
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "at30tse75x.h"

#ifdef MODULE_AT30TSE75X

static bool initialized = false;
static at30tse75x_t dev;

int _at30tse75x_handler(int argc, char **argv)
{
    if(argc <= 1) {
        printf("Usage: %s init|read|mode|resolution|save|restore|config\n", argv[0]);
        return -1;
    }

    if(strcmp(argv[1], "init") == 0) {
        if(argc == 2) {
            printf("Usage: %s init #I2C [addr]\n"
                   "  e.g. %s init 0\n", argv[0], argv[0]);
            return -1;
        }

        int error;
        unsigned addr = 0x48;   /* default to A0-A2 connected to GND */

        /* Try to parse i2c dev */
        i2c_t i2c_dev = (i2c_t) strtol(argv[2], &argv[1] + 1, 10);

        /* Address given */
        if(argc == 4) {
            char* hex = strstr(argv[3], "0x");
            if(hex) {
                addr = strtoul(hex+2, NULL, 16);
            }
        }

        error = at30tse75x_init(&dev, i2c_dev, addr);
        if (error) {
            printf("Error initializing AT30TSE75x sensor on I2C #%u @ 0x%x\n", i2c_dev, addr);
            initialized = false;
            return 1;
        }
        else {
            printf("Initialized AT30TSE75x sensor on I2C #%u @ 0x%x\n", i2c_dev, addr);
            initialized = true;
        }
    } else {
        if(!initialized) {
            puts("Please initialize first");
            return -1;
        }
        if(strcmp(argv[1], "read") == 0) {
            float temperature;
            if(at30tse75x_get_temperature(&dev, &temperature) != 0) {
                puts("Reading temperature failed");
                return -1;
            }
            printf("Temperature: %i.%03u °C\n",
                    (int)temperature,
                    (unsigned)((temperature - (int)temperature) * 1000));
        } else if(strcmp(argv[1], "mode") == 0) {
            if(argc == 2) {
                printf("Usage: %s mode one-shot|comparator|interrupt\n", argv[0]);
                return -1;
            }
            at30tse75x_mode_t mode;
            if(strcmp(argv[2], "one-shot") == 0) {
                mode = AT30TSE75X_MODE_ONE_SHOT;
            } else if(strcmp(argv[2], "comparator") == 0) {
                mode = AT30TSE75X_MODE_COMPARATOR;
            } else if(strcmp(argv[2], "interrupt") == 0) {
                mode = AT30TSE75X_MODE_INTERRUPT;
            } else {
                puts("Invalid mode");
                return -1;
            }
            if(at30tse75x_set_mode(&dev, mode) != 0) {
                return -1;
            }
            printf("Mode set to %s\n", argv[2]);
        } else if(strcmp(argv[1], "resolution") == 0) {
            if(argc == 2) {
                printf("Usage: %s resolution 9|10|11|12\n", argv[0]);
                return -1;
            }
            unsigned resolution = strtoul(argv[2], NULL, 10);
            if(at30tse75x_set_resolution(&dev, resolution - 9) != 0) {
                return -1;
            }
            printf("Resolution set to %u bits\n", resolution);
        } else if(strcmp(argv[1], "save") == 0) {
            uint8_t config;
            if(at30tse75x_get_config(&dev, &config) != 0) {
                return -1;
            }
            if(at30tse75x_save_config(&dev) != 0) {
                return -1;
            }
            printf("Config (0x%x) saved\n", config);
        } else if(strcmp(argv[1], "restore") == 0) {
            uint8_t config;
            if(at30tse75x_restore_config(&dev) != 0) {
                return -1;
            }
            if(at30tse75x_get_config(&dev, &config) != 0) {
                return -1;
            }
            printf("Config restored to 0x%x\n", config);

        } else if(strcmp(argv[1], "config") == 0) {
            if(argc == 2) {
                uint8_t config;
                if(at30tse75x_get_config(&dev, &config) != 0) {
                    return -1;
                }
                printf("Config: 0x%x\n", config);
            } else {
                /* Try to parse config in hex format */
                uint8_t config;
                char* hex = strstr(argv[2], "0x");
                if(!hex) {
                    printf("Usage: %s config 0x__"
                           "  to set config\n", argv[0]);
                    return -1;
                }
                config = strtoul(hex+2, NULL, 16);
                if(at30tse75x_set_config(&dev, config) != 0) {
                    return -1;
                }
                printf("Config set to: 0x%x\n", config);
            }
        }
    }
    return 0;
}

#endif /* MODULE_AT30TSE75X */
