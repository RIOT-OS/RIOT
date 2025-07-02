"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

from scancode.cli import run_scan as scancode_run_scan

# For use with multiprocessing, this helper function has been created
# in its own module to avoid pickling issues.
def scancode_scan_file(pth: str):
    return pth, scancode_run_scan(
            pth,
            strip_root=False,
            full_root=True,
            processes=-1,
            quiet=True,
            verbose=False,
            keep_temp_files=False,
            return_results=True,
            return_codebase=False,
            max_depth=1,
            copyright=True,
            license=True,
            email=False,
            url=False,
            package=False,
            system_package=False,
            generated=True,
            license_score=90,
            unknown_licenses=False)
