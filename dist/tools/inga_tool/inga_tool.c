/* Reset INGA nodes and configure their FTDI
 *
 * (C) 2011 Daniel Willmann <daniel@totalueberwachung.de>
 * (C) 2014 Sven Hesse <drmccoy@drmccoy.de>
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include <popt.h>

#include "inga_usb.h"

enum {
    MODE_RESET,
    MODE_UPDATE_EEPROM,
    MODE_READ_SERIAL,
    MODE_EUI64,
};

struct ftdi_eemem_info {
    const char *manufacturer;
    const char *product;
    const char *serial;
    int chip_type;
    int vendor_id;
    int product_id;
    int max_power;
    int cbus[5];
};

struct config_t {
    struct inga_usb_config_t usb;

    int mode;
    int verbose;
    int eep_cbusio;
    struct ftdi_eemem_info eep;
};

#define VERBOSE(args ...) \
    if (cfg->verbose) \
        fprintf(stderr, ## args)


void inga_reset(struct config_t *cfg)
{
    int rc;
    struct inga_usb_ftdi_t *ftdi;
    struct inga_usb_device_t *usbdev = NULL;

    if (inga_usb_ftdi_init(&ftdi) < 0) {
        fprintf(stderr, "ftdi_init failed\n");
        exit(EXIT_FAILURE);
    }

    /* Find the USB device for the path */
    usbdev = inga_usb_find_device(&cfg->usb, cfg->verbose);
    if (!usbdev) {
        fprintf(stderr, "Could not find device\n");
        exit(EXIT_FAILURE);
    }

    rc = inga_usb_ftdi_open(ftdi, usbdev);
    if (rc < 0) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", rc, inga_usb_ftdi_get_error_string(ftdi));
        exit(EXIT_FAILURE);
    }

    printf("Resetting INGA node...");
    fflush(stdout);

    /* Set CBUS3 to output and high */
    inga_usb_ftdi_set_bitmode(ftdi, 0x88, BITMODE_CBUS);

    /* sleep(1); */

    /* Set CBUS3 to output and low */
    inga_usb_ftdi_set_bitmode(ftdi, 0x80, BITMODE_CBUS);

    VERBOSE("done\n");

    inga_usb_ftdi_set_bitmode(ftdi, 0, BITMODE_RESET);

out:

    VERBOSE("Resetting USB device\n");

    inga_usb_ftdi_reset(ftdi);
    inga_usb_ftdi_close(ftdi);
    inga_usb_ftdi_deinit(ftdi);

    inga_usb_free_device(usbdev);
}

void inga_read_eeprom(struct config_t *cfg, struct inga_usb_ftdi_t *ftdi, struct ftdi_eemem_info *info)
{
    int rc;
    struct inga_usb_device_t *usbdev = NULL;

    if (inga_usb_ftdi_init(&ftdi) < 0) {
        fprintf(stderr, "ftdi_init failed\n");
        exit(EXIT_FAILURE);
    }

    /* Find the USB device for the path */
    usbdev = inga_usb_find_device(&cfg->usb, cfg->verbose);
    if (!usbdev) {
        fprintf(stderr, "Could not find device\n");
        exit(EXIT_FAILURE);
    }

    rc = inga_usb_ftdi_open(ftdi, usbdev);
    if (rc < 0) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", rc, inga_usb_ftdi_get_error_string(ftdi));
        exit(EXIT_FAILURE);
    }

    VERBOSE("Reading out EEPROM image...");
    fflush(stdout);
    rc = inga_usb_ftdi_eeprom_read(ftdi);
    if (rc < 0) {
        fprintf(stderr, "\nCould not read and decode EEPROM: %i\n", rc);
        exit(EXIT_FAILURE);
    }
    VERBOSE("done\n\n");

    if ((inga_usb_ftdi_eeprom_get_value(ftdi, CHIP_TYPE, &info->chip_type) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, VENDOR_ID, &info->vendor_id) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, PRODUCT_ID, &info->product_id) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, MAX_POWER, &info->max_power) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_0, &info->cbus[0]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_1, &info->cbus[1]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_2, &info->cbus[2]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_3, &info->cbus[3]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_4, &info->cbus[4]) < 0)) {

        fprintf(stderr, "Could not read the EEPROM values\n");
        exit(EXIT_FAILURE);
    }

    if ((inga_usb_ftdi_eeprom_get_string(ftdi, MANUFACTURER_STRING, &info->manufacturer) < 0) ||
        (inga_usb_ftdi_eeprom_get_string(ftdi, PRODUCT_STRING, &info->product) < 0) ||
        (inga_usb_ftdi_eeprom_get_string(ftdi, SERIAL_STRING, &info->serial) < 0)) {

        fprintf(stderr, "Could not read the EEPROM manufacturer, product and serial strings\n");
        exit(EXIT_FAILURE);
    }

    VERBOSE("Resetting USB device\n");

    inga_usb_ftdi_reset(ftdi);
    inga_usb_ftdi_close(ftdi);
    inga_usb_ftdi_deinit(ftdi);

    inga_usb_free_device(usbdev);
}

