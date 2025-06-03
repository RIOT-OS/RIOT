"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

if __name__ == "__main__":
    # update search path for local testing
    import sys
    import pathlib
    sys.path.insert(0,pathlib.Path(__file__).absolute().parents[3].as_posix())

import logging
logger = logging.getLogger(__name__)
import re
from typing import List
import unittest

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.data.copyright_info import CopyrightInfo, CopyrightDeclarationType

_copyright_matcher = re.compile(
    r'(?P<tag>[ \t]*SPDX-FileCopyrightText:?|[ \t]*[Cc]opyright \([cC]\)|[ \t]*[Cc]opyright:?|[ \t]*\([cC]\))+[\t ]+(?P<years>[0-9]{4}[0-9, \t-]*)[ \t]+(?P<holder>.*)(\*/)?',
    re.UNICODE)

__all__ = ["CopyrightsScanner"]

class CopyrightsScanner(Plugin):
    def get_name(self):
        return "copyrights-scanner"

    def get_description(self):
        return "Scans copyright declarations from source files."

    def run(self, app_info, _):
        for file in app_info.files:
            if file.path.exists():
                copyrights: List[CopyrightInfo] = []
                with file.path.open("rt") as f:
                    for line in f:
                        m = _copyright_matcher.search(line)
                        if m:
                            holder = m.group("holder")
                            if holder:
                                holder = holder.strip()
                            years = m.group("years")
                            if years:
                                years = years.strip()
                            tag = m.group("tag")
                            if tag:
                                tag = tag.strip()
                            if holder or years:
                                # we accept any tagged data, even if incomplete
                                copyright_info = CopyrightInfo(
                                    holder=holder, years=years,
                                    declaration_type=CopyrightDeclarationType.TEXT_TAGGED,
                                    tag=tag if tag else None,
                                    url=None)
                                copyrights.append(copyright_info)
                            logger.debug(f"Found copyright: {years} {holder} in file: {file.path}")
                if copyrights:
                    file.copyrights = copyrights
        return app_info


class TestCopyrightsMatcher(unittest.TestCase):
    def test_copyrights_match_success(self):
        test_cases_match = {
            # string concatenation avoids matching the test case
            "(C)" + " 1759 John Doe": ("1759", "John Doe"),
            "(c)" + " 1759 John Doe": ("1759", "John Doe"),
            "Copyright" + " (C)" + " 1759 John Doe": ("1759", "John Doe"),
            "Copyright" + " (c)" + " 1759 John Doe": ("1759", "John Doe"),
            "copyright" + " (c)" + " 1759 John Doe": ("1759", "John Doe"),
            "copyright" + " (C)" + " 1759 John Doe": ("1759", "John Doe"),
            "copyright" + " 1759 John Doe": ("1759", "John Doe"),
            "Copyright" + " 1759 John Doe": ("1759", "John Doe"),
            "Copyright" + "  1759 John Doe": ("1759", "John Doe"),
            "Copyright" + "  1759\tJohn Doe": ("1759", "John Doe"),
            "Copyright" + "  1759  John Doe": ("1759 ", "John Doe"),
            "SPDX-FileCo" + "pyrightText: 1759 John Doe": ("1759", "John Doe"),
            "SPDX-FileCo" + "pyrightText:  1759 John Doe": ("1759", "John Doe"),
            "SPDX-FileCo" + "pyrightText:  1759\tJohn Doe": ("1759", "John Doe"),
            "SPDX-FileCo" + "pyrightText:  1759  John Doe": ("1759 ", "John Doe"),
        }
        for t, a in test_cases_match.items():
            m = _copyright_matcher.search(t)
            self.assertIsNotNone(m, f"Expected match for line: {t}")
            if m: # make the linter happy
                self.assertIsNotNone(m.group("holder"),
                                 f"Expected holder for line: {t} (groups: {m.groups()})")
                self.assertIsNotNone(m.group("years"),
                                 f"Expected years for line: {t} (groups: {m.groups()})")
                self.assertIsNotNone(m.group("tag"),
                                 f"Expected tag for line: {t} (groups: {m.groups()})")
                self.assertEqual(m.group("years"), a[0],
                                 f"Expected years: {a[0]} for line: {t} (groups: {m.groups()})")
                self.assertEqual(m.group("holder"), a[1],
                                 f"Expected holder: {a[1]} for line: {t} (groups: {m.groups()})")


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
