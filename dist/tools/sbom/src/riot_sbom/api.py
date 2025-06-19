"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

from .data.app_info import AppInfo
from .processing.build_scanner import BuildScanner
from .processing import plugin_registry
import logging
logger = logging.getLogger(__name__)
import pathlib
import pickle

def run_build(app_dir: pathlib.Path) -> AppInfo:
    """
    Run the build process for a RIOT application.

    :param app_dir: The directory of the RIOT application.
    """
    if not app_dir.is_dir():
        raise ValueError(f'{app_dir} is not a directory')
    if not app_dir.joinpath('Makefile').is_file():
        raise ValueError(f'{app_dir} does not contain a Makefile')
    scanner = BuildScanner(app_dir)
    logger.info(f'Running build scanner on {app_dir}')
    scanner.run()
    return scanner.get_app_info()

def load_app_info(file: pathlib.Path) -> AppInfo:
    """
    Load application information from a file.

    :param file: The file to load the application information from.
    """
    if not file.is_file():
        raise ValueError(f'{file} is not a file')
    with open(file, 'rb') as f:
        logger.info(f'Loading application information from {file}')
        app_info = pickle.load(f)
        if not isinstance(app_info, AppInfo):
            raise ValueError(f'{file} does not contain valid application information')
        return app_info

def save_app_info(app_info: AppInfo, file: pathlib.Path) -> None:
    """
    Save application information to a file.

    :param app_info: The application information to save.
    :param file: The file to save the application information to.
    """
    if not file.parent.is_dir():
        raise ValueError(f'{file.parent} is not a directory. Cannot create output file.')
    with open(file, 'wb') as f:
        logger.info(f'Saving application information to {file}')
        pickle.dump(app_info, f)

def run_plugin(plugin: str, app_info: AppInfo, output_file_prefix: pathlib.Path | None) -> AppInfo:
    """
    Run a processing plugin plugin on the application information.

    :param app_info: The application information to process.
    :param plugin: The name of the plugin to run.
    :param output_file: The file to write the output to.
    """
    plugin_instance = plugin_registry.get_plugin(plugin)
    if plugin_instance is None:
        raise ValueError(f'Plugin {plugin} not found')
    logger.info(f'Running plugin {plugin}')
    return plugin_instance.run(app_info, output_file_prefix)
