#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: MIT

# Checks all packages' Makefiles for SPDX compatibility of their license
# declarations.

set -e

cd "$(dirname "$(readlink -f "$0")")"

./get_package_license_identifiers.sh | ./verify_spdx_license_expressions.py
