/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#if MODULE_TINYUSB_DEVICE && MODULE_TINYUSB_CLASS_CDC && MODULE_USB_BOARD_RESET

#define USB_H_USER_IS_RIOT_INTERNAL

#include "usb_board_reset_internal.h"
#include "class/cdc/cdc.h"

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const* p_line_coding)
{
    (void)itf;
    assert(p_line_coding != NULL);

    /* The first parameter is the USBUS CDC ACM device, but this is
     * not used in `usb_board_reset_coding_cb`. Therefore we can simply
     * reuse this callback function in tinyUSB without any problems. */
    usb_board_reset_coding_cb(NULL,
                              p_line_coding->bit_rate,
                              p_line_coding->data_bits,
                              p_line_coding->parity,
                              p_line_coding->stop_bits);
}

#else
typedef int dont_be_pedantic;
#endif
