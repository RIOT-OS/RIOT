"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

from dataclasses import dataclass
from enum import Enum
import hashlib
from pathlib import Path
from typing import List
import unittest
import weakref

from .package_info import PackageReference
from .copyright_info import CopyrightInfo
from .license_info import LicenseInfo
from .author_info import AuthorInfo

__all__ = ['DigestType', 'FileInfo']


class DigestType(Enum):
    """
    Different types of digests for a file.
    """
    SHA1 = 'sha1'
    MD5 = 'md5'
    SHA256 = 'sha256'
    SHA512 = 'sha512'
    SHA3_256 = 'sha3_256'
    SHA3_384 = 'sha3_384'
    SHA3_512 = 'sha3_512'


@dataclass
class FileInfo:
    path: Path # path to the file
    package: PackageReference | None # identifying data which references the package this file belongs to
    licenses: List[LicenseInfo] | None # none means no license, empty list means unknown
    copyrights: List[CopyrightInfo] | None # none means no copyright, empty list means unknown
    authors: List[AuthorInfo] | None # none means no author (e.g. auto-generated), empty list means unknown

    def __post_init__(self):
        if not self.path.exists():
            raise FileNotFoundError(f"File {self.path} does not exist.")
        if not self.path.is_file():
            raise ValueError(f"Path {self.path} is not a file.")
        self.digests = {
            digest_type: FileInfo.get_digest(self.path, digest_type) for digest_type in DigestType
        }

    @staticmethod
    def get_digest(path: Path, digest_type: DigestType) -> str:
        with open(path, 'rb') as f:
            return hashlib.file_digest(f, digest_type.value).hexdigest()


class TestFileInfo(unittest.TestCase):
    def test_get_digest(self):
        file = Path(__file__)
        # Create a FileInfo object
        file_info = FileInfo(path=file, package=None,
                             licenses=[], copyrights=None, authors=None)
        for digest_type in DigestType:
            expected_digest = hashlib.new(digest_type.value, file.read_bytes()).hexdigest()
            self.assertEqual(file_info.digests[digest_type], expected_digest, f"Digest mismatch for {digest_type.value}")


if __name__ == '__main__':
    import logging
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
