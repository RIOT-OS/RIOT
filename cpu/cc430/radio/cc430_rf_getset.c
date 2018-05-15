/*
 * Copyright (C) 2016 MUTEX NZ Ltd.
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc430
 * @{
 *
 * @file
 * @brief       Getter and setter functions for the cc430_rf driver
 *
 * @author      Aaron Sowry <aaron@mutex.nz>
 * @author      Ian Martin <ian@locicontrols.com>
 *
 * @}
 */

#include "cc430_rf_getset.h"

uint8_t cc430_get_max_packet_size(void)
{
    return CC430_MAX_PACKET_SIZE;
}


int8_t cc430_set_channel(cc110x_t *dev, uint8_t channr)
{

    if (channr > MAX_CHANNR) {
        return -1;
    }

    cc430_radio_write_single_reg(CHANNR, channr);
    dev->radio_channel = channr;

    return channr;
}


uint8_t cc430_set_address(cc110x_t *dev, uint8_t address)
{
    if (!(address < MIN_UID)) {
        if (dev->radio_state != RADIO_UNKNOWN) {
            cc430_radio_write_single_reg(ADDR, address);
            dev->radio_address = address;
            return address;
        }
    }

    return 0;
}

uint8_t cc430_get_radio_state(void)
{
    uint8_t stateRead;
    uint8_t state;

    stateRead = cc430_radio_read_single_reg(MARCSTATE);

    switch (stateRead) {
        case 0:
            state = SLEEP;
            break;
        case 1:
            state = IDLE;
            break;
        case 2:
            state = XOFF;
            break;
        case 3:
        case 4:
        case 5:
            state = MANCAL;
            break;
        case 6:
        case 7:
            state = FS_WAKEUP;
        case 8:
        case 12:
            state = CALIBRATE;
            break;
        case 9:
        case 10:
        case 11:
            state = SETTLING;
            break;
        case 13:
        case 14:
        case 15:
            state = RX;
            break;
        case 16:
            state = TXRX_SETTLING;
            break;
        case 17:
            state = RX_OVERFLOW;
            break;
        case 18:
            state = FSTXON;
        case 19:
        case 20:
            state = TX;
            break;
        case 21:
            state = RXTX_SETTLING;
            break;
        case 22:
            state = TX_UNDERFLOW;
            break;
        default:
            state = STATE_READ_ERROR;
            break;
    }
    return state;
}

