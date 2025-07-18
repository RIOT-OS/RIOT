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
    sys.path.insert(0, pathlib.Path(__file__).absolute().parents[3].as_posix())

import logging
logger = logging.getLogger(__name__)
import unittest

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.data.app_info import AppInfo
from riot_sbom.data.package_info import PackageInfo
from riot_sbom.data.copyright_info import CopyrightInfo, CopyrightDeclarationType
from riot_sbom.data.license_info import LicenseInfo, LicenseDeclarationType
from riot_sbom.data.author_info import AuthorInfo, AuthorDeclarationType


__all__ = ["InferFileDataFromPackage"]


class InferFileDataFromPackage(Plugin):
    def get_name(self):
        return "infer-file-data-from-package"

    def get_description(self):
        return "Tries to complete file data from associated package."

    def run(self, app_info: AppInfo, _) -> AppInfo:
        for file in app_info.files:
            if file.package and file.package in app_info.packages:
                package: PackageInfo = app_info.packages[file.package]
                if not file.copyrights and package.copyrights:
                    file.copyrights = [CopyrightInfo(
                        holder=cr.holder,
                        years=cr.years,
                        declaration_type=CopyrightDeclarationType.DERIVED,
                        tag=None,
                        url=cr.url
                    ) for cr in package.copyrights]
                if not file.licenses and package.licenses:
                    file.licenses = [LicenseInfo(
                        declaration_text=lic.declaration_text,
                        declaration_type=LicenseDeclarationType.DERIVED,
                        license_text=lic.license_text,
                        url=lic.url
                    ) for lic in package.licenses]
                if not file.authors and package.authors:
                    file.authors = [AuthorInfo(
                        name=auth.name,
                        email=auth.email,
                        declaration_type=AuthorDeclarationType.DERIVED
                    ) for auth in package.authors]
        return app_info


class TestInferFileDataFromPackage(unittest.TestCase):
    def test_infer_file_data_from_package(self):
        import tempfile
        import pathlib
        from riot_sbom.data.license_info import LicenseInfo, LicenseDeclarationType
        from riot_sbom.data.copyright_info import CopyrightInfo, CopyrightDeclarationType
        from riot_sbom.data.author_info import AuthorInfo, AuthorDeclarationType
        from riot_sbom.data.checked_url import CheckedUrl
        from riot_sbom.data.package_info import PackageReference
        from riot_sbom.data.file_info import FileInfo
        app_package=PackageInfo(
            name="example-app",
            version="1.0.0",
            licenses=[LicenseInfo(declaration_text="MIT",
                                    declaration_type=LicenseDeclarationType.EXACT_REFERENCE,
                                    license_text=None, url=None)],
            copyrights=[CopyrightInfo(holder="Example Holder",
                                        years="2025",
                                        declaration_type=CopyrightDeclarationType.TEXT_TAGGED,
                                        tag="SPDX-FileCopyrightText:",
                                        url=None)],
            download_url=CheckedUrl("https://example.com/package.tar.gz"),
            authors=[AuthorInfo(name="John Doe",
                                email="john@doe.com",
                                declaration_type=AuthorDeclarationType.TEXT_TAGGED)],
            source_dir=pathlib.Path("/path/to/source"),
            supplier="Example Supplier"
        )
        riot_package=PackageInfo(
            name="example-riot",
            version="1.0.0",
            licenses=[LicenseInfo(declaration_text="MIT",
                                    declaration_type=LicenseDeclarationType.EXACT_REFERENCE,
                                    license_text=None, url=None)],
            copyrights=[CopyrightInfo(holder="Example Holder",
                                        years="2025",
                                        declaration_type=CopyrightDeclarationType.TEXT_TAGGED,
                                        tag="SPDX-FileCopyrightText:",
                                        url=None)],
            download_url=CheckedUrl("https://example.com/package.tar.gz"),
            authors=[AuthorInfo(name="John Doe",
                                email="john@doe.com",
                                declaration_type=AuthorDeclarationType.TEXT_TAGGED)],
            source_dir=pathlib.Path("/path/to/source"),
            supplier="Example Supplier"
        )
        board_package=PackageInfo(
            name="example-board",
            version="1.0.0",
            licenses=[LicenseInfo(declaration_text="MIT",
                                    declaration_type=LicenseDeclarationType.EXACT_REFERENCE,
                                    license_text=None, url=None)],
            copyrights=[CopyrightInfo(holder="Example Holder",
                                        years="2025",
                                        declaration_type=CopyrightDeclarationType.TEXT_TAGGED,
                                        tag="SPDX-FileCopyrightText:",
                                        url=None)],
            download_url=CheckedUrl("https://example.com/package.tar.gz"),
            authors=[AuthorInfo(name="John Doe",
                                email="john@doe.com",
                                declaration_type=AuthorDeclarationType.TEXT_TAGGED)],
            source_dir=pathlib.Path("/path/to/source"),
            supplier="Example Supplier")
        app_info = AppInfo(
            build_dir=pathlib.Path("/path/to/build"),
            app_package_ref=PackageReference.from_package_info(app_package),
            riot_package_ref=PackageReference.from_package_info(riot_package),
            board_package_ref=PackageReference.from_package_info(board_package),
            packages={
                PackageReference.from_package_info(app_package): app_package,
                PackageReference.from_package_info(riot_package): riot_package,
                PackageReference.from_package_info(board_package): board_package
            },
            files=[]
        )
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_dir_path = pathlib.Path(temp_dir)
            for ii, pkg in enumerate(app_info.packages.values()):
                source_file: pathlib.Path = temp_dir_path / f"source_file_{ii}.c"
                with open(source_file, 'w') as f:
                    f.write(f"// Source file {ii}")
                app_info.files.append(FileInfo(
                    path=source_file,
                    package=PackageReference.from_package_info(pkg),
                    licenses=None,
                    copyrights=None,
                    authors=None
                ))
            this_plugin = InferFileDataFromPackage()
            app_info = this_plugin.run(app_info, None)
            for file in app_info.files:
                self.assertIsNotNone(file.copyrights, f"Expected copyrights to be inferred for file: {file}")
                self.assertIsNotNone(file.licenses, f"Expected licenses to be inferred for file: {file}")
                self.assertIsNotNone(file.authors, f"Expected authors to be inferred for file: {file}")
                if not file.copyrights or not file.licenses or not file.authors:
                    continue # make linter happy
                self.assertEqual(len(file.copyrights), 1, f"Expected exactly one copyright for file: {file}")
                self.assertEqual(len(file.licenses), 1, f"Expected exactly one license for file: {file}")
                self.assertEqual(len(file.authors), 1, f"Expected exactly one author for file: {file}")


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
