"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

if __name__ == "__main__":
    # update search path for local testing
    import sys
    import pathlib
    sys.path.insert(0,pathlib.Path(__file__).absolute().parents[3].as_posix())

import logging
logger = logging.getLogger(__name__)
import os
import pathlib
import subprocess
from typing import Dict
import unittest

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.data.package_info import PackageInfo, PackageReference
from riot_sbom.data.app_info import AppInfo

def _find_system_package_for_file(file_path: pathlib.Path) -> Dict[str, str] | None:
    """
    Finds the system package for a given file path, if any.
    """
    if os.name == "nt":
        raise NotImplementedError("Microsoft Windows is not supported.")
    elif os.name == "posix":
        lsb_release_info = subprocess.run(
            ["lsb_release", "-a"],
            capture_output=True,
            text=True,
            check=False,
        )
        if lsb_release_info.returncode != 0:
            raise RuntimeError("lsb_release command failed. This plugin currently only supports Linux with lsb_release available.")
        # TODO add support for other distributions or package managers
        lsb_map = {l[0].strip(): l[1].strip() for l in (line.split(":", 1) for line in lsb_release_info.stdout.splitlines()) if len(l) == 2}
        if 'Distributor ID' not in lsb_map or 'Release' not in lsb_map:
            raise RuntimeError("lsb_release output does not contain expected fields.")
        if lsb_map['Distributor ID'] == "Ubuntu":
            # For Ubuntu, use dpkg to find the package
            dpkg_query = subprocess.run(
                ["dpkg", "-S", str(file_path)],
                capture_output=True,
                text=True,
                check=False,
            )
            if dpkg_query.returncode == 0:
                # TODO: add more info from `apt-cache show`
                return {
                    'name': dpkg_query.stdout.split(":")[0],
                    'supplier': lsb_map['Distributor ID'],
                    'version': lsb_map['Release']
                }
        else:
            raise NotImplementedError(
                f"System package detection for this OS is not implemented. lsb_release output:\n{lsb_release_info.stdout}")


class SystemPackageProvider(Plugin):
    def get_name(self):
        return "system-package-provider"

    def get_description(self):
        return "Will attempt to provide system package references for source files with no package relation."

    def run(self, app_info: AppInfo, _):
        for file in app_info.files:
            if not file.package and file.path.exists():
                system_package = _find_system_package_for_file(file.path)
                if system_package:
                    logger.debug(f"Found system package '{system_package}' for file: {file.path}")
                    package_ref = PackageReference(system_package['name'], pathlib.Path("/"))
                    if package_ref not in app_info.packages:
                        app_info.packages[package_ref] = PackageInfo(
                            name=system_package['name'],
                            version=system_package.get('version', None),
                            supplier=system_package.get('supplier', None),
                            authors=None,
                            source_dir=pathlib.Path("/"), # for relative file path representation
                            download_url=None,
                            licenses=None,
                            copyrights=None,
                        )
                    file.package = package_ref
                else:
                    logger.debug(f"No system package found for file: {file.path}")
        return app_info


class TestSystemPackageProvider(unittest.TestCase):
    def test_find_system_package_for_file(self):
        # This test assumes that the file exists and is part of a package.
        # Adjust the file path as necessary for your system.
        ls_location = subprocess.run(
            ["which", "ls"],
            capture_output=True,
            text=True,
            check=False,
        )
        if (ls_location.returncode != 0 or not ls_location.stdout.strip()
                or not pathlib.Path(ls_location.stdout.strip()).exists()):
            self.skipTest("The 'ls' command was not found on this system.")
        test_file = pathlib.Path(ls_location.stdout.strip())
        package_info = _find_system_package_for_file(test_file)
        self.assertIsNotNone(package_info)
        if not package_info:
            # make linter happy
            return
        self.assertIn('name', package_info)
        self.assertIn('supplier', package_info)
        self.assertIn('version', package_info)

    def test_no_system_package_found(self):
        import tempfile
        with tempfile.NamedTemporaryFile() as temp_file:
            test_file = pathlib.Path(temp_file.name)
            package_info = _find_system_package_for_file(test_file)
            self.assertIsNone(package_info, "Expected no system package to be found for a temporary file.")


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
