#!/bin/bash

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH

# Checks all packages' Makefiles for SPDX compatibility of their license
# declarations.

cd "$(dirname \"$(readlink -f \"$0\")\")"

set -e

./get_package_license_identifiers.sh | ./verify_spdx_license_expressions.py
