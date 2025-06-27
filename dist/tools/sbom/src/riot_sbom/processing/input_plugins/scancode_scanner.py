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
import multiprocessing
import os
from pathlib import Path
import re
from typing import List, Dict
import unittest

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.data.app_info import AppInfo
from riot_sbom.data.file_info import FileInfo
from riot_sbom.data.author_info import AuthorInfo, AuthorDeclarationType
from riot_sbom.data.license_info import LicenseInfo, LicenseDeclarationType
from riot_sbom.data.copyright_info import CopyrightInfo, CopyrightDeclarationType
from riot_sbom.processing.input_plugins.scancode_helper import scancode_scan_file


__all__ = ["ScancodeScanner"]


class ScancodeScanner(Plugin):
    def __init__(self, n_procs: int=os.cpu_count() or 1):
        self.__n_procs: int = n_procs

    def get_name(self):
        return "scancode-scanner"

    def get_description(self):
        return "Will use `scancode` to provide file details for authors, copyrights, licenses from a file's contents."

    def run(self, app_info: AppInfo, _) -> AppInfo:
        logger.info("Scanning files with scancode")
        # the way scancode is built, we have to pass the files individually
        # and not as a list of paths as these will always be reduced to a common
        # prefix, resulting in many redundant scans
        with multiprocessing.Pool(self.__n_procs) as pool:
            from scancode.cli import run_scan as scancode_run_scan
            file_scan_result = pool.starmap(
                    scancode_scan_file,
                    [(str(pth), ) for pth in set(file.path
                                                 for file in app_info.files
                                                 if file.path.exists())])
        author_matcher = re.compile(r'([^<]*)? *(<.*>)?')
        copyright_matcher = re.compile(
            r'(?P<tag>[ \t]*SPDX-FileCopyrightText:?|[ \t]*[Cc]opyright|[ \t]*\([cC]\))*[\t ]*(?P<years>[0-9]{4}[0-9, \t-]*)[ \t]+(?P<holder>.*)(\*/)?',
            re.UNICODE)
        scancode_output_transformed: Dict[Path, FileInfo] = {}
        logger.debug(f"Transforming scancode output (number of results: {len(file_scan_result)})",)
        for ii in range(len(file_scan_result)):
            pth = file_scan_result[ii][0]
            if not file_scan_result[ii][1][0]:
                logger.warning(f"Scancode did not return any results for file: {pth}")
            else:
                scan_result = file_scan_result[ii][1][1]
                logger.debug(f"Processing scan result {ii}: {file_scan_result[ii]}")
                pth = Path('/' + scan_result['files'][0]['path'])
                logger.debug(f"Processing file: {pth}")
                path_scan_result = FileInfo(
                    path=pth,
                    package=None,
                    licenses=None,
                    authors=None,
                    copyrights=None,
                )
                if (scan_result.get('files') and
                        scan_result.get('files')[0].get('copyrights')):
                    path_scan_result.copyrights = []
                    for c in scan_result['files'][0]['copyrights']:
                        m = copyright_matcher.match(c['copyright'])
                        if m:
                            years = m.group('years')
                            if years:
                                years = years.strip()
                            holder = m.group('holder')
                            if holder:
                                holder = holder.strip()
                            tag = m.group('tag')
                            if tag:
                                tag = tag.strip()
                            path_scan_result.copyrights.append(CopyrightInfo(
                                holder=holder,
                                years=years,
                                declaration_type=CopyrightDeclarationType.TEXT_TAGGED if tag else CopyrightDeclarationType.TEXT_MATCHED,
                                tag=tag,
                                url=None))
                if (scan_result.get('files') and
                        scan_result.get('files')[0].get('authors')):
                    authors: List[AuthorInfo] = []
                    for c in scan_result['files'][0]['authors']:
                        m = author_matcher.match(c['author'])
                        if m:
                            name = m.group(1)
                            if name:
                                name = name.strip(" \t\"")
                            email = m.group(2)
                            if email:
                                email = email.strip(' \t<>')
                            authors.append(AuthorInfo(
                                name=name,
                                email=email,
                                declaration_type=AuthorDeclarationType.TEXT_MATCHED))
                    path_scan_result.authors = authors
                if scan_result.get('license_detections'):
                    path_scan_result.licenses = [
                            LicenseInfo(
                                declaration_text=l['license_expression_spdx'],
                                declaration_type=LicenseDeclarationType.INEXACT_MATCH,
                                license_text=None, url=None)
                            for l in scan_result['license_detections']]
                if not path_scan_result.licenses and (scan_result.get('files') and
                        scan_result.get('files')[0]
                        .get('detected_license_expression_spdx')):
                    path_scan_result.licenses = [
                            LicenseInfo(
                                declaration_text=(scan_result
                                    .get('files')[0]
                                    .get('detected_license_expression_spdx')
                                    .replace('\n', '')),
                                declaration_type=LicenseDeclarationType.INEXACT_MATCH,
                                license_text=None, url=None)
                            ]
                logger.debug(f"Transformed scan result for {pth}: "
                                f"authors={path_scan_result.authors}, "
                                f"copyrights={path_scan_result.copyrights}, "
                                f"licenses={path_scan_result.licenses}")
                scancode_output_transformed[pth] = path_scan_result
        for file in app_info.files:
            if (file.path in scancode_output_transformed
                    and (scancode_output_transformed[file.path].authors
                         or scancode_output_transformed[file.path].copyrights
                         or scancode_output_transformed[file.path].licenses)):
                scan_result = scancode_output_transformed[file.path]
                # This overwrites the file's data with the scancode results
                if scan_result.authors:
                    file.authors = scan_result.authors
                if scan_result.copyrights:
                    file.copyrights = scan_result.copyrights
                if scan_result.licenses:
                    file.licenses = scan_result.licenses
            else:
                logger.info(f"Scancode did not return any data for file: {file.path}")
        return app_info


