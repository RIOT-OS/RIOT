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

#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <libgen.h>
#if __linux__
#include <libudev.h>
#endif
#include <string.h>

#include "inga_usb.h"

#define VERBOSE(args ...) \
    if (verbose) \
        fprintf(stderr, ## args)

static void inga_usb_resolve(struct inga_usb_config_t *cfg, int verbose);

#if FTDI_VERSION == 0
/* Code specific to libftdi 0.x / libusb-0.1 */

#include <usb.h>

struct inga_usb_device_t {
    struct usb_device *usbdev;
};

struct inga_usb_ftdi_t {
    struct ftdi_context ctx;

    struct ftdi_eeprom eeprom;
    unsigned char eeprom_data[FTDI_DEFAULT_EEPROM_SIZE];
};

struct inga_usb_device_t *inga_usb_find_device(struct inga_usb_config_t *cfg, int verbose)
{
    int rc;
    struct usb_bus *bus;
    struct inga_usb_device_t *usb;
    struct usb_dev_handle *ufd;
    long int busnum;
    char *bus_end, buf[256];

    inga_usb_resolve(cfg, verbose);

    usb_init();
    usb_find_busses();
    usb_find_devices();

    bus = usb_get_busses();

    usb = (struct inga_usb_device_t *) malloc(sizeof(struct inga_usb_device_t));
    memset(usb, 0, sizeof(struct inga_usb_device_t));

    /* Iterate over all devices in all busses and see if one matches */
    for (; bus != NULL; bus = bus->next) {
        for (usb->usbdev = bus->devices; usb->usbdev != NULL; usb->usbdev = usb->usbdev->next) {

            /* Check if busnumber and devicenumber match */
            if ((cfg->busnum != 0) && (cfg->devnum != 0)) {
                if (usb->usbdev->devnum != cfg->devnum) {
                    continue;
                }

                /* Convert the bus number */
                busnum = strtol(usb->usbdev->bus->dirname, &bus_end, 10);
                if (bus_end[0] == 0 && usb->usbdev->bus->dirname[0] != 0) {
                    if (busnum != cfg->busnum) {
                        continue;
                    }
                }
                VERBOSE("Found USB device matching %i:%i\n", cfg->busnum, cfg->devnum);
            }

            /* Check if USB serial matches */
            if (cfg->device_serial) {
                ufd = usb_open(usb->usbdev);
                if (!ufd) {
                    continue;
                }
                if (!usb->usbdev->descriptor.iSerialNumber) {
                    usb_close(ufd);
                    continue;
                }
                rc = usb_get_string_simple(ufd, usb->usbdev->descriptor.iSerialNumber, buf, sizeof(buf));
                usb_close(ufd);

                if (rc <= 0 || strcmp(buf, cfg->device_serial)) {
                    continue;
                }

                VERBOSE("Found USB serial %s on USB device %s:%i\n", cfg->device_serial, usb->usbdev->bus->dirname, usb->usbdev->devnum);
            }

            VERBOSE("All requirements match, using %i:%i as target device\n", atoi(usb->usbdev->bus->dirname), usb->usbdev->devnum);
            return usb;
        }
    }

    inga_usb_free_device(usb);
    return NULL;
}

int inga_usb_ftdi_reset(struct inga_usb_ftdi_t *ftdi)
{
    if (ftdi_usb_reset(&ftdi->ctx) < 0) {
        return -1;
    }

    if (usb_reset(ftdi->ctx.usb_dev) < 0) {
        return -1;
    }

    return 0;
}

int inga_usb_ftdi_close(struct inga_usb_ftdi_t *ftdi)
{
    return ftdi_usb_close(&ftdi->ctx);
}

void inga_usb_free_device(struct inga_usb_device_t *usb)
{
    free(usb);
}


int inga_usb_ftdi_eeprom_read(struct inga_usb_ftdi_t *ftdi)
{
    int rc;

    if ((rc = ftdi_read_eeprom(&ftdi->ctx, ftdi->eeprom_data)) < 0) {
        return rc;
    }

    if ((rc = ftdi_eeprom_decode(&ftdi->eeprom, ftdi->eeprom_data, sizeof(ftdi->eeprom_data))) < 0) {
        return rc;
    }

    /* Decode fails to set the size which is needed to build an image again */
    ftdi->eeprom.size = FTDI_DEFAULT_EEPROM_SIZE;

    if (ftdi->eeprom.chip_type != ftdi->ctx.type) {
        fprintf(stderr, "WARNING: eeprom.chip_type (%d) != ctx.type (%d)\n", ftdi->eeprom.chip_type, ftdi->ctx.type);
    }

    return 0;
}

int inga_usb_ftdi_eeprom_write(struct inga_usb_ftdi_t *ftdi)
{
    int rc;

    if ((rc = ftdi_eeprom_build(&ftdi->eeprom, ftdi->eeprom_data)) < 0) {
        return rc;
    }

    if ((rc = ftdi_write_eeprom(&ftdi->ctx, ftdi->eeprom_data)) < 0) {
        return rc;
    }

    return 0;
}

int inga_usb_ftdi_eeprom_get_value(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_value value_name, int *value)
{
    switch (value_name) {
        case VENDOR_ID:
            *value = ftdi->eeprom.vendor_id;
            break;
        case PRODUCT_ID:
            *value = ftdi->eeprom.product_id;
            break;
        case SELF_POWERED:
            *value = ftdi->eeprom.self_powered;
            break;
        case REMOTE_WAKEUP:
            *value = ftdi->eeprom.remote_wakeup;
            break;
        case IN_IS_ISOCHRONOUS:
            *value = ftdi->eeprom.in_is_isochronous;
            break;
        case OUT_IS_ISOCHRONOUS:
            *value = ftdi->eeprom.out_is_isochronous;
            break;
        case SUSPEND_PULL_DOWNS:
            *value = ftdi->eeprom.suspend_pull_downs;
            break;
        case USE_SERIAL:
            *value = ftdi->eeprom.use_serial;
            break;
        case USB_VERSION:
            *value = ftdi->eeprom.usb_version;
            break;
        case MAX_POWER:
            *value = ftdi->eeprom.max_power;
            break;
        case CBUS_FUNCTION_0:
            *value = ftdi->eeprom.cbus_function[0];
            break;
        case CBUS_FUNCTION_1:
            *value = ftdi->eeprom.cbus_function[1];
            break;
        case CBUS_FUNCTION_2:
            *value = ftdi->eeprom.cbus_function[2];
            break;
        case CBUS_FUNCTION_3:
            *value = ftdi->eeprom.cbus_function[3];
            break;
        case CBUS_FUNCTION_4:
            *value = ftdi->eeprom.cbus_function[4];
            break;
        case CBUS_FUNCTION_5:
            *value = 0;
            break;
        case CBUS_FUNCTION_6:
            *value = 0;
            break;
        case CBUS_FUNCTION_7:
            *value = 0;
            break;
        case CBUS_FUNCTION_8:
            *value = 0;
            break;
        case CBUS_FUNCTION_9:
            *value = 0;
            break;
        case INVERT:
            *value = ftdi->eeprom.invert;
            break;
        case CHIP_TYPE:
            *value = ftdi->eeprom.chip_type;
            break;
        case CHIP_SIZE:
            *value = ftdi->eeprom.size;
            break;
        default:
            return -1;
    }
    return 0;
}

int inga_usb_ftdi_eeprom_set_value(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_value value_name, int value)
{
    switch (value_name) {
        case VENDOR_ID:
            ftdi->eeprom.vendor_id = value;
            break;
        case PRODUCT_ID:
            ftdi->eeprom.product_id = value;
            break;
        case SELF_POWERED:
            ftdi->eeprom.self_powered = value;
            break;
        case REMOTE_WAKEUP:
            ftdi->eeprom.remote_wakeup = value;
            break;
        case IN_IS_ISOCHRONOUS:
            ftdi->eeprom.in_is_isochronous = value;
            break;
        case OUT_IS_ISOCHRONOUS:
            ftdi->eeprom.out_is_isochronous = value;
            break;
        case SUSPEND_PULL_DOWNS:
            ftdi->eeprom.suspend_pull_downs = value;
            break;
        case USE_SERIAL:
            ftdi->eeprom.use_serial = value;
            break;
        case USB_VERSION:
            ftdi->eeprom.usb_version = value;
            break;
        case MAX_POWER:
            ftdi->eeprom.max_power = value;
            break;
        case CBUS_FUNCTION_0:
            ftdi->eeprom.cbus_function[0] = value;
            break;
        case CBUS_FUNCTION_1:
            ftdi->eeprom.cbus_function[1] = value;
            break;
        case CBUS_FUNCTION_2:
            ftdi->eeprom.cbus_function[2] = value;
            break;
        case CBUS_FUNCTION_3:
            ftdi->eeprom.cbus_function[3] = value;
            break;
        case CBUS_FUNCTION_4:
            ftdi->eeprom.cbus_function[4] = value;
            break;
        case CBUS_FUNCTION_5:
            ftdi->eeprom.cbus_function[5] = value;
            break;
        case CBUS_FUNCTION_6:
            ftdi->eeprom.cbus_function[6] = value;
            break;
        case CBUS_FUNCTION_7:
            ftdi->eeprom.cbus_function[7] = value;
            break;
        case CBUS_FUNCTION_8:
            ftdi->eeprom.cbus_function[8] = value;
            break;
        case CBUS_FUNCTION_9:
            ftdi->eeprom.cbus_function[9] = value;
            break;
        case INVERT:
            ftdi->eeprom.invert = value;
            break;
        case CHIP_TYPE:
            ftdi->eeprom.chip_type = value;
            break;
        case CHIP_SIZE:
            return -1;
        default:
            return -1;
    }
    return 0;
}

int inga_usb_ftdi_eeprom_get_string(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_string string_name, const char **str)
{
    *str = NULL;

    switch (string_name) {
        case MANUFACTURER_STRING:
            *str = ftdi->eeprom.manufacturer;
            break;

        case PRODUCT_STRING:
            *str = ftdi->eeprom.product;
            break;

        case SERIAL_STRING:
            *str = ftdi->eeprom.serial;
            break;

        default:
            return -1;
    }

    return 0;
}

int inga_usb_ftdi_eeprom_set_string(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_string string_name, const char *str)
{
    char **dst = NULL;

    switch (string_name) {
        case MANUFACTURER_STRING:
            dst = &ftdi->eeprom.manufacturer;
            break;

        case PRODUCT_STRING:
            dst = &ftdi->eeprom.product;
            break;

        case SERIAL_STRING:
            dst = &ftdi->eeprom.serial;
            break;

        default:
            return -1;
    }

    if (*dst == str) {
        return 0;
    }

    free(*dst);
    *dst = NULL;

    if (str) {
        *dst = strdup(str);
    }

    return 0;
}


#elif FTDI_VERSION == 1
/* Code specific to libftdi 1.x / libusb-1.x */

#include <libusb.h>

#define FTDI_DEFAULT_EEPROM_SIZE 256
#define FTDI_EEPROM_VALUES CHANNEL_D_RS485

struct inga_usb_device_t {
    libusb_context *usbctx;

    struct libusb_device *usbdev;
};

struct inga_usb_ftdi_t {
    struct ftdi_context ctx;

    char *eeprom_manufacturer;
    char *eeprom_product;
    char *eeprom_serial;

    int eeprom_values[FTDI_EEPROM_VALUES];
};

struct inga_usb_device_t *inga_usb_find_device(struct inga_usb_config_t *cfg, int verbose)
{
    struct inga_usb_device_t *usb;
    libusb_device **dev_list = NULL;
    ssize_t dev_count, i, j;

    inga_usb_resolve(cfg, verbose);

    usb = (struct inga_usb_device_t *) malloc(sizeof(struct inga_usb_device_t));
    memset(usb, 0, sizeof(struct inga_usb_device_t));

    libusb_init(&usb->usbctx);

    dev_count = libusb_get_device_list(usb->usbctx, &dev_list);
    if (dev_count < 0) {
        fprintf(stderr, "Failed get USB device list\n");
        exit(EXIT_FAILURE);
    }

    /* Iterate over all devices and see if one matches */
    for (i = 0; i < dev_count; i++) {
        usb->usbdev = dev_list[i];

        if ((cfg->busnum != 0) && (cfg->devnum != 0)) {
            if ((libusb_get_bus_number(usb->usbdev) != cfg->busnum) || (libusb_get_device_address(usb->usbdev) != cfg->devnum)) {
                continue;
            }

            VERBOSE("Found USB device matching %i:%i\n", cfg->busnum, cfg->devnum);
        }

        /* Check if USB serial matches */
        if (cfg->device_serial) {
            struct libusb_device_descriptor desc;
            libusb_device_handle *handle;
            char buf[256];
            int rc;

            if (libusb_get_device_descriptor(usb->usbdev, &desc) != 0) {
                continue;
            }

            if (!desc.iSerialNumber) {
                continue;
            }

            if (libusb_open(usb->usbdev, &handle) < 0) {
                continue;
            }

            rc = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (unsigned char *)buf, sizeof(buf));
            libusb_close(handle);

            if (rc <= 0 || strcmp(buf, cfg->device_serial)) {
                continue;
            }

            VERBOSE("Found USB serial %s on USB device %i:%i\n", cfg->device_serial,
                    libusb_get_bus_number(usb->usbdev), libusb_get_device_address(usb->usbdev));
        }

        VERBOSE("All requirements match, using %i:%i as target device\n",
                libusb_get_bus_number(usb->usbdev), libusb_get_device_address(usb->usbdev));

        /* Unreference the devices we are not going to use */
        for (j = 0; j < dev_count; j++)
            if (dev_list[j] != usb->usbdev) {
                libusb_unref_device(dev_list[j]);
            }

        libusb_free_device_list(dev_list, 0);
        return usb;
    }

    libusb_free_device_list(dev_list, 1);
    inga_usb_free_device(usb);
    return NULL;
}

