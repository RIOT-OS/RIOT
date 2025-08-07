#!/usr/bin/env python

# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: MIT

# This script expects newline-separated license expressions which it will
# attempt to parse and validate.
# The script will return 0 if all provided expressions are valid, otherwise 1.

from spdx_tools.common.spdx_licensing import spdx_licensing
from spdx_tools.spdx.validation.license_expression_validator import validate_license_expression
import sys
from types import SimpleNamespace

dummy_spdx_document = SimpleNamespace()
dummy_spdx_document.extracted_licensing_info = []

success = True

for line in sys.stdin:
    text = line.strip()
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
