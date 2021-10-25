/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

SECTIONS
{
    /* Populate information about rom size */
    _srom = ORIGIN(ROM);
    _erom = ORIGIN(ROM) + LENGTH(ROM);

    .end_fw (NOLOAD) : ALIGN(4) {
        _end_fw = . ;
    } > ROM
}
