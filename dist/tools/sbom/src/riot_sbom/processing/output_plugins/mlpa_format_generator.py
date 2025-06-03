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

import json
import logging
logger = logging.getLogger(__name__)
import pathlib
import tempfile
from typing import List
import unittest

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.util.file_or_stdout_resource import FileOrStdoutResource
from riot_sbom.data.license_info import LicenseInfo, LicenseDeclarationType
from riot_sbom.data.copyright_info import CopyrightInfo, CopyrightDeclarationType
from riot_sbom.data.app_info import AppInfo
from riot_sbom.data.file_info import DigestType
from riot_sbom.data.package_info import PackageInfo, PackageReference
from riot_sbom.data.author_info import AuthorInfo, AuthorDeclarationType
from riot_sbom.data.checked_url import CheckedUrl
from riot_sbom.data.file_info import FileInfo


__all__ = ["MlpaJsonGenerator"]

class MlpaJsonGenerator(Plugin):
    def get_name(self):
        return "mlpa-json-generator"

    def get_description(self):
        return "Generates an ML!PA-format complying JSON output."

    def run(self, app_info: AppInfo, output_file_prefix: pathlib.Path) -> AppInfo:
        """
        Generates an ML!PA-format complying JSON output.
        The output is a JSON file that contains information about the package,
        its dependencies, and the source files used in the package.
        """
        def _get_copyright_string(copyrights: List[CopyrightInfo] | None) -> str:
            if not copyrights:
                return ""
            copyright_string = ""
            for copyright in copyrights:
                copyright_string += f"{copyright.years} {copyright.holder};\n"
            return copyright_string.strip('; \t\n')
        def _get_license_string(licenses: List[LicenseInfo] | None) -> str:
            if not licenses:
                return ""
            license_string = ""
            for license in licenses:
                license_string += f"{license.declaration_text};\n"
            return license_string.strip('; \t\n')
        json_output = {}
        app_pkg = app_info.packages.get(app_info.app_package_ref, None)
        if app_pkg is None:
            raise ValueError("App package reference not found in app_info.packages")
        json_output["packageName"] = app_pkg.name
        json_output["packageVersion"] = app_pkg.version
        json_output["packageLicense"] = _get_license_string(app_pkg.licenses)
        json_output["packageCopyright"] = _get_copyright_string(app_pkg.copyrights)
        json_output["dependencies"] = {}
        for pkg_ref, pkg in app_info.packages.items():
            if pkg_ref == app_info.app_package_ref:
                logger.debug(f"Skipping package {pkg} in dependencies list")
                continue
            json_output["dependencies"][pkg.name] = {
                "version": pkg.version,
                "type": "source",
                "license": _get_license_string(pkg.licenses),
                "copyright": _get_copyright_string(pkg.copyrights),
                "checksum": None
            }
            if pkg.download_url and pkg.download_url.get():
                json_output["dependencies"][pkg.name]["source"] = pkg.download_url.get()
            elif pkg.supplier:
                json_output["dependencies"][pkg.name]["source"] = pkg.supplier
            else:
                json_output["dependencies"][pkg.name]["source"] = ""
        json_output["sourceFiles"] = {}
        for file in app_info.files:
            pkg = app_info.packages.get(file.package, None) if file.package else None
            json_output["sourceFiles"][file.path.name] = {
                "version": pkg.version if pkg else "",
                "checksum": file.digests[DigestType.MD5],
                "license": _get_license_string(file.licenses),
                "copyright": _get_copyright_string(file.copyrights)
            }
        with FileOrStdoutResource(output_file_prefix, ".mlpa.json") as output_file:
            logger.info(f"Writing ML!PA JSON output to {output_file.name}")
            json.dump(json_output, output_file, indent=2)
        return app_info