int inga_usb_ftdi_reset(struct inga_usb_ftdi_t *ftdi)
{
    if (ftdi_usb_reset(&ftdi->ctx) < 0) {
        return -1;
    }

    if (libusb_reset_device(ftdi->ctx.usb_dev) < 1) {
        return -1;
    }

    return 0;
}

int inga_usb_ftdi_close(struct inga_usb_ftdi_t *ftdi)
{
    libusb_device *dev = NULL;
    int interface;

    /* Remember the USB device and ftdi_sio interface */
    if (ftdi->ctx.usb_dev) {
        dev = libusb_get_device(ftdi->ctx.usb_dev);
        interface = ftdi->ctx.interface;
    }

    /* Close the FTDI */
    if (ftdi_usb_close(&ftdi->ctx) < 0) {
        return -1;
    }

    /* Reattach the ftdi_sio kernel module to the USB device */
    if (dev) {
        libusb_device_handle *handle = NULL;

        if (libusb_open(dev, &handle) >= 0) {
            libusb_attach_kernel_driver(handle, interface);
            libusb_close(handle);
        }
    }

    return 0;
}

void inga_usb_free_device(struct inga_usb_device_t *usb)
{
    if (!usb) {
        return;
    }

    if (usb->usbdev) {
        libusb_unref_device(usb->usbdev);
    }

    if (usb->usbctx) {
        libusb_exit(usb->usbctx);
    }

    free(usb);
}

