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

#ifndef INGA_USB_H
#define INGA_USB_H

#include <ftdi.h>

#if FTDI_VERSION == 0
enum ftdi_eeprom_value {
    VENDOR_ID           = 0,
    PRODUCT_ID          = 1,
    SELF_POWERED        = 2,
    REMOTE_WAKEUP       = 3,
    IN_IS_ISOCHRONOUS   = 6,
    OUT_IS_ISOCHRONOUS  = 7,
    SUSPEND_PULL_DOWNS  = 8,
    USE_SERIAL          = 9,
    USB_VERSION         = 10,
    MAX_POWER           = 12,
    CBUS_FUNCTION_0     = 17,
    CBUS_FUNCTION_1     = 18,
    CBUS_FUNCTION_2     = 19,
    CBUS_FUNCTION_3     = 20,
    CBUS_FUNCTION_4     = 21,
    CBUS_FUNCTION_5     = 22,
    CBUS_FUNCTION_6     = 23,
    CBUS_FUNCTION_7     = 24,
    CBUS_FUNCTION_8     = 25,
    CBUS_FUNCTION_9     = 26,
    INVERT              = 30,
    CHIP_SIZE           = 43,
    CHIP_TYPE           = 44,
};
#endif /* FTDI_VERSION == 0 */

enum ftdi_eeprom_string {
    MANUFACTURER_STRING = 0,
    PRODUCT_STRING      = 1,
    SERIAL_STRING       = 2
};

struct inga_usb_config_t {
    char *device_path;
    char *device_serial;
    char *device_id;

    int busnum;
    int devnum;
};

struct inga_usb_device_t;
struct inga_usb_ftdi_t;

struct inga_usb_device_t *inga_usb_find_device(struct inga_usb_config_t *cfg, int verbose);

void inga_usb_free_device(struct inga_usb_device_t *usb);

int inga_usb_ftdi_init(struct inga_usb_ftdi_t **ftdi);
void inga_usb_ftdi_deinit(struct inga_usb_ftdi_t *ftdi);

int inga_usb_ftdi_open(struct inga_usb_ftdi_t *ftdi, struct inga_usb_device_t *usb);
int inga_usb_ftdi_close(struct inga_usb_ftdi_t *ftdi);

int inga_usb_ftdi_reset(struct inga_usb_ftdi_t *ftdi);

int inga_usb_ftdi_eeprom_read(struct inga_usb_ftdi_t *ftdi);
int inga_usb_ftdi_eeprom_write(struct inga_usb_ftdi_t *ftdi);

int inga_usb_ftdi_eeprom_get_value(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_value value_name, int *value);
int inga_usb_ftdi_eeprom_set_value(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_value value_name, int value);

int inga_usb_ftdi_eeprom_get_string(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_string string_name, const char **str);
int inga_usb_ftdi_eeprom_set_string(struct inga_usb_ftdi_t *ftdi, enum ftdi_eeprom_string string_name, const char *str);

int inga_usb_ftdi_set_bitmode(struct inga_usb_ftdi_t *ftdi, unsigned char bitmask, unsigned char mode);

int inga_usb_ftdi_get_chip_type(struct inga_usb_ftdi_t *ftdi);

char *inga_usb_ftdi_get_error_string(struct inga_usb_ftdi_t *ftdi);

#endif /* INGA_USB_H */
