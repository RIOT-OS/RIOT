"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

from riot_sbom.processing.plugin_type import Plugin

class TestPlugin1_1(Plugin):
    """
    Plugin subclass for testing purposes.
    """
    def __init__(self):
        super().__init__()
        self._name = "TestPlugin1_1"
        self._description = "Test plugin for testing purposes."

    def get_name(self):
        """
        Get the name of the plugin.
        """
        return self._name

    def get_description(self):
        """
        Get the description of the plugin.
        """
        return self._description

    def run(self, app_info, output_file_prefix):
        # This is a dummy implementation for testing purposes.
        # In a real plugin, this method would process the application information and write the output to a file.
        print(f"Running {self._name} on {app_info} with output file prefix {output_file_prefix}")
        return app_info


class TestPlugin1_2(TestPlugin1_1):
    """
    Plugin subclass for testing purposes.
    """
    def __init__(self):
        self._name = "TestPlugin1_2"
        self._description = "Test plugin for testing purposes."