static void inga_usb_read_eeprom_string(char **str, const char *buf, int offset, int length)
{
    int i;

    free(*str);
    *str = NULL;

    if (length > 0) {
        *str = malloc(length);

        for (i = 0; i < length - 1; i++)
            (*str)[i] = buf[2 + offset + 2 * i];
        (*str)[i] = '\0';
    }
}

int inga_usb_ftdi_eeprom_read(struct inga_usb_ftdi_t *ftdi)
{
    int rc;
    char buf[FTDI_DEFAULT_EEPROM_SIZE];
    int string_size, eeprom_size, i;

    if ((rc = ftdi_read_eeprom(&ftdi->ctx)) < 0) {
        return rc;
    }

    if ((rc = ftdi_eeprom_decode(&ftdi->ctx, 0)) < 0) {
        return rc;
    }

    if ((rc = ftdi_get_eeprom_buf(&ftdi->ctx, (unsigned char *)buf, sizeof(buf))) < 0) {
        return rc;
    }

    if ((rc = ftdi_get_eeprom_value(&ftdi->ctx, CHIP_SIZE, &eeprom_size)) < 0) {
        return rc;
    }

    inga_usb_read_eeprom_string(&ftdi->eeprom_manufacturer, buf, buf[0x0E] & (eeprom_size - 1), buf[0x0F] / 2);
    inga_usb_read_eeprom_string(&ftdi->eeprom_product, buf, buf[0x10] & (eeprom_size - 1), buf[0x11] / 2);
    inga_usb_read_eeprom_string(&ftdi->eeprom_serial, buf, buf[0x12] & (eeprom_size - 1), buf[0x13] / 2);

    return 0;
}

