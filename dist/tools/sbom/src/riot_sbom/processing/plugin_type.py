"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

from pathlib import Path

from ..data.app_info import AppInfo


__all__ = ["Plugin"]


class Plugin:
    """
    A base class for plugins.
    This class must be inherited by all plugins.
    """

    def get_name(self):
        """
        Returns the name of the plugin.
        This method should be overridden by subclasses to provide the
        plugin's name.
        """
        raise NotImplementedError("Subclasses must implement this method")

    def get_description(self):
        """
        Returns a description of the plugin.
        This method should be overridden by subclasses to provide the
        plugin's description.
        """
        raise NotImplementedError("Subclasses must implement this method")

    def run(self, app_info: AppInfo, output_file_prefix: Path | None) -> AppInfo:
        """
        Runs the plugin on the given app_info and output_file_prefix.
        This method should be overridden by subclasses to implement the
        plugin's functionality.

        Implementations may choose to operate on a deep copy of the provided
        AppInfo object. Normally, the modified original app_info should be
        returned. Subsequent plugins should always receive the returned
        AppInfo object.

        :param app_info: The application information to process.
        :type app_info: AppInfo
        :param output_file_prefix: The prefix for the output files.
        :type output_file_prefix: Path | None
        :return: The processed application information.
        """
        raise NotImplementedError("Subclasses must implement this method")
