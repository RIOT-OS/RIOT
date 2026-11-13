# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
od-related shell interactions

Defines od-related shell command interactions.

This does not work if the `od_string` module is included.
"""

import re
from typing import Optional, Union

from riotctrl.shell import ShellInteractionParser


class OdParser(ShellInteractionParser):
    def __init__(self):
        self.c_od = re.compile(
            r"(?P<offset>[0-9A-Fa-f]{8})(?P<bytes>(  [0-9A-Fa-f]{2})*)\s*$"
        )

    def _parse(
        self, cmd_output, offset=0, multiple=False
    ) -> Optional[Union[bytes, list[bytes]]]:
        # clean out potential stray (asynchronously printed) shell prompt
        cmd_output = cmd_output.replace("> ", "")
        res = None
        i = 0
        for line in cmd_output.splitlines():
            if (match := self.c_od.match(line)) is not None:
                output_offset = int(match["offset"], base=16)
                if not multiple:
                    assert output_offset == (offset + i), (
                        f"Offset 0x{output_offset:08x} != 0x{offset + i:08x}, "
                        "you might need to set the `offset` parameter of the "
                        "parse() method to a different value or use parse_multiple()."
                    )
                byts = bytes.fromhex(match["bytes"])
                if multiple and output_offset < (offset + i):
                    i = 0
                    res.append(byts)
                elif res is None:
                    if multiple:
                        res = [byts]
                    else:
                        res = byts
                else:
                    if multiple:
                        res[-1] += byts
                    else:
                        res += byts
                i += len(byts)
        return res

    def parse(self, cmd_output, offset=0) -> Optional[bytes]:
        return self._parse(cmd_output, offset=offset, multiple=False)

    def parse_multiple(self, cmd_output, offset=0) -> Optional[list[bytes]]:
        return self._parse(cmd_output, offset=offset, multiple=True)