static void inga_usb_ftdi_read_eeprom_values(struct inga_usb_ftdi_t *ftdi)
{
    int i;

    for (i = 0; i < FTDI_EEPROM_VALUES; i++)
        ftdi_get_eeprom_value(&ftdi->ctx, i, &ftdi->eeprom_values[i]);
}

static void inga_usb_ftdi_write_eeprom_values(struct inga_usb_ftdi_t *ftdi)
{
    int i;

    for (i = 0; i < FTDI_EEPROM_VALUES; i++)
        ftdi_set_eeprom_value(&ftdi->ctx, i, ftdi->eeprom_values[i]);
}

int inga_usb_ftdi_eeprom_write(struct inga_usb_ftdi_t *ftdi)
{
    int rc;

    /* ftdi_eeprom_initdefaults() is the only way to change the manufacturer, product and serial strings,
       but that function also clobbers everything else. Therefore, we save all EEPROM values beforehand
       and restore them afterwards.
     */

    inga_usb_ftdi_read_eeprom_values(ftdi);

    if ((rc = ftdi_eeprom_initdefaults(&ftdi->ctx,
                                       ftdi->eeprom_manufacturer, ftdi->eeprom_product, ftdi->eeprom_serial)) < 0) {
        return rc;
    }

    inga_usb_ftdi_write_eeprom_values(ftdi);


    if ((rc = ftdi_eeprom_build(&ftdi->ctx)) < 0) {
        return rc;
    }

    if ((rc = ftdi_write_eeprom(&ftdi->ctx)) < 0) {
        return rc;
    }

    return 0;
}

