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
from riot_sbom.data.license_info import LicenseInfo, LicenseDeclarationType

_spdx_identifier_matcher = re.compile(r'SPDX-License-Identifier:[ \t]*(?P<license>[\w \t\(\)+\.-]{3,}?)[ \t]*(\*/)?$', re.UNICODE)

__all__ = ["SpdxIdsScanner"]

class SpdxIdsScanner(Plugin):
    def get_name(self):
        return "spdx-identifiers-scanner"

    def get_description(self):
        return "Scans SPDX identifier based license declarations from source files."

    def run(self, app_info, _):
        for file in app_info.files:
            if file.path.exists():
                licenses: List[LicenseInfo] = []
                with file.path.open("rt") as f:
                    for line in f:
                        m = _spdx_identifier_matcher.search(line)
                        if m:
                            license = m.group("license")
                            if license:
                                license = license.strip()
                                if not license.strip('()-.+'):
                                    # invalid or empty license statement
                                    logger.warning(f"Found empty SPDX identifier in line: {line} of file: {file.path}")
                                    continue
                                license_info = LicenseInfo(
                                    declaration_text=license,
                                    declaration_type=LicenseDeclarationType.EXACT_REFERENCE,
                                    url=None,
                                    license_text=None
                                )
                                licenses.append(license_info)
                            logger.debug(f"Found SPDX identifier: {license} in file: {file.path}")
                if licenses:
                    file.licenses = licenses
        return app_info


class TestSpdxIdsMatcher(unittest.TestCase):
    def test_spdx_ids_match_success(self):
        test_cases_match = {
            # string concatenation avoids matching the test case
            "SPDX-License-Identifier" + ": MIT": "MIT",
            "SPDX-License-Identifier" + ": Apache-2.0": "Apache-2.0",
            "SPDX-License-Identifier" + ": GPL-3.0-or-later": "GPL-3.0-or-later",
            "SPDX-License-Identifier" + ": BSD-3-Clause": "BSD-3-Clause",
            "SPDX-License-Identifier" + ": CC-BY-4.0 OR MIT": "CC-BY-4.0 OR MIT",
            "SPDX-License-Identifier" + ": CC-BY-4.0 OR MIT  \t": "CC-BY-4.0 OR MIT",
        }
        for t, a in test_cases_match.items():
            m = _spdx_identifier_matcher.search(t)
            self.assertIsNotNone(m, f"Expected match for line: {t}")
            if m: # make the linter happy
                self.assertIsNotNone(m.group("license"),
                                 f"Expected license for line: {t} (groups: {m.groups()})")
                self.assertEqual(m.group("license"), a,
                                 f"Expected license: {a} for line: {t} (groups: {m.groups()})")

    def test_spdx_ids_match_fail(self):
        test_cases_no_match = [
            # string concatenation avoids matching the test case
            "SPDX-License-Identifier" + ":  \t@#$%^&*",
            "SPDX-License-Identifier" + ":  MIT#",
        ]
        for t in test_cases_no_match:
            m = _spdx_identifier_matcher.search(t)
            self.assertIsNone(m, f"Expected no match for line: \"{t}\"")

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
