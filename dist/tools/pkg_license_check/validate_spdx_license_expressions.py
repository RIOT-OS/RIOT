#!/usr/bin/env python

# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: LGPL-2.1-only

# This script expects newline-separated license expressions which it will
# attempt to parse and validate.
# The script will return 0 if all provided expressions are valid, otherwise 1.

import os
from pathlib import Path
from spdx_tools.common.spdx_licensing import spdx_licensing
from spdx_tools.spdx.validation.license_expression_validator import validate_license_expression
import sys
from types import SimpleNamespace

dummy_spdx_document = SimpleNamespace()
dummy_spdx_document.extracted_licensing_info = []

# LICENSES_DIRS is a colon-separated list of directories containing LicenseRef files
licesnse_ref_files = []
licenses_dirs = os.getenv("LICENSES_DIRS").split(os.pathsep)

# get a list of LicenseRef files from directories pointed to by LICENSES_DIRS
for licenses_dir in [Path(d) for d in licenses_dirs]:
    count = 0
    if not licenses_dir.is_dir():
        print(f"Warning: LICENSES_DIRS entry {licenses_dir} is not a directory")
        continue
    for path in licenses_dir.rglob("LicenseRef-*"):
        if path.is_file():
            count += 1
            licesnse_ref_files.append(path)
    print(f"Found {count} LicenseRef files in {licenses_dir}.")

# add the LicenseRef files to the validator database
# take their ID from the file names stem (without the extension)
for license_ref_file in licesnse_ref_files:
    license_ref_name = Path(license_ref_file).stem
    dummy_spdx_document.extracted_licensing_info.append(
        SimpleNamespace(license_id=license_ref_name)
    )

success = True

for line in sys.stdin:
    text = line.strip()
    if not text:
        continue
    sys.stdout.write(f"\nTESTING {text}: ")
    try:
        expression = spdx_licensing.parse(text)
    except Exception as e:
        print(f"Parse error: {text} (error: {e})")
        success = False
        continue
    msgs = validate_license_expression(expression, dummy_spdx_document, None)
    if msgs:
        print(f"Validation error: {text} (messages: {msgs})")
        success = False
    else:
        print("SUCCESS")

sys.exit(0 if success else 1)
