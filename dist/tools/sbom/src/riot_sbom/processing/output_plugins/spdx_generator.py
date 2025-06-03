"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

__all__ = ["SpdxGenerator", "SpdxBuilder"]


if __name__ == "__main__":
    # update search path for local testing
    import sys
    import pathlib
    sys.path.insert(0,pathlib.Path(__file__).absolute().parents[3].as_posix())


import logging
logger = logging.getLogger(__name__)
import os
import pathlib
from datetime import datetime
from typing import List, Tuple, Any
import tempfile
import unittest
import uuid

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.util.file_or_stdout_resource import FileOrStdoutResource
from riot_sbom.data.app_info import AppInfo
from riot_sbom.data.package_info import PackageInfo, PackageReference
from riot_sbom.data.file_info import FileInfo, DigestType
from riot_sbom.data.license_info import LicenseInfo, LicenseDeclarationType
from riot_sbom.data.copyright_info import CopyrightInfo, CopyrightDeclarationType
from riot_sbom.data.author_info import AuthorInfo, AuthorDeclarationType
from riot_sbom.data.checked_url import CheckedUrl

from spdx_tools.common.spdx_licensing import spdx_licensing
from spdx_tools.spdx.model import (
    Actor,
    ActorType,
    Checksum,
    ChecksumAlgorithm,
    CreationInfo,
    Document,
    ExternalPackageRef,
    ExternalPackageRefCategory,
    File,
    FileType,
    Package,
    PackagePurpose,
    PackageVerificationCode,
    Relationship,
    RelationshipType,
)
from spdx_tools.spdx.model.spdx_none import SpdxNone
from spdx_tools.spdx.model.spdx_no_assertion import SpdxNoAssertion
from spdx_tools.spdx.validation.document_validator import validate_full_spdx_document
from spdx_tools.spdx.validation.validation_message import ValidationMessage
from spdx_tools.spdx.writer.tagvalue.tagvalue_writer import write_document_to_stream

