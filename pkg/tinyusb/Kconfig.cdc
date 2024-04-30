# Copyright (c) 2022 Gunar Schorcht
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

if USEMODULE_TINYUSB_CLASS_CDC && USEMODULE_TINYUSB_DEVICE

config TUSBD_CDC_NUMOF
    int "Number of CDC interfaces"
    default 1
    range 1 2

config TUSBD_CDC_NOTIF_EP_SIZE
    int "CDC notification endpoint size [byte]"
    default 8

config TUSBD_CDC_FS_EP_SIZE
    int "CDC Full-Speed endpoint size [byte]"
    default 64

config TUSBD_CDC_HS_EP_SIZE
    int "CDC High-Speed endpoint size [byte]"
    default 512

config TUSBD_CDC_0_STRING
    string "CDC0 descriptor string"
    depends on TUSBD_CDC_NUMOF > 0
    default "TinyUSB CDC0"

config TUSBD_CDC_1_STRING
    string "CDC1 descriptor string"
    depends on TUSBD_CDC_NUMOF > 1
    default "TinyUSB CDC1"

endif
