"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

__all__ = ["LicenseDeclarationType", "LicenseInfo"]

from dataclasses import dataclass
from enum import Enum

from .checked_url import CheckedUrl

class LicenseDeclarationType(Enum):
    """
    Different types of declarations for a license.
    A package or file can contain the license text, reference a license by
    defined name or have an inexact match to a license. If no match exists,
    a license can still be derived for a file from the package license or
    for a package from its contained files.
    """
    TEXT_CONTAINED = "text_contained"
    EXACT_REFERENCE = "exact_reference"
    INEXACT_MATCH = "inexact_match"
    DERIVED = "derived"

@dataclass
class LicenseInfo:
    declaration_text: str
    declaration_type: LicenseDeclarationType
    license_text: str | None
    url: CheckedUrl | None
