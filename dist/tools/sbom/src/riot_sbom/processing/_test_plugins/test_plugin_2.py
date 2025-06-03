"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

import test_plugin_1

class TestPlugin2(test_plugin_1.TestPlugin1_1):
    """
    Plugin subclass for testing purposes.
    """
    def __init__(self):
        self._name = "TestPlugin2"
        self._description = "Test plugin for testing purposes."
