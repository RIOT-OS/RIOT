"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

import importlib.util
import io
import logging
logger = logging.getLogger(__name__)
import os
from pathlib import Path
import sys
from typing import Dict, List, TextIO
from types import ModuleType
import unittest

if __name__ == "__main__":
    # update search path for local testing
    import pathlib
    import os
    import sys
    pkg_path = pathlib.Path(__file__).absolute().parents[2].as_posix()
    os.environ["PYTHONPATH"] = ":".join(
        [pkg_path, os.environ.get("PYTHONPATH", "")]).strip(":")
    sys.path.insert(0, pkg_path)
    from riot_sbom.processing.plugin_type import Plugin
else:
    from .plugin_type import Plugin

__all__ = ["load_plugins_from_directory", "print_plugin_list",
              "get_plugin_names", "get_plugin", "register_plugin"]


_plugin_registry: Dict[str, Plugin] = {}


def _load_modules_from_directory(directory: Path, add_to_search_path=True) -> List[ModuleType]:
    """
    Load all modules from the specified directory.

    :param directory: The directory to load modules from.
    :type directory: str
    :return: A list of loaded modules.
    """
    modules = []
    for filename in os.listdir(directory):
        if filename.endswith('.py') and filename != '__init__.py':
            if add_to_search_path and directory.absolute().as_posix() not in sys.path:
                # allow loading of local libraries in plugin directory
                logger.debug(f"Adding {directory.absolute().as_posix()} to sys.path")
                sys.path.append(directory.absolute().as_posix())
            module_name = filename[:-3]
            module_path = os.path.join(directory, filename)
            try:
                spec = importlib.util.spec_from_file_location(module_name, module_path)
                if spec is None or spec.loader is None:
                    logger.warning(f"Could not load module {module_name} from {module_path}")
                    continue
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
                modules.append(module)
            except Exception as e:
                logger.warning(f"Failed to load module {module_name} from {module_path}: {e}")
                continue
            logger.debug(f"Loaded module: {module_name} from {module_path}")
    return modules


def register_plugin(name: str, plugin: Plugin) -> None:
    """
    Register a plugin with the given name.

    :param name: The name of the plugin.
    :type name: str
    :param plugin: The plugin instance.
    :type plugin: Plugin
    """
    logger.debug(f"Registering plugin: {name} to registry: {_plugin_registry}")
    if not name:
        raise ValueError("Plugin name cannot be empty.")
    if name in _plugin_registry:
        logger.debug(f"Plugin {name} is already registered. Skipping.")
    if not isinstance(plugin, Plugin):
        raise TypeError(f"Plugin {name} is not an instance of Plugin.")
    _plugin_registry[name] = plugin
    logger.info(f"Registered plugin: {name} - {plugin.get_description()}")


def _load_plugins_from_module(module: ModuleType) -> None:
    logger.debug(f"Loading plugins from module: {module.__name__}")
    for name, obj in vars(module).items():
        logger.debug(f"Checking {name} in module {module.__name__}")
        logger.debug(f"Object type: {type(obj)}")
        if isinstance(obj, type):
            logger.debug(f"Object {name} is a class")
            logger.debug(f"Class {name} is a subclass of Plugin: {issubclass(obj, Plugin)}")
            logger.debug(f"Class {name} is not Plugin: {obj is not Plugin}")
        if isinstance(obj, type) and issubclass(obj, Plugin) and obj is not Plugin:
            try:
                plugin_instance = obj()
            except Exception as e:
                import traceback
                logger.warning(f"Failed to instantiate plugin {name}: {traceback.format_exc()}")
                continue
            register_plugin(plugin_instance.get_name(), plugin_instance)


def load_plugins_from_directory(directory: Path) -> None:
    """
    Load all plugins from the specified directory.

    :param directory: The directory to load plugins from.
    :type directory: str
    :return: A list of loaded plugins.
    """
    modules = _load_modules_from_directory(directory)
    for module in modules:
        _load_plugins_from_module(module)


def print_plugin_list(target: TextIO = sys.stdout) -> None:
    """
    Print the list of available plugins.
    """
    print("Available plugins:", file=target)
    for name, plugin in _plugin_registry.items():
        print(f"---\nPLUGIN: {name}", file=target)
        print(f"DESCRIPTION: {plugin.get_description()}", file=target)


def get_plugin_names() -> List[str]:
    """
    Get the names of all available plugins.

    :return: A list of plugin names.
    """
    return list(_plugin_registry.keys())


def get_plugin(name: str) -> Plugin:
    """
    Get a plugin by its name.

    :param name: The name of the plugin.
    :type name: str
    :return: The plugin instance.
    """
    if name not in _plugin_registry:
        raise KeyError(f"Plugin {name} not found.")
    return _plugin_registry[name]


class TestPluginRegistry(unittest.TestCase):
    test_directory = Path(__file__).parent / "_test_plugins"

    def setUp(self):
        load_plugins_from_directory(TestPluginRegistry.test_directory)

    def tearDown(self):
        _plugin_registry.clear()

    def test_load_plugins_from_directory(self):
        _plugin_registry.clear()
        load_plugins_from_directory(TestPluginRegistry.test_directory)
        self.assertGreater(len(_plugin_registry), 0, "No plugins loaded from directory")

    def test_get_plugin_names(self):
        names = get_plugin_names()
        self.assertIsInstance(names, list, "Plugin names should be a list")
        self.assertGreater(len(names), 0, "No plugin names found")

    def test_get_plugin(self):
        names = get_plugin_names()
        if names:
            plugin = get_plugin(names[0])
            self.assertIsInstance(plugin, Plugin, "Retrieved object is not a Plugin instance")
        else:
            self.fail("No plugin names found to test get_plugin()")

    def test_print_plugin_list(self):
        text = io.StringIO()
        print_plugin_list(text)
        output = text.getvalue()
        self.assertIn("Available plugins:", output, "Plugin list should contain 'Available plugins:'")
        self.assertIn("PLUGIN:", output, "Plugin list should contain 'PLUGIN:'")
        self.assertIn("DESCRIPTION:", output, "Plugin list should contain 'DESCRIPTION:'")

    def test_plugin_registry(self):
        # Check if the plugin registry is not empty after loading
        self.assertEqual(len(_plugin_registry), 3, "Plugin registry should contain 3 plugins")

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    unittest.main()
