#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# SPDX-FileCopyrightText: 2020 ARM Limited or its affiliates
# SPDX-License-Identifier: Apache-2.0

import setuptools
import os
import suit_tool

with open('README.md', 'r') as fd:
    long_description = fd.read()

if os.name == 'nt':
    entry_points={
        "console_scripts": [
            "suit-tool=suit_tool.clidriver:main",
        ],
    }
    scripts = []
else:
    platform_deps = []
    # entry points are nice, but add ~100ms to startup time with all the
    # pkg_resources infrastructure, so we use scripts= instead on unix-y
    # platforms:
    scripts = ['bin/suit-tool', ]
    entry_points = {}

setuptools.setup (
                name = 'ietf-suit-tool',
             version = suit_tool.__version__,
              author = 'Brendan Moran',
        author_email = 'brendan.moran@arm.com',
         description = 'A tool for constructing SUIT manifests',
    long_description = long_description,
                 url = 'https://github.com/ARMmbed/suit-manifest-generator',
            packages = setuptools.find_packages(exclude=['examples*', 'parser_examples*', '.git*']),
     python_requires ='>=3.6',
             scripts = scripts,
        entry_points = entry_points,
            zip_safe = False,
    install_requires = [
        'cbor2>=5.0.0',
        'colorama>=0.4.0',
        'cryptography>=2.8',
    ],
         classifiers = [
            "Programming Language :: Python :: 3",
            "License :: OSI Approved :: Apache Software License",
            "Development Status :: 3 - Alpha",
            "Operating System :: OS Independent"
         ],
    long_description_content_type = 'text/markdown'
)