int inga_usb_ftdi_eeprom_get_value(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_value value_name, int *value)
{
    int rc;

    if ((rc = ftdi_get_eeprom_value(&ftdi->ctx, value_name, value) < 0)) {
        return rc;
    }

    if (value_name == MAX_POWER) {
        *value /= 2;
    }

    return rc;
}

int inga_usb_ftdi_eeprom_set_value(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_value value_name, int value)
{
    if (value_name == MAX_POWER) {
        value *= 2;
    }

    return ftdi_set_eeprom_value(&ftdi->ctx, value_name, value);
}

int inga_usb_ftdi_eeprom_get_string(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_string string_name, const char **str)
{
    *str = NULL;

    switch (string_name) {
        case MANUFACTURER_STRING:
            *str = ftdi->eeprom_manufacturer;
            break;

        case PRODUCT_STRING:
            *str = ftdi->eeprom_product;
            break;

        case SERIAL_STRING:
            *str = ftdi->eeprom_serial;
            break;

        default:
            return -1;
    }

    return 0;
}

int inga_usb_ftdi_eeprom_set_string(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_string string_name, const char *str)
{
    char **dst = NULL;

    switch (string_name) {
        case MANUFACTURER_STRING:
            dst = &ftdi->eeprom_manufacturer;
            break;

        case PRODUCT_STRING:
            dst = &ftdi->eeprom_product;
            break;

        case SERIAL_STRING:
            dst = &ftdi->eeprom_serial;
            break;

        default:
            return -1;
    }

    if (*dst == str) {
        return 0;
    }

    free(*dst);
    *dst = NULL;

    if (str) {
        *dst = strdup(str);
    }

    return 0;
}

