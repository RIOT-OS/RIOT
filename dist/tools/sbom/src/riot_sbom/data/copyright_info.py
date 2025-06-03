"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

__all__ = ["CopyrightDeclarationType", "CopyrightInfo"]

from dataclasses import dataclass
from enum import Enum

class CopyrightDeclarationType(Enum):
    """
    Different types of declarations for a copyright.
    A file can contain the copyright text or it can be derived for
    a file from the package copyright.
    """
    TEXT_TAGGED = "text_tagged"
    TEXT_MATCHED = "text_matched"
    DERIVED = "derived"

@dataclass
class CopyrightInfo:
    holder: str
    years: str
    declaration_type: CopyrightDeclarationType
    tag: str | None
    url: str | None