class TestMlpaJsonGenerator(unittest.TestCase):
    def test_get_name(self):
        plugin = MlpaJsonGenerator()
        self.assertEqual(plugin.get_name(), "mlpa-json-generator")

    def test_get_description(self):
        plugin = MlpaJsonGenerator()
        self.assertEqual(plugin.get_description(), "Generates an ML!PA-format complying JSON output.")

    def test_run(self):
        schema = """
{
  "$schema": "http://json-schema.org/draft-04/schema#",
  "type": "object",
  "properties": {
    "packageName": {
      "type": "string"
    },
    "packageVersion": {
      "type": "string"
    },
    "packageLicense": {
      "type": "string"
    },
    "packageCopyright": {
      "type": "string"
    },
    "dependencies": {
      "type": "object",
      "properties": {
        "packageName": {
          "type": "object",
          "properties": {
            "version": {
              "type": "string"
            },
            "source": {
              "type": "string"
            },
            "type": {
              "type": "string"
            },
            "license": {
              "type": "string"
            },
            "copyright": {
              "type": "string"
            },
            "checksum": {
              "type": "string"
            }
          },
          "required": [
            "version",
            "source",
            "type"
          ]
        }
      }
    },
    "sourceFiles": {
      "type": "object",
      "properties": {
        "filename": {
          "type": "object",
          "properties": {
            "version": {
              "type": "string"
            },
            "checksum": {
              "type": "string"
            },
            "license": {
              "type": "string"
            },
            "copyright": {
              "type": "string"
            }
          },
          "required": [
            "version",
            "checksum",
            "license"
          ]
        }
      }
    }
  },
  "required": [
    "packageName",
    "packageVersion",
    "dependencies"
  ]
}
"""
        app_package=PackageInfo(
            name="example-app",
            version="1.0.0",
            licenses=[LicenseInfo(declaration_text="MIT",
                                    declaration_type=LicenseDeclarationType.EXACT_REFERENCE,
                                    license_text=None, url=None)],
            copyrights=[CopyrightInfo(holder="Example Holder",
                                        years="2025",
                                        declaration_type=CopyrightDeclarationType.TEXT_TAGGED,
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
                PackageReference.from_package_info(board_package): board_package,
                PackageReference("example-pkg1", pathlib.Path("/path/to/source")): PackageInfo(
                    name="example-pkg1",
                    version="1.0.0",
                    licenses=[LicenseInfo(declaration_text="MIT",
                                        declaration_type=LicenseDeclarationType.EXACT_REFERENCE,
                                        license_text=None, url=None)],
                    copyrights=[CopyrightInfo(holder="Example Holder",
                                            years="2025",
                                            declaration_type=CopyrightDeclarationType.TEXT_TAGGED,
                                            url=None)],
                    download_url=CheckedUrl("https://example.com/package.tar.gz"),
                    authors=[AuthorInfo(name="John Doe",
                                        email="john@doe.com",
                                        declaration_type=AuthorDeclarationType.TEXT_TAGGED)],
                    source_dir=pathlib.Path("/path/to/source"),
                    supplier="Example Supplier"
                ),
                PackageReference("example-pkg2", pathlib.Path("/path/to/source")): PackageInfo(
                    name="example-pkg2",
                    version="1.0.0",
                    licenses=[LicenseInfo(declaration_text="MIT",
                                        declaration_type=LicenseDeclarationType.EXACT_REFERENCE,
                                        license_text=None, url=None)],
                    copyrights=[CopyrightInfo(holder="Example Holder",
                                            years="2025",
                                            declaration_type=CopyrightDeclarationType.TEXT_TAGGED,
                                            url=None)],
                    download_url=CheckedUrl("https://example.com/package.tar.gz"),
                    authors=[AuthorInfo(name="John Doe",
                                        email="john@doe.com",
                                        declaration_type=AuthorDeclarationType.TEXT_TAGGED)],
                    source_dir=pathlib.Path("/path/to/source"),
                    supplier="Example Supplier")
            },
            files=[]
        )
        with tempfile.TemporaryDirectory() as temp_dir:
            temp_dir_path = pathlib.Path(temp_dir)
            for ii, pkg in enumerate(app_info.packages.values()):
                source_file = temp_dir_path / f"source_file_{ii}.c"
                with open(source_file, 'w') as f:
                    f.write(f"// Source file {ii}")
                app_info.files.append(FileInfo(
                    path=source_file,
                    package=PackageReference.from_package_info(pkg),
                    licenses=pkg.licenses,
                    copyrights=pkg.copyrights,
                    authors=pkg.authors
                ))
            plugin = MlpaJsonGenerator()
            output_file_prefix = temp_dir_path / "test_output"
            new_app_info = plugin.run(app_info, output_file_prefix)
            self.assertIsInstance(new_app_info, AppInfo)
            self.assertEqual(new_app_info, app_info)
            self.assertTrue(output_file_prefix.with_suffix(".mlpa.json").exists())
            with open(output_file_prefix.with_suffix(".mlpa.json"), 'rt') as f:
                json_data = json.load(f)
            self.assertEqual(json_data["packageName"], app_package.name)
            self.assertEqual(json_data["packageVersion"], app_package.version)
            self.assertEqual(json_data["packageLicense"], "MIT")
            self.assertEqual(json_data["packageCopyright"], "2025 Example Holder")
            self.assertIn("example-pkg1", json_data["dependencies"])
            self.assertIn("example-pkg2", json_data["dependencies"])
            for file in app_info.files:
                self.assertIn(file.path.name, json_data["sourceFiles"])
                self.assertEqual(json_data["sourceFiles"][file.path.name]["checksum"], file.digests[DigestType.MD5])
                self.assertEqual(json_data["sourceFiles"][file.path.name]["license"], "MIT")
                self.assertEqual(json_data["sourceFiles"][file.path.name]["copyright"], "2025 Example Holder")
                if not file.package:
                    continue
                file_pkg = app_info.packages[file.package]
                if not file_pkg:
                    self.fail(f"Package for file {file.path} not found in app_info.packages")
                self.assertEqual(json_data["sourceFiles"][file.path.name]["version"],
                                    file_pkg.version)
            for pkg_ref, pkg in app_info.packages.items():
                if pkg_ref == app_info.app_package_ref:
                    continue
                self.assertIn(pkg.name, json_data["dependencies"])
                self.assertEqual(json_data["dependencies"][pkg.name]["version"], pkg.version)
                self.assertEqual(json_data["dependencies"][pkg.name]["license"], "MIT")
                self.assertEqual(json_data["dependencies"][pkg.name]["copyright"], "2025 Example Holder")
                self.assertEqual(json_data["dependencies"][pkg.name]["checksum"], None)
            has_jsonschema = False
            try:
                import jsonschema
                has_jsonschema = True
            except ImportError:
                self.skipTest("jsonschema module not available, skipping schema validation test")
            if has_jsonschema:
                self.assertIsNone(jsonschema.validate(json_data, json.loads(schema)))


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