void inga_print_eeinfo(struct config_t *cfg)
{
    struct inga_usb_ftdi_t *ftdi;
    struct ftdi_eemem_info info;

    inga_read_eeprom(cfg, ftdi, &info);


    printf("EEPROM config:\n"
           "Chip type:    %i\n"
           "VID:          0x%04x\n"
           "PID:          0x%04x\n"
           "Manufacturer: %s\n"
           "Product:      %s\n"
           "Serial:       %s\n"
           "Max power:    %i\n"
           "CBUS:         %1x%1x %1x%1x 0%1x\n\n",
           info.chip_type, info.vendor_id, info.product_id, info.manufacturer, info.product, info.serial, info.max_power * 2,
           info.cbus[1], info.cbus[0], info.cbus[3], info.cbus[2], info.cbus[4]);
}

uint64_t inga_serial_to_id(const char *serial)
{
    int i = 0;
    int character = 0;
    uint64_t return_value = 0;

    for (i = 0; i < 8; i++) {
        character = serial[i] - 48; // "0" == 48, Z == 42

        if (character > 42) {
            character = 42;
        }

        return_value = (return_value << 6) | (character & 0x3F);
    }

    /* Insert FFFE as required by http://msdn.microsoft.com/en-us/library/aa915616.aspx */
    return_value = ((return_value & 0xFFFFFF000000) << 16) | (((uint64_t) 0xFEFF) << 24) | (return_value & 0x00000000FFFFF);

    /* Insert 00 */
    return_value &= 0xFFFFFFFFFFFFFF7F;

    return return_value;
}

void inga_eui64(struct config_t *cfg)
{
    struct inga_usb_ftdi_t *ftdi;
    struct ftdi_eemem_info info;

    const char *serial = NULL;
    uint8_t *pointer = NULL;

    inga_read_eeprom(cfg, ftdi, &info);

    /* Obtain EUI64 */
    uint64_t id = inga_serial_to_id(info.serial);
    pointer = (uint8_t *) &id;

    printf("EUI64: ");
    int i;
    for (i = 0; i < 8; i++) {
        printf("%02X", pointer[i]);

        if (i < 7) {
            printf(":");
        }
    }
    printf("\n");
}

