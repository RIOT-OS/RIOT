"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

import contextlib
import sys
from pathlib import Path

__all__ = ["FileOrStdoutResource"]

class FileOrStdoutResource(contextlib.AbstractContextManager):
    """
    A context manager that provides a file or stdout resource.
    If the output_file_prefix is None, it uses stdout.
    Otherwise, it creates a file with the given prefix.
    This may be used for plugins that need to write output to a file
    or to stdout, depending on the configuration.
    """

    def __init__(self, output_file_prefix: Path | None, output_file_suffix: str = ".txt"):
        """
        Initializes the FileOrStdoutResource.
        :param output_file_prefix: The prefix for the output file. If None, stdout is used.
        :param output_file_suffix: The suffix for the output file. Default is ".txt".
        """
        self.output_file_prefix = output_file_prefix
        self.output_file_suffix = output_file_suffix
        if self.output_file_prefix is not None:
            self.output_file_name = self.output_file_prefix.with_suffix(self.output_file_suffix)
        else:
            self.output_file_name = None
        self.output_file = None

    def __enter__(self):
        if self.output_file is not None:
            raise RuntimeError("Output file already opened")
        if self.output_file_name is None:
            return sys.stdout
        self.output_file = self.output_file_name.open('wt')
        return self.output_file

    def __exit__(self, *_):
        if self.output_file is not None:
            self.output_file.close()