class SpdxBuilder:
    def __init__(self):
        """
        :param document_name: The name of the SPDX document.
        :param data_license: The SPDX license identifier of the SPDX document.
        :param document_namespace: The namespace of the SPDX document (URL-formatted).
        :param creators: A list of tuples containing the name and email of the creators of the SPDX document.
        """
        self.__document_ref = "SPDXRef-DOCUMENT"
        self.__application_ref = "SPDXRef-Application"
        creation_info = CreationInfo(
            spdx_version="SPDX-2.3",
            spdx_id=self.__document_ref,
            name="Package SBOM for RIOT Application",
            data_license="CC0-1.0",
            document_namespace=f'https://spdx.org/spdxdocs/riot-sbom-{uuid.uuid4()}',
            creators=[Actor(ActorType.TOOL, "riot_sbom", "")],
            created=datetime.now(),
        )
        self.__document = Document(creation_info)
        self.__package_ref_to_spdx_ref_map = {}

    def set_creation_info(self, document_name: str, data_license: str,
                          document_namespace: str, creators: List[Tuple[str, str]]):
        """
        Set the creation information for the SPDX document.
        :param document_name: The name of the SPDX document.
        :param data_license: The SPDX license identifier of the SPDX document.
        :param document_namespace: The namespace of the SPDX document (URL-formatted).
        :param creators: A list of tuples containing the name and email of the creators of the SPDX document.
        """
        logger.debug("Setting creation information for SPDX document")
        self.__document.creation_info.name = document_name
        self.__document.creation_info.data_license = data_license
        self.__document.creation_info.document_namespace = document_namespace
        self.__document.creation_info.creators = [
            Actor(ActorType.PERSON, name, email) for name, email in creators
        ]

    def add_package(self, package_info: PackageInfo, dependent_package_ref: PackageReference | None=None):
        """
        Add a package to the SPDX document.
        :param package_info: The package information to add.
        :param dependent_package_ref: The reference of the package that this package depends on, if any.

        :raises ValueError: If the dependent package has not been added yet.
        :raises ValueError: If the package to add has already been added.
        """
        if dependent_package_ref and dependent_package_ref not in self.__package_ref_to_spdx_ref_map:
            raise ValueError(f"Dependent package {dependent_package_ref} must be added first.")
        if PackageReference.from_package_info(package_info) in self.__package_ref_to_spdx_ref_map:
            raise ValueError(f"Package {package_info.name} has already been added to the SPDX document.")
        dependent_package_ref = (self.__package_ref_to_spdx_ref_map[dependent_package_ref]
                                if dependent_package_ref else None)
        logger.debug(f"Adding package {package_info.name} (dependency of {dependent_package_ref})")
        package_ref = (self.__application_ref
                       if not dependent_package_ref
                       else f"SPDXRef-Package-{len(self.__document.packages)}")
        self.__package_ref_to_spdx_ref_map[PackageReference.from_package_info(package_info)] = package_ref
        if not package_info.licenses:
            license_explicit = (SpdxNone()
                                if package_info.licenses == []
                                else SpdxNoAssertion())
            license_concluded = license_explicit
        else:
            license_concluded, license_explicit = self._format_licenses(
                package_info.licenses, package_info.source_dir if package_info.source_dir else None)
            if len(license_explicit) > 1:
                logger.warning(f"Package {package_info.name} is associated with more than one explicit license: {package_info.licenses}."
                                "Picking the first license as explicit license.")
            license_explicit = license_explicit[0]
        package = Package(
            name=package_info.name,
            spdx_id=package_ref,
            download_location=package_info.download_url.get() if package_info.download_url else SpdxNoAssertion(),
            version=package_info.version if package_info.version else None,
            supplier=Actor(ActorType.ORGANIZATION, package_info.supplier, "") if package_info.supplier is not None else None,
            files_analyzed=True,
            license_concluded=license_concluded,
            #license_info_from_files=[spdx_licensing.parse("GPL-2.0-only"), spdx_licensing.parse("MIT")],
            license_declared=license_explicit,
            copyright_text=self._format_copyrights(package_info.copyrights),
            primary_package_purpose=PackagePurpose.SOURCE if dependent_package_ref else PackagePurpose.APPLICATION,
        )
        self.__document.packages.append(package)
        if self.__application_ref == package_ref:
            relationship = Relationship(self.__document_ref, RelationshipType.DESCRIBES, package_ref)
            self.__document.relationships.append(relationship)
        else:
            relationship = Relationship(self.__application_ref, RelationshipType.DEPENDS_ON, package_ref)
            self.__document.relationships.append(relationship)


    def add_file(self, file_info: FileInfo, file_package_info: PackageInfo | None=None):
        logger.debug(f"Adding file {file_info.path} in package {file_package_info}")
        if file_info.package and not file_package_info:
            raise ValueError(f"File {file_info.path} belongs to package {file_info.package}, "
                             f"but no package information was provided.")
        if file_package_info and not file_info.package:
            logger.warning(f"File {file_info.path} does not belong to any package, "
                             f"but package information was provided for {file_package_info.name}. Ignoring package information.")
            file_package_info = None
        if file_package_info and PackageReference.from_package_info(file_package_info) not in self.__package_ref_to_spdx_ref_map:
            raise ValueError(f"Package {file_package_info.name} must be added first.")
        if file_package_info and PackageReference.from_package_info(file_package_info) != file_info.package:
            raise ValueError(f"File {file_info.path} does not belong to package {file_package_info.name}.")
        file_ref = f"SPDXRef-File-{len(self.__document.files) + 1}"
        if file_info.package is None:
            file_package_ref = None
        else:
            file_package_ref = self.__package_ref_to_spdx_ref_map.get(
                file_info.package, None)
        file_name = (file_info.path.name
                     if not file_package_info
                     else os.path.relpath(file_info.path, file_package_info.source_dir))
        if not file_info.licenses:
            license_explicit = [SpdxNone()
                                if file_info.licenses == []
                                else SpdxNoAssertion()]
            license_concluded = license_explicit[0]
        else:
            license_concluded, license_explicit = self._format_licenses(
                file_info.licenses, file_info.path)
        file = File(
            name=file_name,
            spdx_id=file_ref,
            file_types=[FileType.SOURCE],
            checksums=[
                Checksum(ChecksumAlgorithm.SHA1, file_info.digests[DigestType.SHA1]),
                Checksum(ChecksumAlgorithm.MD5, file_info.digests[DigestType.MD5]),
            ],
            license_concluded=license_concluded,
            license_info_in_file=license_explicit,
            copyright_text=self._format_copyrights(file_info.copyrights)
        )
        self.__document.files.append(file)
        if file_package_ref:
            relationship = Relationship(file_package_ref, RelationshipType.CONTAINS, file_ref)
            self.__document.relationships.append(relationship)
        else:
            relationship = Relationship(self.__application_ref, RelationshipType.DEPENDS_ON, file_ref)
            self.__document.relationships.append(relationship)

    def write(self, file_path: pathlib.Path | None):
        logger.info(f"Writing SPDX to {file_path if file_path else 'stdout'}")
        with FileOrStdoutResource(file_path) as output_stream:
            write_document_to_stream(self.__document, output_stream, validate=False)

    def validate(self) -> List[ValidationMessage]:
        return validate_full_spdx_document(self.__document)

    @staticmethod
    def _format_licenses(licenses: List[LicenseInfo], path: pathlib.Path | None) -> Tuple[Any, List[Any]]:
        spdx_licenses_concluded = []
        spdx_licenses_in_file = []
        for license_info in licenses:
            try:
                spdx_license = spdx_licensing.parse(license_info.declaration_text)
            except:
                logger.warning(f"License {license_info.declaration_text} in {path} could not be parsed. "
                                "Skipping.")
                continue
            if spdx_license is None:
                logger.warning(f"License {license_info.declaration_text} in {path} could not be parsed. "
                                "Using no assertion instead.")
                spdx_license = SpdxNoAssertion()
            if license_info.declaration_type == LicenseDeclarationType.DERIVED:
                if spdx_license not in spdx_licenses_concluded:
                    spdx_licenses_concluded.append(spdx_license)
            else:
                if spdx_license not in spdx_licenses_in_file:
                    spdx_licenses_in_file.append(spdx_license)
        spdx_licenses_concluded = [l for l in spdx_licenses_concluded if l != SpdxNoAssertion()]
        if len(spdx_licenses_concluded) > 1:
            logger.warning(f"{path} is associated with the following licenses: {licenses}."
                            "Picking the first license as concluded license.")
            license_concluded = spdx_licenses_concluded[0]
        elif len(spdx_licenses_concluded) == 0:
            license_concluded = SpdxNoAssertion()
        else:
            license_concluded = spdx_licenses_concluded[0]
        license_explicit = [l for l in spdx_licenses_in_file if l != SpdxNoAssertion()]
        if len(license_explicit) == 0:
            license_explicit = [SpdxNoAssertion()]
        return license_concluded, license_explicit

    @staticmethod
    def _format_copyrights(copyrights: List[CopyrightInfo] | None) -> str | SpdxNoAssertion:
        return ("".join(f'<text>{(c.tag + " ") if c.tag else ""}{c.years} {c.holder}</text>' for c in copyrights)
                    if copyrights else SpdxNoAssertion())