void inga_eeprom(struct config_t *cfg)
{
    int rc;
    struct inga_usb_ftdi_t *ftdi;
    struct inga_usb_device_t *usbdev = NULL;
    const char *manufacturer = NULL, *product = NULL, *serial = NULL;
    int chip_type, vid, pid, max_power, cbus[5];

    if (inga_usb_ftdi_init(&ftdi) < 0) {
        fprintf(stderr, "ftdi_init failed\n");
        exit(EXIT_FAILURE);
    }

    /* Find the USB device for the path */
    usbdev = inga_usb_find_device(&cfg->usb, cfg->verbose);
    if (!usbdev) {
        fprintf(stderr, "Could not find device\n");
        exit(EXIT_FAILURE);
    }

    rc = inga_usb_ftdi_open(ftdi, usbdev);
    if (rc < 0) {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", rc, inga_usb_ftdi_get_error_string(ftdi));
        exit(EXIT_FAILURE);
    }

    printf("Reading out EEPROM image...");
    fflush(stdout);
    rc = inga_usb_ftdi_eeprom_read(ftdi);
    if (rc < 0) {
        fprintf(stderr, "\nCould not read and decode EEPROM: %i\n", rc);
        exit(EXIT_FAILURE);
    }
    VERBOSE("done\n\n");

    if ((inga_usb_ftdi_eeprom_get_value(ftdi, CHIP_TYPE, &chip_type) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, VENDOR_ID, &vid) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, PRODUCT_ID, &pid) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, MAX_POWER, &max_power) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_0, &cbus[0]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_1, &cbus[1]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_2, &cbus[2]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_3, &cbus[3]) < 0) ||
        (inga_usb_ftdi_eeprom_get_value(ftdi, CBUS_FUNCTION_4, &cbus[4]) < 0)) {

        fprintf(stderr, "Could not read the EEPROM values\n");
        exit(EXIT_FAILURE);
    }

    if ((inga_usb_ftdi_eeprom_get_string(ftdi, MANUFACTURER_STRING, &manufacturer) < 0) ||
        (inga_usb_ftdi_eeprom_get_string(ftdi, PRODUCT_STRING, &product) < 0) ||
        (inga_usb_ftdi_eeprom_get_string(ftdi, SERIAL_STRING, &serial) < 0)) {

        fprintf(stderr, "Could not read the EEPROM manufacturer, product and serial strings\n");
        exit(EXIT_FAILURE);
    }

    VERBOSE("\nEEPROM config:\n"
            "Chip type:    %i\n"
            "VID:          0x%04x\n"
            "PID:          0x%04x\n"
            "Manufacturer: %s\n"
            "Product:      %s\n"
            "Serial:       %s\n"
            "Max power:    %i\n"
            "CBUS:         %1x%1x %1x%1x 0%1x\n\n",
            chip_type, vid, pid, manufacturer, product, serial, max_power * 2,
            cbus[1], cbus[0], cbus[3], cbus[2], cbus[4]);

    vid = cfg->eep.vendor_id ? cfg->eep.vendor_id : vid;
    pid = cfg->eep.product_id ? cfg->eep.product_id : pid;

    manufacturer = cfg->eep.manufacturer ? cfg->eep.manufacturer : manufacturer;
    product = cfg->eep.product ? cfg->eep.product : product;
    serial = cfg->eep.serial ? cfg->eep.serial : serial;

    max_power = (cfg->eep.max_power > 0) ? (cfg->eep.max_power / 2) : max_power;

    cbus[0] = cfg->eep.cbus[0];
    cbus[1] = cfg->eep.cbus[1];
    cbus[2] = cfg->eep.cbus[2];
    cbus[3] = cfg->eep.cbus[3];
    cbus[4] = cfg->eep.cbus[4];

    VERBOSE("\nUpdating with EEPROM config:\n"
            "Chip type:    %i\n"
            "VID:          0x%04x\n"
            "PID:          0x%04x\n"
            "Manufacturer: %s\n"
            "Product:      %s\n"
            "Serial:       %s\n"
            "Max power:    %i\n"
            "CBUS:         %1x%1x %1x%1x 0%1x\n\n",
            chip_type, vid, pid, manufacturer, product, serial, max_power * 2,
            cbus[1], cbus[0], cbus[3], cbus[2], cbus[4]);

    if ((inga_usb_ftdi_eeprom_set_value(ftdi, CHIP_TYPE, chip_type) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, VENDOR_ID, vid) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, PRODUCT_ID, pid) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, MAX_POWER, max_power) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, CBUS_FUNCTION_0, cbus[0]) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, CBUS_FUNCTION_1, cbus[1]) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, CBUS_FUNCTION_2, cbus[2]) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, CBUS_FUNCTION_3, cbus[3]) < 0) ||
        (inga_usb_ftdi_eeprom_set_value(ftdi, CBUS_FUNCTION_4, cbus[4]) < 0)) {

        fprintf(stderr, "Could not write the EEPROM values\n");
        exit(EXIT_FAILURE);
    }

    if ((inga_usb_ftdi_eeprom_set_string(ftdi, MANUFACTURER_STRING, manufacturer) < 0) ||
        (inga_usb_ftdi_eeprom_set_string(ftdi, PRODUCT_STRING, product) < 0) ||
        (inga_usb_ftdi_eeprom_set_string(ftdi, SERIAL_STRING, serial) < 0)) {

        fprintf(stderr, "Could not write the EEPROM manufacturer, product and serial strings\n");
        exit(EXIT_FAILURE);
    }

    printf("Writing updated EEPROM image...");
    fflush(stdout);

    rc = inga_usb_ftdi_eeprom_write(ftdi);
    if (rc < 0) {
        fprintf(stderr, "Could not build and write EEPROM: %i\n", rc);
        exit(EXIT_FAILURE);
    }
    sleep(10);
    VERBOSE("done\n");

out:

    VERBOSE("Resetting USB device\n");

    inga_usb_ftdi_reset(ftdi);
    inga_usb_ftdi_close(ftdi);
    inga_usb_ftdi_deinit(ftdi);

    inga_usb_free_device(usbdev);
}

void usage(poptContext poptc, int exitcode, char *error, char *addl)
{
    poptPrintUsage(poptc, stderr, 0);
    if (error) {
        fprintf(stderr, "%s: %s\n", error, addl);
    }
    exit(exitcode);
}

