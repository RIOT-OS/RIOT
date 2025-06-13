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
from typing import List, Tuple
import unittest
import uuid

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.util.file_or_stdout_resource import FileOrStdoutResource
from riot_sbom.data.app_info import AppInfo
from riot_sbom.data.package_info import PackageInfo, PackageReference
from riot_sbom.data.file_info import FileInfo, DigestType
from riot_sbom.data.license_info import LicenseDeclarationType

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
        package = Package(
            name=package_info.name,
            spdx_id=package_ref,
            download_location=package_info.download_url.get() if package_info.download_url else SpdxNoAssertion(),
            version=package_info.version if package_info.version else None,
            #file_name=package_info.source_dir.name, # probably only required for archives
            #supplier=Actor(ActorType.PERSON, "Jane Doe", "jane.doe@example.com"),
            #originator=Actor(ActorType.ORGANIZATION, "some organization", "contact@example.com"),
            files_analyzed=True,
            # verification_code=PackageVerificationCode(
            #     value="d6a770ba38583ed4bb4525bd96e50461655d2758", excluded_files=["./some.file"]
            # ),
            # checksums=[
            #     Checksum(ChecksumAlgorithm.SHA1, "d6a770ba38583ed4bb4525bd96e50461655d2758"),
            #     Checksum(ChecksumAlgorithm.MD5, "624c1abb3664f4b35547e7c73864ad24"),
            # ],
            #license_concluded=spdx_licensing.parse("GPL-2.0-only OR MIT"),
            #license_info_from_files=[spdx_licensing.parse("GPL-2.0-only"), spdx_licensing.parse("MIT")],
            #license_declared=(spdx_licensing.parse(package_info.license)
            #                  if package_info.license else SpdxNone()),
            #license_comment=None,
            #copyright_text=package_info.copyright,
            #description="package description",
            #attribution_texts=["package attribution"],
            primary_package_purpose=PackagePurpose.SOURCE if dependent_package_ref else PackagePurpose.APPLICATION,
            #release_date=datetime(2015, 1, 1),
            # external_references=[
            #     ExternalPackageRef(
            #         category=ExternalPackageRefCategory.OTHER,
            #         reference_type="http://reference.type",
            #         locator="reference/locator",
            #         comment="external reference comment",
            #     )
            # ],
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
            file_license = [SpdxNone()
                                if file_info.licenses == []
                                else SpdxNoAssertion()]
            license_concluded = file_license[0]
        if file_info.licenses:
            spdx_licenses_concluded = []
            spdx_licenses_in_file = []
            for license_info in file_info.licenses:
                try:
                    spdx_license = spdx_licensing.parse(license_info.declaration_text)
                except:
                    logger.warning(f"License {license_info.declaration_text} in file {file_info.path} could not be parsed. "
                                    "Skipping.")
                    continue
                if spdx_license is None:
                    logger.warning(f"License {license_info.declaration_text} in file {file_info.path} could not be parsed. "
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
                logger.warning(f"File {file_info.path} has multiple licenses: {file_info.licenses}. "
                                "Picking the first license.")
                license_concluded = spdx_licenses_concluded[0]
            elif len(spdx_licenses_concluded) == 0:
                license_concluded = SpdxNoAssertion()
            else:
                license_concluded = spdx_licenses_concluded[0]
            file_license = [l for l in spdx_licenses_in_file if l != SpdxNoAssertion()]
            if len(file_license) == 0:
                file_license = [SpdxNoAssertion()]
        file = File(
            name=file_name,
            spdx_id=file_ref,
            file_types=[FileType.SOURCE],
            checksums=[
                Checksum(ChecksumAlgorithm.SHA1, file_info.digests[DigestType.SHA1]),
                Checksum(ChecksumAlgorithm.MD5, file_info.digests[DigestType.MD5]),
            ],
            license_concluded=license_concluded,
            license_info_in_file=file_license,
            copyright_text=("".join(f'<text>{c.years} {c.holder}</text>' for c in file_info.copyrights)
                    if file_info.copyrights else SpdxNoAssertion())
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


class SpdxGenerator(Plugin):
    def get_name(self):
        return "spdx-generator"

    def get_description(self):
        return "Writes an spdx file in tag:value format."

    def run(self, app_info: AppInfo, output_file_prefix: pathlib.Path) -> AppInfo:
        logger.info("Generating SPDX document")
        builder = SpdxBuilder()
        builder.set_creation_info(
            document_name=f"SBOM for {app_info.packages[app_info.app_package_ref].name} application",
            data_license="CC0-1.0",
            document_namespace=f'https://spdx.org/spdxdocs/riot-sbom-{uuid.uuid4()}',
            creators=([(author.name, author.email) for author in app_info.packages[app_info.app_package_ref].authors]
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
        output_file = output_file_prefix.parent / (output_file_prefix.name + "-sbom.spdx")
        builder.write(output_file)
        validation_messages = builder.validate()
        if validation_messages:
            logger.error(f"SPDX document validation failed with {len(validation_messages)} messages.")
            for message in validation_messages:
                logger.error(message)
        else:
            logger.info("SPDX document validation succeeded.")
        return app_info
