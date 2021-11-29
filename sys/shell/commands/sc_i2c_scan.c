/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       An I2C bus scanner
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"
#include "periph/i2c.h"

static int get_dev(i2c_t *dev, int argc, char **argv)
{
    if (argc == 2) {
        int idx = atoi(argv[1]);
        if ((idx < 0) || (idx >= (int)I2C_NUMOF)) {
            printf("I2C device with number \"%s\" not found\n", argv[1]);
        }
        else {
            *dev = I2C_DEV(idx);
            return 0;
        }
    }
    printf("Usage: %s [DEVICE_NUMBER]\n", argv[0]);
    return -1;
}

static inline int is_addr_reserved(uint16_t addr)
{
    if ((addr < 0x0e) || (addr > 0x77))
        return 1;

    return 0;
}

int _i2c_scan(int argc, char **argv)
{
    i2c_t dev;
    if (get_dev(&dev, argc, argv)) {
        return -1;
    }

    printf("Scanning I2C device %s...\n", argv[1]);
    i2c_acquire(dev);

    puts(
        "addr not ack'ed = \"-\", addr ack'ed = \"X\", addr reserved = \"R\", error = \"E\"\n"
        "     0 1 2 3 4 5 6 7 8 9 a b c d e f"
    );

    for (char i = 0; i < 8; i++) {
        char row[] = { '0', 'x', '0' + i, '0', '\0' };
        fputs(row, stdout);
        uint16_t addr = i;
        addr <<= 4;
        for (unsigned j = 0; j < 16; j++) {
            char str[] = { ' ', '-', '\0' };
            if (is_addr_reserved(addr)) {
                str[1] = 'R';
            }
            else {
                char dummy[1];
                int retval;
                while (-EAGAIN == (retval = i2c_read_byte(dev, addr, dummy, 0))) {
                    /* retry until bus arbitration succeeds */
                }

                switch (retval) {
                    case 0:
                        /* success: Device did respond */
                        str[1] = 'X';
                        break;
                    case -ENXIO:
                        /* No ACK --> no device */
                        break;
                    default:
                        /* Some unexpected error */
                        str[1] = 'E';
                        break;
                }
            }

            fputs(str, stdout);
            addr++;
        }
        puts("");
    }

    i2c_release(dev);
    return 0;
}
