/*
 * Copyright (c) 2020 ML!PA Consulting GmbH
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
 * @brief       Application for testing low-level QSPI driver implementations
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "od.h"
#include "periph/qspi.h"
#include "shell.h"

static qspi_t _get_dev(int argc, char **argv, bool *oob)
{
    qspi_t dev = QSPI_DEV(0);
    *oob = false;

    if (argc > 1) {
        unsigned idx = atoi(argv[1]);
        if (idx >= QSPI_NUMOF) {
            printf("%s: invalid device: %s\n", argv[0], argv[1]);
            *oob = true;
        }

        dev = QSPI_DEV(idx);
    }

    return dev;
}

static int cmd_read(int argc, char **argv)
{
    bool oob;
    qspi_t dev = _get_dev(argc, argv, &oob);
    uint32_t addr, len;

    if (oob) {
        return -1;
    }

    if (argc < 4) {
        printf("usage: %s <dev> <addr> <len>\n", argv[0]);
        return -1;
    }

    addr = atoi(argv[2]);
    len  = atoi(argv[3]);

    void *buffer = malloc(len);
    if (buffer == NULL) {
        puts("out of memory");
        return -1;
    }

    qspi_acquire(dev);
    qspi_read(dev, addr, buffer, len);
    qspi_release(dev);

    od_hex_dump(buffer, len, 0);

    free(buffer);

    return 0;
}

static int cmd_erase(int argc, char **argv)
{
    bool oob;
    qspi_t dev = _get_dev(argc, argv, &oob);
    uint32_t addr;
    uint8_t mode;

    if (oob) {
        return -1;
    }

    if (argc < 4) {
        printf("usage: %s <dev> <addr> <mode>\n", argv[0]);
        printf("\tmode 0: sector erase\n");
        printf("\tmode 1: block erase\n");
        printf("\tmode 2: chip erase\n");
        return -1;
    }

    addr = atoi(argv[2]);
    mode = atoi(argv[3]);

    const qspi_erase_size_t cmds[] = {
        QSPI_ERASE_4K,
        QSPI_ERASE_64K,
        QSPI_ERASE_CHIP,
    };

    if (mode > ARRAY_SIZE(cmds)) {
        printf("%s: invalid mode\n", argv[0]);
        return -1;
    }

    qspi_acquire(dev);
    qspi_erase(dev, addr, cmds[mode]);
    qspi_release(dev);

    return 0;
}
static int cmd_write(int argc, char **argv)
{
    bool oob;
    qspi_t dev = _get_dev(argc, argv, &oob);
    uint32_t addr, len;

    if (oob) {
        return -1;
    }

    if (argc < 4) {
        printf("usage: %s <dev> <addr> <data>\n", argv[0]);
        return -1;
    }

    addr = atoi(argv[2]);
    len  = strlen(argv[3]);

    qspi_acquire(dev);
    qspi_write(dev, addr, argv[3], len);
    qspi_release(dev);

    return 0;
}

static int cmd_jedec(int argc, char **argv)
{
    bool oob;
    qspi_t dev = _get_dev(argc, argv, &oob);

    if (oob) {
        return -1;
    }

    struct __attribute__ ((packed)) {
        uint8_t manuf;
        uint16_t device;
        uint8_t unique[17];
    } jedec_id;

    qspi_acquire(dev);
    qspi_cmd_read(dev, SFLASH_CMD_READ_JEDEC_ID, &jedec_id, sizeof(jedec_id));
    qspi_release(dev);

    printf("Manufacturer ID: %x\n", jedec_id.manuf);
    printf("Device ID: %x\n", jedec_id.device);
    printf("Unique ID:");
    for (unsigned i = 0; i < sizeof(jedec_id.unique); ++i) {
        printf(" %02x", jedec_id.unique[i]);
    }
    puts("");

    return 0;
}

static qspi_mode_t _qspi_mode(unsigned mode)
{
    switch (mode) {
    case 3: return QSPI_MODE_3;
    case 2: return QSPI_MODE_2;
    case 1: return QSPI_MODE_1;
    default:
    case 0: return QSPI_MODE_0;
    }
}

static int cmd_cfg(int argc, char **argv)
{
    bool oob;
    qspi_t dev = _get_dev(argc, argv, &oob);

    if (oob) {
        return -1;
    }

    if (argc < 4) {
        printf("usage: %s <dev> <addr_len> <freq MHz> [mode]\n", argv[0]);
        return -1;
    }

    qspi_mode_t mode  = QSPI_MODE_0;
    unsigned addr_len = atoi(argv[2]);
    unsigned freq_mhz = atoi(argv[3]);

    uint32_t flags = QSPI_FLAG_4BIT
                   | (addr_len == 4 ? QSPI_FLAG_ADDR_32BIT : 0);

    if (argc > 3) {
        mode = _qspi_mode(atoi(argv[4]));
    }

    printf("QSPI%s: %u-bit, %u MHz\n", argv[1], 8 * addr_len, freq_mhz);

    qspi_configure(dev, mode, flags,  MHZ(freq_mhz));

    return 0;
}
static const shell_command_t shell_commands[] = {
    { "jedec", "Read JEDEC ID of the QSPI flash", cmd_jedec },
    { "init", "Configure the QSPI peripheral", cmd_cfg },
    { "read", "Read a region of memory on the QSPI flash", cmd_read },
    { "erase", "Erase a region of memory on the QSPI flash", cmd_erase },
    { "write", "Write a region of memory on the QSPI flash", cmd_write },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("Manual QSPI peripheral driver test");
    printf("There are %u QSPI peripherals.\n", QSPI_NUMOF);

    /* run the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