void parse_options(int argc, const char **argv, struct config_t *cfg)
{
    char rc, *tmp;
    poptContext poptc;

    struct poptOption options[] = {
        { "reset", 'r', POPT_ARG_VAL, &cfg->mode, MODE_RESET, "Reset INGA node (default)",
          NULL },
        { "flash", 'f', POPT_ARG_VAL, &cfg->mode, MODE_UPDATE_EEPROM, "Update FTDI EEPROM of INGA",
          NULL },
        { "serial", 's', POPT_ARG_VAL, &cfg->mode, MODE_READ_SERIAL, "Read the FTDI serial from its EEPROM",
          NULL },
        { "device", 'd', POPT_ARG_STRING, &cfg->usb.device_path, 0, "Path to the serial device",
          "pathname" },
        { "usbserial", 'u', POPT_ARG_STRING, &cfg->usb.device_serial, 0, "Serial of the FTDI to use",
          "usbserial" },
        { "id", 'i', POPT_ARG_STRING, &cfg->usb.device_id, 0, "Limit choice to USB device id",
          "device_id" },
        { "verbose", 'v', POPT_ARG_NONE, &cfg->verbose, 0, "Be verbose",
          NULL },
        { "max-power", 'p', POPT_ARG_INT, &cfg->eep.max_power, 0, "Maximum current to draw from USB (mA)",
          "current" },
        { "eui64", 'e', POPT_ARG_VAL, &cfg->mode, MODE_EUI64, "Print out INGAs EUI64",
          NULL },
        POPT_AUTOHELP
        { NULL, 0, 0, NULL, 0 }
    };

    cfg->mode = MODE_RESET;

    poptc = poptGetContext(NULL, argc, argv, options, 0);

    while ((rc = poptGetNextOpt(poptc)) >= 0) ;

    if (poptPeekArg(poptc) != NULL) {
        poptPrintUsage(poptc, stderr, 0);
        exit(EXIT_FAILURE);
    }

    if (rc < -1) {
        /* Option parsing error */
        fprintf(stderr, "%s: %s\n", poptBadOption(poptc, POPT_BADOPTION_NOALIAS), poptStrerror(rc));
        exit(EXIT_FAILURE);
    }

    if (!cfg->usb.device_path && !cfg->usb.device_serial) {
        usage(poptc, 1, "At least one of --device or --serial has to be set", "");
    }

    poptFreeContext(poptc);

    if (!cfg->usb.device_path) {
        return;
    }

    /* Resolve symlinks/relative paths */
    tmp = realpath(cfg->usb.device_path, NULL);
    if (!tmp) {
        perror("Could not open device");
        exit(EXIT_FAILURE);
    }

    VERBOSE("Resolving path %s to %s\n", cfg->usb.device_path, tmp);

    free(cfg->usb.device_path);
    cfg->usb.device_path = tmp;
}

struct config_t *init_config(void)
{
    struct config_t *cfg = malloc(sizeof(struct config_t));

    if (!cfg) {
        return NULL;
    }

    memset(cfg, 0, sizeof(struct config_t));

    /* Set some sensible defaults
     * Enable CBUS3 IO, keep vendor/product ID as is,
     * set manufacturer to IBR and product string to INGA,
     * keep serial, don't change the max power setting */
    cfg->eep.cbus[0] = CBUS_TXLED;
    cfg->eep.cbus[1] = CBUS_RXLED;
    cfg->eep.cbus[2] = CBUS_TXDEN;
    cfg->eep.cbus[3] = CBUS_IOMODE;
    cfg->eep.cbus[4] = CBUS_SLEEP;
    cfg->eep.manufacturer = "IBR";
    cfg->eep.product = "INGA";
    cfg->eep.max_power = -1;

    return cfg;
}

int main(int argc, const char **argv)
{
    struct config_t *cfg;

    cfg = init_config();
    if (!cfg) {
        fprintf(stderr, "Could not initialize config\n");
        exit(EXIT_FAILURE);
    }

    parse_options(argc, argv, cfg);

    VERBOSE("Config: path: %s, serial: %s, id: %s\n", cfg->usb.device_path, cfg->usb.device_serial, cfg->usb.device_id);

    if (cfg->mode == MODE_RESET) {
        inga_reset(cfg);
    }
    else if (cfg->mode == MODE_UPDATE_EEPROM) {
        inga_eeprom(cfg);
    }
    else if (cfg->mode == MODE_READ_SERIAL) {
        inga_print_eeinfo(cfg);
    }
    else if (cfg->mode == MODE_EUI64) {
        inga_eui64(cfg);
    }

    exit(EXIT_SUCCESS);
}