#else
    #error Unrecognized libftdi version
#endif


/* Code common to libftdi 0.x / libusb-0.1 and libftdi 1.x / libusb-1.x */

/* Use udev to find the USB bus and device numbers a ttyUSB device is connected to */
static void inga_usb_resolve(struct inga_usb_config_t *cfg, int verbose)
{
#if __linux__
    int rc;
    struct udev *udev;
    struct udev_device *dev;
    char *devpath;
    const char *devnum, *busnum;

    if (cfg->device_path) {
        udev = udev_new();
        if (!udev) {
            fprintf(stderr, "Failed to initialize udev\n");
            exit(EXIT_FAILURE);
        }

        rc = asprintf(&devpath, "/sys/class/tty/%s", basename(cfg->device_path));
        if (rc < 0) {
            fprintf(stderr, "Failed to generate sysfs path\n");
            exit(EXIT_FAILURE);
        }

        dev = udev_device_new_from_syspath(udev, devpath);
        free(devpath);
        if (!dev) {
            fprintf(stderr, "Failed get udev device\n");
            exit(EXIT_FAILURE);
        }

        while (dev) {
            busnum = udev_device_get_sysattr_value(dev, "busnum");
            devnum = udev_device_get_sysattr_value(dev, "devnum");

            if (busnum && devnum) {
                /* TODO: Check conversion errors */
                cfg->busnum = atoi(busnum);
                cfg->devnum = atoi(devnum);
                VERBOSE("Device %s resolved to USB device %i:%i\n", cfg->device_path, cfg->busnum, cfg->devnum);
                break;
            }

            dev = udev_device_get_parent(dev);
        }
        udev_unref(udev);

        if (cfg->busnum == 0 || cfg->devnum == 0) {
            fprintf(stderr, "Failed to resolve %s to USB device\n", cfg->device_path);
            exit(EXIT_FAILURE);
        }
    }
#endif
}

int inga_usb_ftdi_init(struct inga_usb_ftdi_t **ftdi)
{
    *ftdi = (struct inga_usb_ftdi_t *) malloc(sizeof(struct inga_usb_ftdi_t));
    memset(*ftdi, 0, sizeof(struct inga_usb_ftdi_t));

    return ftdi_init(&(*ftdi)->ctx);
}

void inga_usb_ftdi_deinit(struct inga_usb_ftdi_t *ftdi)
{
    if (!ftdi) {
        return;
    }

    ftdi_deinit(&ftdi->ctx);

    free(ftdi);
}

int inga_usb_ftdi_open(struct inga_usb_ftdi_t *ftdi, struct inga_usb_device_t *usb)
{
    return ftdi_usb_open_dev(&ftdi->ctx, usb->usbdev);
}

int inga_usb_ftdi_set_bitmode(struct inga_usb_ftdi_t *ftdi, unsigned char bitmask, unsigned char mode)
{
    return ftdi_set_bitmode(&ftdi->ctx, bitmask, mode);
}

int inga_usb_ftdi_get_chip_type(struct inga_usb_ftdi_t *ftdi)
{
    return ftdi->ctx.type;
}


char *inga_usb_ftdi_get_error_string(struct inga_usb_ftdi_t *ftdi)
{
    return ftdi_get_error_string(&ftdi->ctx);
}
