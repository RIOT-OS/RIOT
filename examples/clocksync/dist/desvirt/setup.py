#!/usr/bin/env python2

from distutils.core import setup
setup(name='des-virt',
      packages=['desvirt'],
      scripts=['vnet', 'topology_creator'],
      version='0.03',
      author='Kaspar Schleiser',
      author_email='kaspar.schleiser@fu-berlin.de',
      url='http://des-testbed.net',
      data_files=[('/etc/desvirt/', [])],      
      )
