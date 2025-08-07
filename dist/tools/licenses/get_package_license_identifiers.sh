#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: MIT

# Print list of unique license identifiers, as declared by package Makefiles.

set -e

cd "$(dirname "$(readlink -f "$0")")/../../../pkg"

grep -E 'LICENSE *[:\?]?=' -- */Makefile | cut -d = -f2 | sed -e 's/^[ \t]*//' -e 's/\([ \t]*#.*\)?[ \t]*$//' | sort | uniq
