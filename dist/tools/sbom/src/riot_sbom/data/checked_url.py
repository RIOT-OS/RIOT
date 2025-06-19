"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

import urllib.parse
import re
import unittest

__all__ = ["CheckedUrl"]


_git_ssh_url_regex = re.compile(
    r"^(?P<user>[^@]+)@(?P<host>[^:]+):/?(?P<path>.+)$"
)

class CheckedUrl:
    """
    Class to represent a checked URL.
    This class is used to store the URL and the result of the check.
    """

    def __init__(self, text: str) -> None:
        self.__text: str = text
        self.__valid: bool = False
        self.__url: str = ""
        self.__check_url()

    def __check_url(self) -> None:
        parsed_url = urllib.parse.urlsplit(self.__text)
        if parsed_url.scheme and parsed_url.netloc:
            self.__url = urllib.parse.urlunsplit(parsed_url)
            self.__valid = True
        else:
            self.__url = ""
            self.__valid = False
        if not self.__valid:
            # This might be a git ssh URL
            # e.g. user@host:repo.git
            # Let's try to parse it and convert it to a valid URL
            git_url_match = _git_ssh_url_regex.match(self.__text)
            if git_url_match:
                user = git_url_match.group("user")
                host = git_url_match.group("host")
                path = git_url_match.group("path")
                self.__url = f"git+ssh://{user}@{host}/{path}"
                self.__valid = True

    def get(self) -> str:
        return str(self)

    def __str__(self) -> str:
        return self.__url


class TestCheckedUrl(unittest.TestCase):
    def test_valid_url(self):
        url = CheckedUrl("https://example.com")
        self.assertEqual(str(url), "https://example.com")

    def test_invalid_url(self):
        url = CheckedUrl("invalid_url")
        self.assertEqual(str(url), "")

    def test_git_ssh_url(self):
        url = CheckedUrl("user@host:repo.git")
        self.assertEqual(str(url), "git+ssh://user@host/repo.git")
        url = CheckedUrl("user@host:/path/to/repo.git")
        self.assertEqual(str(url), "git+ssh://user@host/path/to/repo.git")
        url = CheckedUrl("user@host:path/to/repo.git")
        self.assertEqual(str(url), "git+ssh://user@host/path/to/repo.git")


if __name__ == "__main__":
    import logging
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
