# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

import pytest

import riotctrl_shell.od


def test_none():
    parser = riotctrl_shell.od.OdParser()
    assert parser.parse("") is None


def test_empty():
    parser = riotctrl_shell.od.OdParser()
    assert parser.parse("00000000") == b""
    assert parser.parse("00000000  ") == b""

    with pytest.raises(AssertionError):
        # offset is wrong
        parser.parse("00000004")
    # majuscel hex
    assert parser.parse(f"{0xc0ffee:08X}", offset=0xc0ffee) == b""
    # minuscel hex
    assert parser.parse(f"{0xc0ffee:08x}", offset=0xc0ffee) == b""


def test_output():
    parser = riotctrl_shell.od.OdParser()
    assert parser.parse(
        "00000000  00  01  02  03\n"
        "00000004  04  05  06  07"
    ) == bytes(range(8))
    assert parser.parse(
        "00000051  10  11  12  13\n"
        "00000055  14  15  16  17",
        offset=0x51,
    ) == bytes(range(0x10, 0x18))
    assert parser.parse(
        "00000000  98  27  8E  57  DF  F7  FD  88  DC  14  3A  08  1B  87  2F  CB\n"
        "00000010  D5  3F  CC  2A  90  E9  A0  97  7F  07  A1  EF  0D  F1  8C  26\n"
        "00000020  EC  22  9E  75"
    ) == bytes.fromhex(
        "          98  27  8E  57  DF  F7  FD  88  DC  14  3A  08  1B  87  2F  CB"
        "          D5  3F  CC  2A  90  E9  A0  97  7F  07  A1  EF  0D  F1  8C  26"
        "          EC  22  9E  75"
    )


def test_output_w_garbage():
    parser = riotctrl_shell.od.OdParser()
    assert parser.parse(
        "00000000  08  09  0a  0b  0c  0d  0e  0f\n"
        "garbage garbage garbage > \n"
        "00000008 >  10  11  12  \r\n"
        "0000000b"
    ) == bytes(range(0x8, 0x13))
