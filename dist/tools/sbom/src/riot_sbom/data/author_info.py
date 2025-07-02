"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

__all__ = ["AuthorDeclarationType", "AuthorInfo"]

from dataclasses import dataclass
from enum import Enum

class AuthorDeclarationType(Enum):
    """
    Different types of declarations for an author.
    A file can contain the author details in its text or they can be derived
    for from the package details.
    """
    TEXT_TAGGED = "text_tagged"
    TEXT_MATCHED = "text_matched"
    DERIVED = "derived"

@dataclass
class AuthorInfo:
    name: str
    email: str
    declaration_type: AuthorDeclarationType