class TestScanCodeScanner(unittest.TestCase):
    def test_infer_file_data_from_package(self):
        import tempfile
        import pathlib
        from riot_sbom.data.license_info import LicenseInfo, LicenseDeclarationType
        from riot_sbom.data.copyright_info import CopyrightInfo, CopyrightDeclarationType
        from riot_sbom.data.author_info import AuthorInfo, AuthorDeclarationType
        from riot_sbom.data.checked_url import CheckedUrl
        from riot_sbom.data.package_info import PackageInfo, PackageReference
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
                with open(source_file, 'wt') as f:
                    f.write(f"// Source file {ii}\n")
                    f.write("// SPDX-License-Identifier: MIT\n")
                    f.write("// Copyright (C) 2025 Example Holder\n")
                    f.write("// @author John Doe <john@doe.com>\n")
                app_info.files.append(FileInfo(
                    path=source_file,
                    package=PackageReference.from_package_info(pkg),
                    licenses=None,
                    copyrights=None,
                    authors=None
                ))
                with source_file.open('rt') as f:
                    logger.debug(f"Added file {source_file} to app_info.files with content:\n"
                                f"{f.read()}")
            this_plugin = ScancodeScanner()
            app_info = this_plugin.run(app_info, None)
            for file in app_info.files:
                self.assertIsNotNone(file.copyrights, f"Expected copyrights to be added for file: {file}")
                self.assertIsNotNone(file.licenses, f"Expected licenses to be added for file: {file}")
                self.assertIsNotNone(file.authors, f"Expected authors to be added for file: {file}")
                if not file.copyrights or not file.licenses or not file.authors:
                    continue # make linter happy
                self.assertEqual(len(file.copyrights), 1, f"Expected exactly one copyright for file: {file}")
                self.assertEqual(len(file.licenses), 1, f"Expected exactly one license for file: {file}")
                self.assertEqual(len(file.authors), 1, f"Expected exactly one author for file: {file}")


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
