"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

from dataclasses import dataclass
from pathlib import Path
from typing import List
from typing import Dict

from .file_info import FileInfo
from .package_info import PackageInfo, PackageReference


@dataclass
class AppInfo:
    """
    Application information for a RIOT application.
    """
    build_dir: Path
    app_package_ref: PackageReference
    riot_package_ref: PackageReference
    board_package_ref: PackageReference
    packages: Dict[PackageReference, PackageInfo]
    files: List[FileInfo]
