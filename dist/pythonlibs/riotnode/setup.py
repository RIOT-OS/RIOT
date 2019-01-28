#! /usr/bin/env python3

import os
from setuptools import setup, find_packages

PACKAGE = 'riotnode'
LICENSE = 'LGPLv2.1'
URL = 'https://github.com/RIOT-OS/RIOT'


def get_version(package):
    """ Extract package version without importing file
    Importing cause issues with coverage,
        (modules can be removed from sys.modules to prevent this)
    Importing __init__.py triggers importing rest and then requests too

    Inspired from pep8 setup.py
    """
    with open(os.path.join(package, '__init__.py')) as init_fd:
        for line in init_fd:
            if line.startswith('__version__'):
                return eval(line.split('=')[-1])  # pylint:disable=eval-used
    return None


setup(
    name=PACKAGE,
    version=get_version(PACKAGE),
    description='RIOTNode python abstraction',
    long_description=open('README.rst').read(),
    author='Gaëtan Harter',
    author_email='gaetan.harter@fu-berlin.de',
    url=URL,
    license=LICENSE,
    download_url=URL,
    packages=find_packages(),
    classifiers=['Development Status :: 2 - Pre-Alpha',
                 'Programming Language :: Python',
                 'Programming Language :: Python :: 3',
                 'Programming Language :: Python :: 3.5',
                 'Programming Language :: Python :: 3.6',
                 'Programming Language :: Python :: 3.7',
                 'Intended Audience :: End Users/Desktop',
                 'Environment :: Console',
                 'Topic :: Utilities', ],
    install_requires=[],
    python_requires='>=3.5',
)
