#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: LGPL-2.1-only

# Collects all packages' Makefiles and checks their license expressions
# for SPDX compatibility.

set -euo pipefail

cd "$(dirname "$(readlink -f "$0")")"

./get_package_license_identifiers.sh | \
    cut -f 2 | \
    sed -e '/CopryrightWaived/d' | \
    sort | \
    uniq | \
    ./validate_spdx_license_expressions.py
