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
import re
from typing import List
import unittest

from riot_sbom.processing.plugin_type import Plugin
from riot_sbom.data.author_info import AuthorInfo, AuthorDeclarationType


__all__ = ["AuthorsScanner"]


_author_matcher = re.compile(r"(?P<tag>@?[Aa][Uu][Tt][Hh][Oo][Rr]:?[ \t]+)?(?P<name>\"?[^@\"<>]+\"?[ \t]+)?<?(?P<email>[a-zA-Z0-9\._-]+@[a-zA-Z0-9-]+\.[a-zA-Z]+)?>?",
                             re.UNICODE)


def _strip_author_name(name: str) -> str:
    """
    Strip the author name of leading and trailing whitespace and quotes, if any.
    """
    return name.strip(" \t\"")


class AuthorsScanner(Plugin):
    def get_name(self):
        return "authors-scanner"

    def get_description(self):
        return "Scans @author fields from source files."

    def run(self, app_info, _):
        for file in app_info.files:
            if file.path.exists():
                authors: List[AuthorInfo] = []
                with file.path.open("rt") as f:
                    for line in f:
                        m = _author_matcher.search(line)
                        if m:
                            author = m.group("name")
                            if author:
                                author = _strip_author_name(author)
                            email = m.group("email")
                            tag = m.group("tag")
                            if tag and (author or email):
                                # we accept any data if preceded by an author tag
                                author_info = AuthorInfo(
                                    name=author, email=email,
                                    declaration_type=AuthorDeclarationType.TEXT_TAGGED)
                            elif author and email:
                                # we accept full name and email records only otherwise
                                author_info = AuthorInfo(
                                    name=_strip_author_name(author), email=email,
                                    declaration_type=AuthorDeclarationType.TEXT_MATCHED)
                            else:
                                continue
                            authors.append(author_info)
                            logger.debug(f"Found author: \"{author}\" <{email}> in file: {file.path}")
                if authors:
                    file.authors = authors
        return app_info


class TestAuthorMatcher(unittest.TestCase):
    def test_author_match_success(self):
        test_cases_match = {
            "@author \"John Doe\" <john@doe.com>": ("John Doe", "john@doe.com", True),
            "@author \"Jane Fonda-Doe\" <j.f@doe.com>": ("Jane Fonda-Doe", "j.f@doe.com", True),
            "@author \"J. Doe\" <john@doe.com>": ("J. Doe", "john@doe.com", True),
            "@author \"J. Fonda-Doe\" <j.f@doe.com>": ("J. Fonda-Doe", "j.f@doe.com", True),
            "@author J. Doe <john@doe.com>": ("J. Doe", "john@doe.com", True),
            "@author J. Fonda-Doe <j.f@doe.com>": ("J. Fonda-Doe", "j.f@doe.com", True),
            "@author J. Doe john@doe.com": ("J. Doe", "john@doe.com", True),
            "@author J. Fonda-Doe j.f@doe.com": ("J. Fonda-Doe", "j.f@doe.com", True),
            "Author: \"John Doe\" <john@doe.com>": ("John Doe", "john@doe.com", True),
            "Author: \"Jane Fonda-Doe\" <j.f@doe.com>": ("Jane Fonda-Doe", "j.f@doe.com", True),
            "Author: \"J. Doe\" <john@doe.com>": ("J. Doe", "john@doe.com", True),
            "Author: \"J. Fonda-Doe\" <j.f@doe.com>": ("J. Fonda-Doe", "j.f@doe.com", True),
            "Author: J. Doe <john@doe.com>": ("J. Doe", "john@doe.com", True),
            "Author: J. Fonda-Doe <j.f@doe.com>": ("J. Fonda-Doe", "j.f@doe.com", True),
            "Author: J. Doe john@doe.com": ("J. Doe", "john@doe.com", True),
            "Author: J. Fonda-Doe j.f@doe.com": ("J. Fonda-Doe", "j.f@doe.com", True),
            "\"John Doe\" <john@doe.com>": ("John Doe", "john@doe.com", False),
            "\"Jane Fonda-Doe\" <j.f@doe.com>": ("Jane Fonda-Doe", "j.f@doe.com", False),
            "\"J. Doe\" <john@doe.com>": ("J. Doe", "john@doe.com", False),
            "\"J. Fonda-Doe\" <j.f@doe.com>": ("J. Fonda-Doe", "j.f@doe.com", False),
            "J. Doe <john@doe.com>": ("J. Doe", "john@doe.com", False),
            "J. Fonda-Doe <j.f@doe.com>": ("J. Fonda-Doe", "j.f@doe.com", False),
            "J. Doe john@doe.com": ("J. Doe", "john@doe.com", False),
            "J. Fonda-Doe j.f@doe.com": ("J. Fonda-Doe", "j.f@doe.com", False)
        }
        for t, a in test_cases_match.items():
            m = _author_matcher.search(t)
            self.assertIsNotNone(m, f"Expected match for line: {t}")
            if m: # make the linter happy
                self.assertIsNotNone(m.group("name"),
                                 f"Expected name for line: {t} (groups: {m.groups()})")
                self.assertIsNotNone(m.group("email"),
                                 f"Expected email for line: {t} (groups: {m.groups()})")
                self.assertEqual(_strip_author_name(m.group("name")), a[0],
                                 f"Expected name: {a[0]} for line: {t} (groups: {m.groups()})")
                self.assertEqual(m.group("email"), a[1],
                                 f"Expected email: {a[1]} for line: {t} (groups: {m.groups()})")
                self.assertEqual(m.group("tag") is not None, a[2],
                                 f"Expected tag: {a[2]} for line: {t} (groups: {m.groups()})")


if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
