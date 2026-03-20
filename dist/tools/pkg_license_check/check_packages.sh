#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: MIT

# Collects all packages' Makefiles and checks their license expressions
# for SPDX compatibility.

set -e

cd "$(dirname "$(readlink -f "$0")")"

./get_package_license_identifiers.sh | \
    sed -e '/\tpublic-domain\t/d' | \
    cut -f 3 | \
    sort | \
    uniq | \
    ./validate_spdx_license_expressions.py