class SpdxGenerator(Plugin):
    def get_name(self):
        return "spdx-generator"

    def get_description(self):
        return "Writes an spdx file in tag:value format."

    def run(self, app_info: AppInfo, output_file_prefix: pathlib.Path | None) -> AppInfo:
        logger.info("Generating SPDX document")
        builder = SpdxBuilder()
        builder.set_creation_info(
            document_name=f"SBOM for {app_info.packages[app_info.app_package_ref].name} application",
            data_license="CC0-1.0",
            document_namespace=f'https://spdx.org/spdxdocs/riot-sbom-{uuid.uuid4()}',
            creators=([(author.name, author.email) for author in (app_info.packages[app_info.app_package_ref].authors or [])]
                      if app_info.packages[app_info.app_package_ref].authors else [])
        )
        builder.add_package(app_info.packages[app_info.app_package_ref])
        for package_ref in app_info.packages:
            if package_ref == app_info.app_package_ref:
                continue
            builder.add_package(
                app_info.packages[package_ref],
                app_info.app_package_ref
            )
        for file in app_info.files:
            builder.add_file(file, app_info.packages[file.package] if file.package else None)
        output_file = output_file_prefix.with_suffix(".sbom.spdx") if output_file_prefix else None
        logger.debug(f"SPDX document file name: {output_file if output_file else 'stdout'}")
        builder.write(output_file)
        validation_messages = builder.validate()
        if validation_messages:
            logger.error(f"SPDX document validation failed with {len(validation_messages)} messages.")
            for message in validation_messages:
                logger.error(message)
        else:
            logger.info("SPDX document validation succeeded.")
        return app_info


class TestSpdxGenerator(unittest.TestCase):
    def test_spdx_generator(self):
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
                                            tag="SPDX-FileCopyrightText:",
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
                                            tag="SPDX-FileCopyrightText:",
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
            plugin = SpdxGenerator()
            output_file_prefix = temp_dir_path / "test_output"
            new_app_info = plugin.run(app_info, output_file_prefix)
            self.assertIsInstance(new_app_info, AppInfo)
            self.assertEqual(new_app_info, app_info)
            self.assertTrue(output_file_prefix.with_suffix(".sbom.spdx").exists())

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
