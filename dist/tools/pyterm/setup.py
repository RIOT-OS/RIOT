#!/usr/bin/env python3

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup

setup(name="pyterm",
      version="1.0",
      description="Python-based terminal emulator for serial communication",
      author="Oliver Hahm",
      author_email="ohahm@fu-berlin.de",
      url="http://riot-os.org/",
      install_requires=["pyserial"],
      scripts=["pyterm"])
